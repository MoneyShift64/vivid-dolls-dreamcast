/* Vivid Dolls port - libultra compact sequence player (alCSP*), following the
 * ROM's code (0x8001E370-0x8001F6E0 and the shared seqplayer helpers
 * 0x8002C33C-0x8002CB30): same structures, events and timing.
 *
 * The player is an ALPlayer of the synthesizer: its handler runs from
 * alAudioFrame at the time of its next event, handles every event due
 * (sequence MIDI events, note ends, envelope phases, oscillators, API calls),
 * and returns the time to the next event. A 16 ms API "heartbeat" event keeps
 * it running, so API calls (alCSPPlay...) take effect within 16 ms.
 */
#include <string.h>
#include "al_internal.h"
#include "vd_debug.h"

#define AL_PHASE_ATTACK  0
#define AL_PHASE_DECAY   1
#define AL_PHASE_RELEASE 3

#define AL_PHASE_NOTEON  0
#define AL_PHASE_SUSTAIN 2
#define AL_PHASE_RELEASE_ 3
#define AL_PHASE_SUSTREL 4

#define AL_SUSTAIN 63

static ALMicroTime __CSPVoiceHandler(void *node);
static void __CSPHandleNextSeqEvent(ALCSPlayer *seqp);
static void __CSPHandleMIDIMsg(ALCSPlayer *seqp, ALEvent *event);
static void __CSPHandleMetaMsg(ALCSPlayer *seqp, ALEvent *event);
static void __CSPRepostEvent(ALEventQueue *evtq, ALEventListItem *item);
static void __CSPPostNextSeqEvent(ALCSPlayer *seqp);
static void __setUsptFromTempo(ALCSPlayer *seqp, f32 tempo);

/* ------------------------------------------------ shared seqplayer helpers */

static ALVoiceState *__mapVoice(ALCSPlayer *seqp, u8 key, u8 vel, u8 channel)
{
    ALVoiceState *vs = seqp->vFreeList;

    if (vs) {
        seqp->vFreeList = vs->next;
        vs->next = 0;
        if (!seqp->vAllocHead)
            seqp->vAllocHead = vs;
        else
            seqp->vAllocTail->next = vs;
        seqp->vAllocTail = vs;
        vs->channel = channel;
        vs->key = key;
        vs->velocity = vel;
        vs->voice.clientPrivate = vs;
    }
    return vs;
}

static void __unmapVoice(ALCSPlayer *seqp, ALVoice *voice)
{
    ALVoiceState *prev = 0, *vs;

    for (vs = seqp->vAllocHead; vs != 0; vs = vs->next) {
        if (&vs->voice == voice) {
            if (prev)
                prev->next = vs->next;
            else
                seqp->vAllocHead = vs->next;
            if (vs == seqp->vAllocTail)
                seqp->vAllocTail = prev;
            vs->next = seqp->vFreeList;
            seqp->vFreeList = vs;
            return;
        }
        prev = vs;
    }
}

static ALVoiceState *__lookupVoice(ALCSPlayer *seqp, u8 key, u8 channel)
{
    ALVoiceState *vs;

    for (vs = seqp->vAllocHead; vs != 0; vs = vs->next) {
        if (vs->key == key && vs->channel == channel && vs->phase != AL_PHASE_RELEASE_ &&
            vs->phase != AL_PHASE_SUSTREL)
            return vs;
    }
    return 0;
}

/* binary search of the instrument's key/velocity maps */
static ALSound *__lookupSoundQuick(ALCSPlayer *seqp, u8 key, u8 vel, u8 chan)
{
    ALInstrument *inst = seqp->chanState[chan].instrument;
    s32 l = 1, r = inst->soundCount, i;
    ALKeyMap *keymap;

    while (r >= l) {
        i = (l + r) / 2;
        keymap = inst->soundArray[i - 1]->keyMap;
        if (key >= keymap->keyMin && key <= keymap->keyMax &&
            vel >= keymap->velocityMin && vel <= keymap->velocityMax)
            return inst->soundArray[i - 1];
        else if (key < keymap->keyMin || (vel < keymap->velocityMin && key <= keymap->keyMax))
            r = i - 1;
        else
            l = i + 1;
    }
    return 0;
}

