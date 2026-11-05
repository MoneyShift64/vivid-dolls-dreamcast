/* Vivid Dolls port - libultra compact MIDI sequence reader (alCSeq*), as in the
 * ROM (0x8001D8B0-0x8001E370).
 *
 * The sequence data stays in the ROM's big-endian byte order: the header
 * (16 track offsets + division, u32 each) is read bytewise; everything else
 * is a byte stream. Compact MIDI: note-ons carry their duration (no note-offs),
 * 0xFE introduces a back-reference (2-byte backward offset, 1-byte length) to
 * an earlier byte run, meta 0x2E/0x2D are loop start/end (the loop counter is
 * kept in the sequence data itself, so the sequence buffer must be writable).
 */
#include "al_internal.h"

#define AL_CMIDI_BLOCK_CODE     0xFE
#define AL_CMIDI_LOOPSTART_CODE 0x2E
#define AL_CMIDI_LOOPEND_CODE   0x2D
#define AL_MIDI_META_TEMPO      0x51
#define AL_MIDI_META_EOT        0x2F

static u32 rd32(const u8 *p)
{
    return ((u32)p[0] << 24) | ((u32)p[1] << 16) | ((u32)p[2] << 8) | p[3];
}

static u8 __getTrackByte(ALCSeq *seq, u32 track)
{
    u8 theByte;

    if (seq->curBULen[track]) {
        theByte = *seq->curBUPtr[track];
        seq->curBUPtr[track]++;
        seq->curBULen[track]--;
    } else {
        theByte = *seq->curLoc[track];
        seq->curLoc[track]++;
        if (theByte == AL_CMIDI_BLOCK_CODE) {
            u8 loBackUp, hiBackUp, theLen, nextByte;
            u32 backup;

            nextByte = *seq->curLoc[track];
            seq->curLoc[track]++;
            if (nextByte != AL_CMIDI_BLOCK_CODE) {
                /* back-reference: the run starts (backup + 4) bytes before the
                   position after the 4-byte code */
                hiBackUp = nextByte;
                loBackUp = *seq->curLoc[track];
                seq->curLoc[track]++;
                theLen = *seq->curLoc[track];
                seq->curLoc[track]++;
                backup = ((u32)hiBackUp << 8) + loBackUp;
                seq->curBUPtr[track] = seq->curLoc[track] - (backup + 4);
                seq->curBULen[track] = theLen;
                theByte = *seq->curBUPtr[track];
                seq->curBUPtr[track]++;
                seq->curBULen[track]--;
            }
        }
    }
    return theByte;
}

static u32 __readVarLen(ALCSeq *seq, u32 track)
{
    u32 value, c;

    value = __getTrackByte(seq, track);
    if (value & 0x80) {
        value &= 0x7F;
        do {
            c = __getTrackByte(seq, track);
            value = (value << 7) + (c & 0x7F);
        } while (c & 0x80);
    }
    return value;
}

static void __alCSeqGetTrackEvent(ALCSeq *seq, u32 track, ALEvent *event)
{
    u32 offset;
    u8 status, loopCt, curLpCt, *tmpPtr;

    status = __getTrackByte(seq, track);
    if (status == 0xFF) {               /* meta event (no running status) */
        u8 type = __getTrackByte(seq, track);

        if (type == AL_MIDI_META_TEMPO) {
            event->type = AL_TEMPO_EVT;
            event->msg.tempo.status = status;
            event->msg.tempo.type = type;
            event->msg.tempo.byte1 = __getTrackByte(seq, track);
            event->msg.tempo.byte2 = __getTrackByte(seq, track);
            event->msg.tempo.byte3 = __getTrackByte(seq, track);
            seq->lastStatus[track] = 0;
        } else if (type == AL_MIDI_META_EOT) {
            seq->validTracks ^= 1u << track;
            event->type = seq->validTracks ? AL_TRACK_END : AL_SEQ_END_EVT;
        } else if (type == AL_CMIDI_LOOPSTART_CODE) {
            (void)__getTrackByte(seq, track);
            (void)__getTrackByte(seq, track);
            seq->lastStatus[track] = 0;
            event->type = AL_CSP_LOOPSTART;
        } else if (type == AL_CMIDI_LOOPEND_CODE) {
            tmpPtr = seq->curLoc[track];
            loopCt = *tmpPtr++;
            curLpCt = *tmpPtr;
            if (curLpCt == 0) {         /* done looping */
                *tmpPtr = loopCt;       /* reset the loop count */
                seq->curLoc[track] = tmpPtr + 5;
            } else {
                if (curLpCt != 0xFF)    /* 0xFF = loop forever */
                    *tmpPtr = curLpCt - 1;
                tmpPtr++;
                offset = rd32(tmpPtr);
                tmpPtr += 4;
                seq->curLoc[track] = tmpPtr - offset;
            }
            seq->lastStatus[track] = 0;
            event->type = AL_CSP_LOOPEND;
        }
        /* other meta types: the event type is left as it was (as in the ROM) */
    } else {
        event->type = AL_SEQ_MIDI_EVT;
        if (status & 0x80) {
            event->msg.midi.status = status;
            event->msg.midi.byte1 = __getTrackByte(seq, track);
            seq->lastStatus[track] = status;
        } else {                        /* running status */
            event->msg.midi.status = seq->lastStatus[track];
            event->msg.midi.byte1 = status;
        }
        if ((event->msg.midi.status & 0xF0) != 0xC0 && (event->msg.midi.status & 0xF0) != 0xD0) {
            event->msg.midi.byte2 = __getTrackByte(seq, track);
            if ((event->msg.midi.status & 0xF0) == 0x90)
                event->msg.midi.duration = __readVarLen(seq, track);
        } else {
            event->msg.midi.byte2 = 0;
        }
    }
}

