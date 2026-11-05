/* Vivid Dolls port - libultra sound player (alSndp*), following the ROM's code
 * (0x8001F6E0-0x80020150): same structures, events and timing.
 *
 * A sound plays its envelope: attack (volume ramp to attackVolume over
 * attackTime), decay (to decayVolume over decayTime, unless decayTime is -1 =
 * hold until stopped), then a stop = release ramp to 0 over releaseTime, after
 * which the voice is freed and the sound reports AL_STOPPED. In this ROM's
 * version the decay/release times are divided by the sound's pitch (a sound
 * played at half speed keeps its envelope in step with the sample).
 * API calls post events; the player handles them on its next 16 ms tick.
 */
#include <string.h>
#include "al_internal.h"
#include "vd_debug.h"

enum {
    AL_SNDP_PLAY_EVT, AL_SNDP_STOP_EVT, AL_SNDP_PAN_EVT, AL_SNDP_VOL_EVT, AL_SNDP_PITCH_EVT,
    AL_SNDP_API_EVT, AL_SNDP_DECAY_EVT, AL_SNDP_END_EVT, AL_SNDP_FX_EVT
};

static ALMicroTime _DivS32ByF32(s32 i, f32 f)
{
    f32 r = (f32)i / f;

    if ((f64)r > 2147483647.0)
        return 0x7FFFFFFF;
    return (ALMicroTime)r;
}

static void _removeEvents(ALEventQueue *evtq, ALSoundState *state)
{
    ALEventListItem *item, *next;
    irq_mask_t s = irq_disable();

    for (item = (ALEventListItem *)evtq->allocList.next; item != 0; item = next) {
        next = (ALEventListItem *)item->node.next;
        if (item->evt.msg.snd.state == state) {
            if (next)
                next->delta += item->delta;
            alUnlink(item);
            alLink(item, &evtq->freeList);
        }
    }
    irq_restore(s);
}

static s32 snd_pan(const ALSoundState *state, const ALSound *snd)
{
    s32 tmp = state->pan - AL_PAN_CENTER + snd->samplePan;

    if (tmp < 0)
        tmp = 0;
    if (tmp > 127)
        tmp = 127;
    return tmp;
}

static void post_state_evt(ALSndPlayer *sndp, s16 type, ALSoundState *state, ALMicroTime delta)
{
    ALEvent evt;

    evt.type = type;
    evt.msg.snd.state = state;
    alEvtqPostEvent(&sndp->evtq, &evt, delta);
}

static void _handleEvent(ALSndPlayer *sndp, ALEvent *event)
{
    ALSoundState *state = event->msg.snd.state;
    ALSound *snd = state->sound;
    ALVoiceConfig vc;
    ALMicroTime delta;
    s16 vol;

    switch (event->type) {
    case AL_SNDP_PLAY_EVT:
        if (state->state != AL_STOPPED || snd == 0)
            break;
        vc.priority = state->priority;
        vc.fxBus = 0;
        vc.unityPitch = 0;
        alSynAllocVoice(sndp->drvr, &state->voice, &vc);
        vol = (s16)((s32)snd->envelope->attackVolume * state->vol / AL_VOL_FULL);
        delta = snd->envelope->attackTime;
        alSynStartVoice(sndp->drvr, &state->voice, snd->wavetable);
        state->state = AL_PLAYING;
        alSynSetPan(sndp->drvr, &state->voice, (ALPan)snd_pan(state, snd));
        alSynSetVol(sndp->drvr, &state->voice, vol, delta);
        alSynSetPitch(sndp->drvr, &state->voice, state->pitch);
        alSynSetFXMix(sndp->drvr, &state->voice, state->fxMix);
        post_state_evt(sndp, AL_SNDP_DECAY_EVT, state, _DivS32ByF32(snd->envelope->attackTime, state->pitch));
        vd_audio_stats.snd_play++;
        vd_audio_stats.last_snd_sound = (s32)(state - sndp->sndState);
        VD_LOG(VD_DBG_AUDIO, "sndp: play %d pitch %d/1000 vol %d\n", (int)(state - sndp->sndState),
               (int)(state->pitch * 1000.0f), (int)state->vol);
        break;

    case AL_SNDP_STOP_EVT:
        if (state->state != AL_PLAYING || snd == 0)
            break;
        delta = _DivS32ByF32(snd->envelope->releaseTime, state->pitch);
        alSynSetVol(sndp->drvr, &state->voice, 0, delta);
        if (delta) {
            post_state_evt(sndp, AL_SNDP_END_EVT, state, delta);
            state->state = AL_STOPPING;
            break;
        }
        goto end;

    case AL_SNDP_PAN_EVT:
        state->pan = event->msg.snd.v.pan;
        if (state->state == AL_PLAYING && snd != 0)
            alSynSetPan(sndp->drvr, &state->voice, (ALPan)snd_pan(state, snd));
        break;

    case AL_SNDP_VOL_EVT:
        state->vol = event->msg.snd.v.vol;
        if (state->state == AL_PLAYING && snd != 0) {
            vol = (s16)((s32)snd->envelope->decayVolume * state->vol / AL_VOL_FULL);
            alSynSetVol(sndp->drvr, &state->voice, vol, 1000);
        }
        break;

    case AL_SNDP_PITCH_EVT:
        state->pitch = event->msg.snd.v.pitch;
        if ((f64)state->pitch < 0.0001)
            state->pitch = 0.0001f;
        if (state->state == AL_PLAYING)
            alSynSetPitch(sndp->drvr, &state->voice, state->pitch);
        break;

    case AL_SNDP_DECAY_EVT:
        if (snd->envelope->decayTime != -1) {
            vol = (s16)((s32)snd->envelope->decayVolume * state->vol / AL_VOL_FULL);
            delta = _DivS32ByF32(snd->envelope->decayTime, state->pitch);
            alSynSetVol(sndp->drvr, &state->voice, vol, delta);
            post_state_evt(sndp, AL_SNDP_STOP_EVT, state, delta);
        }
        break;

    case AL_SNDP_END_EVT:
    end:
        alSynStopVoice(sndp->drvr, &state->voice);
        alSynFreeVoice(sndp->drvr, &state->voice);
        _removeEvents(&sndp->evtq, state);
        state->state = AL_STOPPED;
        vd_audio_stats.snd_end++;
        VD_LOG(VD_DBG_AUDIO, "sndp: end %d\n", (int)(state - sndp->sndState));
        break;

    case AL_SNDP_FX_EVT:
        state->fxMix = event->msg.snd.v.fxmix;
        if (state->state == AL_PLAYING)
            alSynSetFXMix(sndp->drvr, &state->voice, state->fxMix);
        break;

    default:
        break;
    }
}