static s16 __vsVol(ALVoiceState *vs, ALCSPlayer *seqp)
{
    u32 t1, t2;

    t1 = ((u32)vs->tremelo * vs->velocity * vs->envGain) >> 6;
    t2 = ((u32)vs->sound->sampleVolume * (u32)(s32)seqp->vol * seqp->chanState[vs->channel].vol) >> 14;
    t1 *= t2;
    t1 >>= 15;
    return (s16)t1;
}

static ALMicroTime __vsDelta(ALVoiceState *vs, ALMicroTime t)
{
    s32 delta = vs->envEndTime - t;

    return delta >= 0 ? delta : 1000;   /* AL_GAIN_CHANGE_TIME */
}

static ALPan __vsPan(ALVoiceState *vs, ALCSPlayer *seqp)
{
    s32 tmp = seqp->chanState[vs->channel].pan - AL_PAN_CENTER + vs->sound->samplePan;

    if (tmp < AL_PAN_LEFT)
        tmp = AL_PAN_LEFT;
    if (tmp > AL_PAN_RIGHT)
        tmp = AL_PAN_RIGHT;
    return (ALPan)tmp;
}

static void __seqpReleaseVoice(ALCSPlayer *seqp, ALVoice *voice, ALMicroTime deltaTime)
{
    ALEvent evt;
    ALEventListItem *item, *next;
    ALVoiceState *vs = voice->clientPrivate;

    /* in the attack phase: drop the pending envelope (decay) event */
    if (vs->envPhase == AL_PHASE_ATTACK) {
        for (item = (ALEventListItem *)seqp->evtq.allocList.next; item != 0; item = next) {
            next = (ALEventListItem *)item->node.next;
            if (item->evt.type == AL_SEQP_ENV_EVT && item->evt.msg.vol.voice == voice) {
                if (next)
                    next->delta += item->delta;
                alUnlink(item);
                alLink(item, &seqp->evtq.freeList);
            }
        }
    }
    vs->velocity = 0;
    vs->envPhase = AL_PHASE_RELEASE;
    vs->envGain = 0;
    vs->envEndTime = seqp->curTime + deltaTime;
    alSynSetPriority(seqp->drvr, voice, 0);
    alSynSetVol(seqp->drvr, voice, 0, deltaTime);
    evt.type = AL_NOTE_END_EVT;
    evt.msg.note.voice = voice;
    alEvtqPostEvent(&seqp->evtq, &evt, deltaTime);
}

/* TRUE if the voice's note-end event is later than killTime (it is then removed). */
static s32 __voiceNeedsNoteKill(ALCSPlayer *seqp, ALVoice *voice, ALMicroTime killTime)
{
    ALEventListItem *item, *next;
    ALMicroTime itemTime = 0;
    s32 needsNoteKill = 1;

    for (item = (ALEventListItem *)seqp->evtq.allocList.next; item != 0; item = next) {
        next = (ALEventListItem *)item->node.next;
        itemTime += item->delta;
        if (item->evt.type == AL_NOTE_END_EVT && item->evt.msg.note.voice == voice) {
            if (itemTime > killTime) {
                if (next)
                    next->delta += item->delta;
                alUnlink(item);
                alLink(item, &seqp->evtq.freeList);
            } else {
                needsNoteKill = 0;
            }
            break;
        }
    }
    return needsNoteKill;
}

static void __resetPerfChanState(ALCSPlayer *seqp, s32 chan)
{
    ALChanState *cs = &seqp->chanState[chan];

    cs->fxId = 0;
    cs->fxmix = 0;
    cs->pan = AL_PAN_CENTER;
    cs->vol = AL_VOL_FULL;
    cs->priority = 5;
    cs->sustain = 0;
    cs->bendRange = 200;
    cs->pitchBend = 1.0f;
}

static void __setInstChanState(ALCSPlayer *seqp, ALInstrument *inst, s32 chan)
{
    ALChanState *cs = &seqp->chanState[chan];

    cs->instrument = inst;
    cs->pan = inst->pan;
    cs->vol = inst->volume;
    cs->priority = inst->priority;
    cs->bendRange = inst->bendRange;
}

static void __initChanState(ALCSPlayer *seqp)
{
    s32 i;

    for (i = 0; i < seqp->maxChannels; i++) {
        seqp->chanState[i].instrument = 0;
        __resetPerfChanState(seqp, i);
    }
}