void alCSeqNew(void *seqv, u8 *ptr)
{
    ALCSeq *seq = seqv;
    u32 i, tmpOff;

    seq->base = ptr;
    seq->validTracks = 0;
    seq->lastDeltaTicks = 0;
    seq->lastTicks = 0;
    seq->deltaFlag = 1;
    for (i = 0; i < 16; i++) {
        seq->lastStatus[i] = 0;
        seq->curBUPtr[i] = 0;
        seq->curBULen[i] = 0;
        tmpOff = rd32(ptr + 4 * i);
        if (tmpOff) {
            seq->validTracks |= 1u << i;
            seq->curLoc[i] = ptr + tmpOff;
            seq->evtDeltaTicks[i] = __readVarLen(seq, i);
        } else {
            seq->curLoc[i] = 0;
        }
    }
    /* the ROM computes 1.0 / (f32)division in double precision */
    seq->qnpt = (f32)(1.0 / (f64)(f32)rd32(ptr + 0x40));
}

void alCSeqNextEvent(ALCSeq *seq, ALEvent *evt)
{
    u32 i, firstTime = 0xFFFFFFFFu, firstTrack = 0;
    u32 lastTicks = seq->lastDeltaTicks;

    for (i = 0; i < 16; i++) {
        if ((seq->validTracks >> i) & 1) {
            if (seq->deltaFlag)
                seq->evtDeltaTicks[i] -= lastTicks;
            if (seq->evtDeltaTicks[i] < firstTime) {
                firstTime = seq->evtDeltaTicks[i];
                firstTrack = i;
            }
        }
    }
    __alCSeqGetTrackEvent(seq, firstTrack, evt);
    evt->msg.midi.ticks = firstTime;
    seq->lastTicks += firstTime;
    seq->lastDeltaTicks = firstTime;
    if (evt->type != AL_TRACK_END)
        seq->evtDeltaTicks[firstTrack] += __readVarLen(seq, firstTrack);
    seq->deltaFlag = 1;
}

/* Ticks to the next event; FALSE when no track is left. */
s32 __alCSeqNextDelta(ALCSeq *seq, s32 *pDeltaTicks)
{
    u32 i, firstTime = 0xFFFFFFFFu;
    u32 lastTicks = seq->lastDeltaTicks;

    if (!seq->validTracks)
        return 0;
    for (i = 0; i < 16; i++) {
        if ((seq->validTracks >> i) & 1) {
            if (seq->deltaFlag)
                seq->evtDeltaTicks[i] -= lastTicks;
            if (seq->evtDeltaTicks[i] < firstTime)
                firstTime = seq->evtDeltaTicks[i];
        }
    }
    seq->deltaFlag = 0;
    *pDeltaTicks = (s32)firstTime;
    return 1;
}

VD_STATIC_ASSERT(sizeof(ALCSeq) == 0xF8, alcseq_size);

VD_ALIAS(func_8001D8B0, alCSeqNew);