static ALMicroTime _sndpVoiceHandler(void *node)
{
    ALSndPlayer *sndp = node;
    ALEvent evt;

    do {
        if (sndp->nextEvent.type == AL_SNDP_API_EVT) {
            evt.type = AL_SNDP_API_EVT;
            alEvtqPostEvent(&sndp->evtq, &evt, sndp->frameTime);
        } else {
            _handleEvent(sndp, &sndp->nextEvent);
        }
        sndp->nextDelta = alEvtqNextEvent(&sndp->evtq, &sndp->nextEvent);
    } while (sndp->nextDelta == 0);
    sndp->curTime += sndp->nextDelta;
    return sndp->nextDelta;
}

void alSndpNew(void *sndpv, void *cv)
{
    ALSndPlayer *sndp = sndpv;
    ALSndpConfig *c = cv;
    ALEventListItem *items;
    ALEvent evt;
    s32 i;

    sndp->target = -1;
    sndp->frameTime = 16000;
    sndp->maxSounds = c->maxSounds;
    sndp->sndState = alHeapAlloc(c->heap, 1, c->maxSounds * (s32)sizeof(ALSoundState));
    for (i = 0; i < c->maxSounds; i++)
        sndp->sndState[i].sound = 0;
    items = alHeapAlloc(c->heap, 1, c->maxEvents * (s32)sizeof(ALEventListItem));
    alEvtqNew(&sndp->evtq, items, c->maxEvents);
    sndp->drvr = (ALSynth *)alGlobals;
    sndp->node.next = 0;
    sndp->node.clientData = sndp;
    sndp->node.handler = _sndpVoiceHandler;
    sndp->curTime = 0;
    alSynAddPlayer(sndp->drvr, &sndp->node);
    evt.type = AL_SNDP_API_EVT;
    alEvtqPostEvent(&sndp->evtq, &evt, sndp->frameTime);
    sndp->nextDelta = alEvtqNextEvent(&sndp->evtq, &sndp->nextEvent);
}

s16 alSndpAllocate(void *sndpv, void *soundv)
{
    ALSndPlayer *sndp = sndpv;
    ALSound *sound = soundv;
    s32 i;

    for (i = 0; i < sndp->maxSounds; i++) {
        ALSoundState *s = &sndp->sndState[i];
        if (s->sound == 0) {
            s->sound = sound;
            s->priority = 5;                    /* AL_DEFAULT_PRIORITY */
            s->state = AL_STOPPED;
            s->pitch = 1.0f;
            s->pan = AL_PAN_CENTER;
            s->fxMix = 0;
            s->vol = (s16)((s32)sound->sampleVolume * 32767 / AL_VOL_FULL);
            return (s16)i;
        }
    }
    return -1;
}