static void __initFromBank(ALCSPlayer *seqp, ALBank *b)
{
    s32 i;
    ALInstrument *inst = 0;

    /* the first non-NULL instrument (the ROM loops until it finds one) */
    for (i = 0; inst == 0; i++)
        inst = b->instArray[i];
    for (i = 0; i < seqp->maxChannels; i++) {
        __resetPerfChanState(seqp, i);
        __setInstChanState(seqp, inst, i);
    }
    if (b->percussion) {
        /* ROM quirk kept: resets chanState[maxChannels] (one past the end; the
           port allocates one spare entry), then sets channel 9 */
        __resetPerfChanState(seqp, i);
        __setInstChanState(seqp, b->percussion, 9);
    }
}

static void __seqpStopOsc(ALCSPlayer *seqp, ALVoiceState *vs)
{
    ALEventListItem *item, *next;
    s16 type;

    for (item = (ALEventListItem *)seqp->evtq.allocList.next; item != 0; item = next) {
        next = (ALEventListItem *)item->node.next;
        type = item->evt.type;
        if ((type == AL_TREM_OSC_EVT || type == AL_VIB_OSC_EVT) && item->evt.msg.osc.vs == vs) {
            (*seqp->stopOsc)(item->evt.msg.osc.oscState);
            alUnlink(item);
            if (next)
                next->delta += item->delta;
            alLink(item, &seqp->evtq.freeList);
            if (type == AL_TREM_OSC_EVT)
                vs->flags &= ~1;
            else
                vs->flags &= ~2;
            if (!vs->flags)
                return;
        }
    }
}

/* ------------------------------------------------------------- the player */

void alCSPNew(void *seqpv, void *cv)
{
    ALCSPlayer *seqp = seqpv;
    ALSeqpConfig *c = cv;
    s32 i;
    ALEventListItem *items;
    ALVoiceState *vs, *voices;
    void *hp = c->heap;

    seqp->bank = 0;
    seqp->target = 0;
    seqp->drvr = (ALSynth *)alGlobals;
    seqp->chanMask = 0xFF;
    seqp->uspt = 488;
    seqp->nextDelta = 0;
    seqp->state = AL_STOPPED;
    seqp->vol = 0x7FFF;
    seqp->frameTime = 16000;           /* AL_USEC_PER_FRAME */
    seqp->curTime = 0;
    seqp->initOsc = (ALOscInit)c->initOsc;
    seqp->updateOsc = (ALOscUpdate)c->updateOsc;
    seqp->stopOsc = (ALOscStop)c->stopOsc;
    seqp->nextEvent.type = AL_SEQP_API_EVT;
    seqp->maxChannels = c->maxChannels;
    seqp->debugFlags = c->debugFlags;
    /* one spare channel entry for the __initFromBank quirk */
    seqp->chanState = alHeapAlloc(hp, c->maxChannels + 1, sizeof(ALChanState));
    __initChanState(seqp);

    voices = alHeapAlloc(hp, c->maxVoices, sizeof(ALVoiceState));
    seqp->vFreeList = 0;
    for (i = 0; i < c->maxVoices; i++) {
        vs = &voices[i];
        vs->next = seqp->vFreeList;
        seqp->vFreeList = vs;
    }
    seqp->vAllocHead = 0;
    seqp->vAllocTail = 0;

    items = alHeapAlloc(hp, c->maxEvents, sizeof(ALEventListItem));
    alEvtqNew(&seqp->evtq, items, c->maxEvents);

    seqp->node.next = 0;
    seqp->node.handler = __CSPVoiceHandler;
    seqp->node.clientData = seqp;
    alSynAddPlayer(seqp->drvr, &seqp->node);
}

static ALMicroTime __CSPVoiceHandler(void *node)
{
    ALCSPlayer *seqp = node;
    ALEvent evt;
    ALVoice *voice;
    ALMicroTime delta;
    ALVoiceState *vs;
    void *oscState;
    f32 oscValue;
    u8 chan;

    do {
        switch (seqp->nextEvent.type) {
        case AL_SEQ_REF_EVT:
            __CSPHandleNextSeqEvent(seqp);
            break;

        case AL_SEQP_API_EVT:
            evt.type = AL_SEQP_API_EVT;
            alEvtqPostEvent(&seqp->evtq, &evt, seqp->frameTime);
            break;

        case AL_NOTE_END_EVT:
            voice = seqp->nextEvent.msg.note.voice;
            alSynStopVoice(seqp->drvr, voice);
            alSynFreeVoice(seqp->drvr, voice);
            vs = voice->clientPrivate;
            if (vs->flags)
                __seqpStopOsc(seqp, vs);
            __unmapVoice(seqp, voice);
            break;

        case AL_SEQP_ENV_EVT:
            voice = seqp->nextEvent.msg.vol.voice;
            vs = voice->clientPrivate;
            if (vs->envPhase == AL_PHASE_ATTACK)
                vs->envPhase = AL_PHASE_DECAY;
            delta = seqp->nextEvent.msg.vol.delta;
            vs->envEndTime = seqp->curTime + delta;
            vs->envGain = seqp->nextEvent.msg.vol.vol;
            alSynSetVol(seqp->drvr, voice, __vsVol(vs, seqp), delta);
            break;

        case AL_TREM_OSC_EVT:
            vs = seqp->nextEvent.msg.osc.vs;
            oscState = seqp->nextEvent.msg.osc.oscState;
            delta = (*seqp->updateOsc)(oscState, &oscValue);
            vs->tremelo = (u8)(u32)oscValue;
            alSynSetVol(seqp->drvr, &vs->voice, __vsVol(vs, seqp), __vsDelta(vs, seqp->curTime));
            evt.type = AL_TREM_OSC_EVT;
            evt.msg.osc.vs = vs;
            evt.msg.osc.oscState = oscState;
            alEvtqPostEvent(&seqp->evtq, &evt, delta);
            break;

        case AL_VIB_OSC_EVT:
            vs = seqp->nextEvent.msg.osc.vs;
            oscState = seqp->nextEvent.msg.osc.oscState;
            chan = seqp->nextEvent.msg.osc.chan;
            delta = (*seqp->updateOsc)(oscState, &oscValue);
            vs->vibrato = oscValue;
            alSynSetPitch(seqp->drvr, &vs->voice, vs->pitch * vs->vibrato * seqp->chanState[chan].pitchBend);
            evt.type = AL_VIB_OSC_EVT;
            evt.msg.osc.vs = vs;
            evt.msg.osc.oscState = oscState;
            evt.msg.osc.chan = chan;
            alEvtqPostEvent(&seqp->evtq, &evt, delta);
            break;

        case AL_SEQP_MIDI_EVT:
        case AL_CSP_NOTEOFF_EVT:
            __CSPHandleMIDIMsg(seqp, &seqp->nextEvent);
            break;

        case AL_SEQP_META_EVT:
            __CSPHandleMetaMsg(seqp, &seqp->nextEvent);
            break;

        case AL_SEQP_VOL_EVT:
            seqp->vol = seqp->nextEvent.msg.spvol.vol;
            for (vs = seqp->vAllocHead; vs != 0; vs = vs->next)
                alSynSetVol(seqp->drvr, &vs->voice, __vsVol(vs, seqp), __vsDelta(vs, seqp->curTime));
            break;

        case AL_SEQP_PRIORITY_EVT:
            chan = seqp->nextEvent.msg.sppriority.chan;
            seqp->chanState[chan].priority = seqp->nextEvent.msg.sppriority.priority;
            break;

        case AL_SEQP_SEQ_EVT:
            seqp->target = seqp->nextEvent.msg.spseq.seq;
            __setUsptFromTempo(seqp, 500000.0f);
            if (seqp->bank)
                __initFromBank(seqp, seqp->bank);
            break;

        case AL_SEQP_BANK_EVT:
            seqp->bank = seqp->nextEvent.msg.spbank.bank;
            __initFromBank(seqp, seqp->bank);
            break;

        case AL_SEQP_PLAY_EVT:
            if (seqp->state != AL_PLAYING) {
                seqp->state = AL_PLAYING;
                vd_audio_stats.seq_play++;
                VD_LOG(VD_DBG_AUDIO, "csp: play (seq %p)\n", (void *)seqp->target);
                __CSPPostNextSeqEvent(seqp);
            }
            break;

        case AL_SEQP_STOP_EVT:
            if (seqp->state == AL_STOPPING) {
                for (vs = seqp->vAllocHead; vs != 0; vs = seqp->vAllocHead) {
                    voice = &vs->voice;
                    alSynStopVoice(seqp->drvr, voice);
                    alSynFreeVoice(seqp->drvr, voice);
                    if (vs->flags)
                        __seqpStopOsc(seqp, vs);
                    __unmapVoice(seqp, voice);
                }
                seqp->state = AL_STOPPED;
                vd_audio_stats.seq_stop++;
                VD_LOG(VD_DBG_AUDIO, "csp: stopped\n");
            }
            break;

        case AL_SEQP_STOPPING_EVT:
            if (seqp->state == AL_PLAYING) {
                /* drop the pending sequence events and release the notes
                   (quickly: 50 ms, unless they end sooner anyway) */
                alEvtqFlushType(&seqp->evtq, AL_SEQ_REF_EVT);
                alEvtqFlushType(&seqp->evtq, AL_CSP_NOTEOFF_EVT);
                alEvtqFlushType(&seqp->evtq, AL_SEQP_MIDI_EVT);
                for (vs = seqp->vAllocHead; vs != 0; vs = vs->next) {
                    if (__voiceNeedsNoteKill(seqp, &vs->voice, 50000))
                        __seqpReleaseVoice(seqp, &vs->voice, 50000);
                }
                seqp->state = AL_STOPPING;
                VD_LOG(VD_DBG_AUDIO, "csp: stopping\n");
                evt.type = AL_SEQP_STOP_EVT;
                alEvtqPostEvent(&seqp->evtq, &evt, AL_EVTQ_END);
            }
            break;

        default:
            break;
        }
        seqp->nextDelta = alEvtqNextEvent(&seqp->evtq, &seqp->nextEvent);
    } while (seqp->nextDelta == 0);

    seqp->curTime += seqp->nextDelta;
    return seqp->nextDelta;
}