void alSndpDeallocate(void *sndpv, s16 id)
{
    ALSndPlayer *sndp = sndpv;

    if (id < 0 || id >= sndp->maxSounds)
        return;
    if (sndp->sndState[id].state == AL_STOPPED) {
        sndp->sndState[id].sound = 0;
        if (sndp->target == id)
            sndp->target = -1;
    }
}

/* The target index is checked here (the ROM indexes sndState[-1] when no
   sound is selected; the game never does that). */
static ALSoundState *target_state(ALSndPlayer *sndp)
{
    if (sndp->target < 0 || sndp->target >= sndp->maxSounds)
        return 0;
    return &sndp->sndState[sndp->target];
}

void alSndpSetSound(void *sndpv, s16 id) { ((ALSndPlayer *)sndpv)->target = id; }
s16  alSndpGetSound(void *sndpv)         { return (s16)((ALSndPlayer *)sndpv)->target; }

void alSndpPlay(void *sndpv)
{
    ALSndPlayer *sndp = sndpv;
    ALSoundState *s = target_state(sndp);

    if (s && s->state == AL_STOPPED)
        post_state_evt(sndp, AL_SNDP_PLAY_EVT, s, 0);
}

void alSndpStop(void *sndpv)
{
    ALSndPlayer *sndp = sndpv;
    ALSoundState *s = target_state(sndp);

    if (s)
        post_state_evt(sndp, AL_SNDP_STOP_EVT, s, 0);
}

s32 alSndpGetState(void *sndpv)
{
    ALSoundState *s = target_state(sndpv);

    return s ? s->state : AL_STOPPED;
}

void alSndpSetPriority(void *sndpv, s16 id, u8 priority)
{
    ALSndPlayer *sndp = sndpv;

    if (id >= 0 && id < sndp->maxSounds)
        sndp->sndState[id].priority = priority;
}

void alSndpSetPitch(void *sndpv, f32 pitch)
{
    ALSndPlayer *sndp = sndpv;
    ALSoundState *s = target_state(sndp);
    ALEvent evt;

    if (!s)
        return;
    evt.type = AL_SNDP_PITCH_EVT;
    evt.msg.snd.state = s;
    evt.msg.snd.v.pitch = pitch;
    alEvtqPostEvent(&sndp->evtq, &evt, 0);
}

void alSndpSetVol(void *sndpv, s16 vol)
{
    ALSndPlayer *sndp = sndpv;
    ALSoundState *s = target_state(sndp);
    ALEvent evt;

    if (!s)
        return;
    evt.type = AL_SNDP_VOL_EVT;
    evt.msg.snd.state = s;
    evt.msg.snd.v.vol = vol;
    alEvtqPostEvent(&sndp->evtq, &evt, 0);
}

void alSndpSetPan(void *sndpv, u8 pan)
{
    ALSndPlayer *sndp = sndpv;
    ALSoundState *s = target_state(sndp);
    ALEvent evt;

    if (!s)
        return;
    evt.type = AL_SNDP_PAN_EVT;
    evt.msg.snd.state = s;
    evt.msg.snd.v.pan = pan;
    alEvtqPostEvent(&sndp->evtq, &evt, 0);
}

void alSndpSetFXMix(void *sndpv, u8 mix)
{
    ALSndPlayer *sndp = sndpv;
    ALSoundState *s = target_state(sndp);
    ALEvent evt;

    if (!s)
        return;
    evt.type = AL_SNDP_FX_EVT;
    evt.msg.snd.state = s;
    evt.msg.snd.v.fxmix = mix;
    alEvtqPostEvent(&sndp->evtq, &evt, 0);
}

VD_STATIC_ASSERT(sizeof(ALSndPlayer) == 0x54, alsndplayer_size);
VD_STATIC_ASSERT(sizeof(ALSoundState) == 0x30, alsoundstate_size);
VD_STATIC_ASSERT(sizeof(ALVoice) == 0x1C, alvoice_size);

VD_ALIAS(func_8001F6E0, alSndpNew);
VD_ALIAS(func_8001FDE0, alSndpAllocate);
VD_ALIAS(func_8001FEA0, alSndpDeallocate);
VD_ALIAS(func_8001FEF0, alSndpSetSound);
VD_ALIAS(func_8001FF00, alSndpPlay);
VD_ALIAS(func_8001FF60, alSndpGetSound);
VD_ALIAS(func_8001FF70, alSndpStop);
VD_ALIAS(func_8001FFC0, alSndpGetState);
VD_ALIAS(func_8001FFE0, alSndpSetPitch);
VD_ALIAS(func_80020030, alSndpSetPriority);
VD_ALIAS(func_80020060, alSndpSetVol);
VD_ALIAS(func_800200B0, alSndpSetPan);
VD_ALIAS(func_80020100, alSndpSetFXMix);