static void __CSPHandleNextSeqEvent(ALCSPlayer *seqp)
{
    ALEvent evt;

    if (seqp->target == 0)
        return;
    evt.type = -1;
    alCSeqNextEvent(seqp->target, &evt);
    switch (evt.type) {
    case AL_SEQ_MIDI_EVT:
        __CSPHandleMIDIMsg(seqp, &evt);
        __CSPPostNextSeqEvent(seqp);
        break;
    case AL_TEMPO_EVT:
        __CSPHandleMetaMsg(seqp, &evt);
        __CSPPostNextSeqEvent(seqp);
        break;
    case AL_SEQ_END_EVT:
        seqp->state = AL_STOPPING;
        VD_LOG(VD_DBG_AUDIO, "csp: sequence end\n");
        evt.type = AL_SEQP_STOP_EVT;
        alEvtqPostEvent(&seqp->evtq, &evt, AL_EVTQ_END);
        break;
    case AL_TRACK_END:
    case AL_CSP_LOOPSTART:
    case AL_CSP_LOOPEND:
        __CSPPostNextSeqEvent(seqp);
        break;
    default:
        break;
    }
}

static void __CSPPostNextSeqEvent(ALCSPlayer *seqp)
{
    ALEvent evt;
    s32 deltaTicks;

    if (seqp->state != AL_PLAYING || seqp->target == 0)
        return;
    if (!__alCSeqNextDelta(seqp->target, &deltaTicks))
        return;
    evt.type = AL_SEQ_REF_EVT;
    alEvtqPostEvent(&seqp->evtq, &evt, deltaTicks * seqp->uspt);
}

static void __CSPHandleMIDIMsg(ALCSPlayer *seqp, ALEvent *event)
{
    ALVoice *voice;
    ALVoiceState *vs;
    s32 status;
    u8 chan, key, vel, byte1, byte2;
    ALSound *sound;
    s16 vol;
    ALEvent evt;
    ALMicroTime deltaTime;
    ALVoiceState *vstate;
    ALPan pan;
    ALInstrument *inst;
    ALVoiceConfig config;
    void *oscState;
    f32 oscValue;
    f32 pitch;
    s32 delta;
    s32 cents;
    u8 fxmix;

    status = event->msg.midi.status & 0xF0;
    chan = event->msg.midi.status & 0x0F;
    byte1 = key = event->msg.midi.byte1;
    byte2 = vel = event->msg.midi.byte2;

    switch (status) {
    case 0x90:                                   /* note on */
        if (vel != 0) {
            if (seqp->state != AL_PLAYING)
                break;
            sound = __lookupSoundQuick(seqp, key, vel, chan);
            if (sound == 0)
                break;
            config.priority = seqp->chanState[chan].priority;
            config.fxBus = 0;
            config.unityPitch = 0;
            vstate = __mapVoice(seqp, key, vel, chan);
            if (vstate == 0)
                break;
            voice = &vstate->voice;
            alSynAllocVoice(seqp->drvr, voice, &config);
            vstate->sound = sound;
            vstate->envPhase = AL_PHASE_ATTACK;
            if (seqp->chanState[chan].sustain > AL_SUSTAIN)
                vstate->phase = AL_PHASE_SUSTAIN;
            else
                vstate->phase = AL_PHASE_NOTEON;
            cents = (key - sound->keyMap->keyBase) * 100 + sound->keyMap->detune;
            vstate->pitch = alCents2Ratio((s16)cents);
            vstate->envGain = sound->envelope->attackVolume;
            vstate->flags = 0;
            vstate->envEndTime = seqp->curTime + sound->envelope->attackTime;

            inst = seqp->chanState[chan].instrument;
            oscValue = 127.0f;                   /* AL_VOL_FULL */
            if (inst->tremType && seqp->initOsc) {
                delta = (*seqp->initOsc)(&oscState, &oscValue, inst->tremType, inst->tremRate,
                                         inst->tremDepth, inst->tremDelay);
                if (delta) {
                    evt.type = AL_TREM_OSC_EVT;
                    evt.msg.osc.vs = vstate;
                    evt.msg.osc.oscState = oscState;
                    alEvtqPostEvent(&seqp->evtq, &evt, delta);
                    vstate->flags |= 1;
                }
            }
            vstate->tremelo = (u8)(u32)oscValue;
            oscValue = 1.0f;
            if (inst->vibType && seqp->initOsc) {
                delta = (*seqp->initOsc)(&oscState, &oscValue, inst->vibType, inst->vibRate,
                                         inst->vibDepth, inst->vibDelay);
                if (delta) {
                    evt.type = AL_VIB_OSC_EVT;
                    evt.msg.osc.vs = vstate;
                    evt.msg.osc.oscState = oscState;
                    evt.msg.osc.chan = chan;
                    alEvtqPostEvent(&seqp->evtq, &evt, delta);
                    vstate->flags |= 2;
                }
            }
            vstate->vibrato = oscValue;

            pitch = vstate->pitch * seqp->chanState[chan].pitchBend * vstate->vibrato;
            fxmix = seqp->chanState[chan].fxmix;
            pan = __vsPan(vstate, seqp);
            vol = __vsVol(vstate, seqp);
            deltaTime = sound->envelope->attackTime;
            alSynStartVoiceParams(seqp->drvr, voice, sound->wavetable, pitch, vol, pan, fxmix, deltaTime);
            vd_audio_stats.notes++;

            /* the decay phase starts after the attack */
            evt.type = AL_SEQP_ENV_EVT;
            evt.msg.vol.voice = voice;
            evt.msg.vol.vol = sound->envelope->decayVolume;
            evt.msg.vol.delta = sound->envelope->decayTime;
            alEvtqPostEvent(&seqp->evtq, &evt, deltaTime);

            /* compact MIDI: the note-off comes from the note's duration */
            if (event->msg.midi.duration) {
                evt.type = AL_CSP_NOTEOFF_EVT;
                evt.msg.midi.status = chan | 0x80;
                evt.msg.midi.byte1 = key;
                evt.msg.midi.byte2 = 0;
                deltaTime = seqp->uspt * event->msg.midi.duration;
                alEvtqPostEvent(&seqp->evtq, &evt, deltaTime);
            }
            break;
        }
        /* note on with velocity 0 = note off */
        /* fall through */
    case 0x80:                                   /* note off */
        vs = __lookupVoice(seqp, key, chan);
        if (vs == 0)
            break;
        if (vs->phase == AL_PHASE_SUSTAIN) {
            vs->phase = AL_PHASE_SUSTREL;
        } else {
            vs->phase = AL_PHASE_RELEASE_;
            __seqpReleaseVoice(seqp, &vs->voice, vs->sound->envelope->releaseTime);
        }
        break;

    case 0xA0:                                   /* polyphonic pressure */
        vs = __lookupVoice(seqp, key, chan);
        if (vs == 0)
            break;
        vs->velocity = vel;
        alSynSetVol(seqp->drvr, &vs->voice, __vsVol(vs, seqp), __vsDelta(vs, seqp->curTime));
        break;

    case 0xD0:                                   /* channel pressure */
        for (vs = seqp->vAllocHead; vs != 0; vs = vs->next) {
            if (vs->channel == chan) {
                vs->velocity = byte1;
                alSynSetVol(seqp->drvr, &vs->voice, __vsVol(vs, seqp), __vsDelta(vs, seqp->curTime));
            }
        }
        break;

    case 0xB0:                                   /* controllers */
        switch (byte1) {
        case 10:                                 /* pan */
            seqp->chanState[chan].pan = byte2;
            for (vs = seqp->vAllocHead; vs != 0; vs = vs->next) {
                if (vs->channel == chan)
                    alSynSetPan(seqp->drvr, &vs->voice, __vsPan(vs, seqp));
            }
            break;
        case 7:                                  /* volume */
            seqp->chanState[chan].vol = byte2;
            for (vs = seqp->vAllocHead; vs != 0; vs = vs->next) {
                if (vs->channel == chan && vs->envPhase != AL_PHASE_RELEASE)
                    alSynSetVol(seqp->drvr, &vs->voice, __vsVol(vs, seqp), __vsDelta(vs, seqp->curTime));
            }
            break;
        case 0x10:                               /* priority */
            seqp->chanState[chan].priority = byte2;
            break;
        case 0x40:                               /* sustain pedal */
            seqp->chanState[chan].sustain = byte2;
            for (vs = seqp->vAllocHead; vs != 0; vs = vs->next) {
                if (vs->channel == chan && vs->phase != AL_PHASE_RELEASE_) {
                    if (byte2 > AL_SUSTAIN) {
                        if (vs->phase == AL_PHASE_NOTEON)
                            vs->phase = AL_PHASE_SUSTAIN;
                    } else if (vs->phase == AL_PHASE_SUSTAIN) {
                        vs->phase = AL_PHASE_NOTEON;
                    } else if (vs->phase == AL_PHASE_SUSTREL) {
                        vs->phase = AL_PHASE_RELEASE_;
                        __seqpReleaseVoice(seqp, &vs->voice, vs->sound->envelope->releaseTime);
                    }
                }
            }
            break;
        case 0x5B:                               /* effect (reverb) mix */
            seqp->chanState[chan].fxmix = byte2;
            for (vs = seqp->vAllocHead; vs != 0; vs = vs->next) {
                if (vs->channel == chan)
                    alSynSetFXMix(seqp->drvr, &vs->voice, byte2);
            }
            break;
        default:
            break;
        }
        break;

    case 0xC0:                                   /* program change */
        /* (an empty program slot would crash the original; skipped here) */
        if (key < seqp->bank->instCount && seqp->bank->instArray[key] != 0)
            __setInstChanState(seqp, seqp->bank->instArray[key], chan);
        break;

    case 0xE0:                                   /* pitch bend */
        cents = (s32)seqp->chanState[chan].bendRange * (((s32)byte2 << 7) + byte1 - 8192);
        cents /= 8192;                           /* (rounds toward zero, as the ROM) */
        seqp->chanState[chan].pitchBend = alCents2Ratio(cents);
        for (vs = seqp->vAllocHead; vs != 0; vs = vs->next) {
            if (vs->channel == chan)
                alSynSetPitch(seqp->drvr, &vs->voice,
                              vs->pitch * seqp->chanState[chan].pitchBend * vs->vibrato);
        }
        break;

    default:
        break;
    }
}

/* Tempo change: new uspt, and re-time the pending note-offs (kept in ticks). */
static void __CSPHandleMetaMsg(ALCSPlayer *seqp, ALEvent *event)
{
    s32 tempo, oldUspt;
    u32 ticks;
    ALMicroTime tempDelta, curDelta = 0;
    ALEventListItem *thisNode, *nextNode, *firstTemp = 0;

    if (event->msg.tempo.status != 0xFF || event->msg.tempo.type != 0x51)
        return;
    tempo = (event->msg.tempo.byte1 << 16) | (event->msg.tempo.byte2 << 8) | event->msg.tempo.byte3;
    oldUspt = seqp->uspt;
    __setUsptFromTempo(seqp, (f32)tempo);

    thisNode = (ALEventListItem *)seqp->evtq.allocList.next;
    while (thisNode) {
        curDelta += thisNode->delta;
        nextNode = (ALEventListItem *)thisNode->node.next;
        if (thisNode->evt.type == AL_CSP_NOTEOFF_EVT) {
            alUnlink(thisNode);
            if (firstTemp)
                alLink(thisNode, firstTemp);
            else {
                thisNode->node.next = 0;
                thisNode->node.prev = 0;
                firstTemp = thisNode;
            }
            tempDelta = curDelta;         /* absolute time of the event */
            if (nextNode) {
                curDelta -= thisNode->delta;
                nextNode->delta += thisNode->delta;
            }
            thisNode->delta = tempDelta;
        }
        thisNode = nextNode;
    }
    thisNode = firstTemp;
    while (thisNode) {
        nextNode = (ALEventListItem *)thisNode->node.next;
        ticks = thisNode->delta / oldUspt;
        thisNode->delta = ticks * seqp->uspt;
        __CSPRepostEvent(&seqp->evtq, thisNode);
        thisNode = nextNode;
    }
}

static void __CSPRepostEvent(ALEventQueue *evtq, ALEventListItem *item)
{
    ALLink *node;
    ALEventListItem *nextItem;
    irq_mask_t s = irq_disable();

    for (node = &evtq->allocList; node != 0; node = node->next) {
        if (!node->next) {
            alLink(item, node);
            break;
        }
        nextItem = (ALEventListItem *)node->next;
        if (item->delta < nextItem->delta) {
            nextItem->delta -= item->delta;
            alLink(item, node);
            break;
        }
        item->delta -= nextItem->delta;
    }
    irq_restore(s);
}

static void __setUsptFromTempo(ALCSPlayer *seqp, f32 tempo)
{
    if (seqp->target)
        seqp->uspt = (s32)(tempo * seqp->target->qnpt);
    else
        seqp->uspt = 488;                       /* 1/(2 * 120 bpm * 480 ppq / 60 s) */
}

/* ---------------------------------------------------------------- API */
static void csp_post(ALCSPlayer *seqp, s16 type, u32 w0)
{
    ALEvent evt;

    memset(&evt, 0, sizeof(evt));
    evt.type = type;
    evt.msg.raw[0] = w0;
    alEvtqPostEvent(&seqp->evtq, &evt, 0);
}

s16 alCSPGetVol(void *p)             { return ((ALCSPlayer *)p)->vol; }
s32 alCSPGetState(void *p)           { return ((ALCSPlayer *)p)->state; }
void alCSPPlay(void *p)              { csp_post(p, AL_SEQP_PLAY_EVT, 0); }
void alCSPStop(void *p)              { csp_post(p, AL_SEQP_STOPPING_EVT, 0); }
void alCSPSetBank(void *p, void *b)  { csp_post(p, AL_SEQP_BANK_EVT, (u32)(uintptr_t)b); }
void alCSPSetSeq(void *p, void *seq) { csp_post(p, AL_SEQP_SEQ_EVT, (u32)(uintptr_t)seq); }

void alCSPSetVol(void *p, s16 vol)
{
    ALEvent evt;

    memset(&evt, 0, sizeof(evt));
    evt.type = AL_SEQP_VOL_EVT;
    evt.msg.spvol.vol = vol;
    alEvtqPostEvent(&((ALCSPlayer *)p)->evtq, &evt, 0);
}

VD_STATIC_ASSERT(sizeof(ALCSPlayer) == 0x7C, alcsplayer_size);
VD_STATIC_ASSERT(sizeof(ALVoiceState) == 0x38, alvoicestate_size);
VD_STATIC_ASSERT(sizeof(ALChanState) == 0x10, alchanstate_size);
VD_STATIC_ASSERT(sizeof(ALEvent) == 0x10, alevent_size);
VD_STATIC_ASSERT(sizeof(ALEventListItem) == 0x1C, aleventlistitem_size);

VD_ALIAS(func_8001E370, alCSPGetVol);
VD_ALIAS(func_8001E380, alCSPGetState);
VD_ALIAS(func_8001E390, alCSPPlay);
VD_ALIAS(func_8001E3C0, alCSPSetBank);
VD_ALIAS(func_8001E3F0, alCSPSetSeq);
VD_ALIAS(func_8001E420, alCSPSetVol);
VD_ALIAS(func_8001E450, alCSPStop);
VD_ALIAS(func_8001E480, alCSPNew);
