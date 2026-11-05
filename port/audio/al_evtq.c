/* Vivid Dolls port - libultra event queue (alEvtq*), as in the ROM
 * (0x80029D40-0x8002A090). Events are kept in time order with deltas relative to
 * the previous event. The list operations run with interrupts disabled, like
 * the original's osSetIntMask(OS_IM_NONE): game threads post player events
 * (alCSPSetSeq/SetVol/Play...) while the audio thread runs the players.
 */
#include <kos.h>
#include <string.h>
#include "al_internal.h"

void alEvtqNew(ALEventQueue *evtq, ALEventListItem *items, s32 itemCount)
{
    s32 i;

    evtq->eventCount = 0;
    evtq->allocList.next = NULL;
    evtq->allocList.prev = NULL;
    evtq->freeList.next = NULL;
    evtq->freeList.prev = NULL;
    for (i = 0; i < itemCount; i++)
        alLink(&items[i], &evtq->freeList);
}

ALMicroTime alEvtqNextEvent(ALEventQueue *evtq, ALEvent *evt)
{
    ALEventListItem *item;
    ALMicroTime delta;
    irq_mask_t s = irq_disable();

    item = (ALEventListItem *)evtq->allocList.next;
    if (item != NULL) {
        alUnlink(item);
        memcpy(evt, &item->evt, sizeof(*evt));
        alLink(item, &evtq->freeList);
        delta = item->delta;
    } else {
        /* queue empty (only if the players' own heartbeat events were lost) */
        evt->type = -1;
        delta = 0;
    }
    irq_restore(s);
    return delta;
}

void alEvtqPostEvent(ALEventQueue *evtq, ALEvent *evt, ALMicroTime delta)
{
    ALEventListItem *item, *nextItem;
    ALLink *node;
    s32 postAtEnd = 0;
    irq_mask_t s = irq_disable();

    item = (ALEventListItem *)evtq->freeList.next;
    if (item == NULL) {
        irq_restore(s);
        return;
    }
    alUnlink(item);
    memcpy(&item->evt, evt, sizeof(*evt));
    if (delta == AL_EVTQ_END)
        postAtEnd = -1;
    for (node = &evtq->allocList; node != NULL; node = node->next) {
        if (node->next == NULL) {           /* end of the list */
            item->delta = postAtEnd ? 0 : delta;
            alLink(item, node);
            break;
        }
        nextItem = (ALEventListItem *)node->next;
        if (delta < nextItem->delta) {
            item->delta = delta;
            nextItem->delta -= delta;
            alLink(item, node);
            break;
        }
        delta -= nextItem->delta;
    }
    irq_restore(s);
}

/* Remove all events of a type (their deltas go to the following event). */
void alEvtqFlushType(ALEventQueue *evtq, s16 type)
{
    ALLink *thisNode, *nextNode;
    ALEventListItem *thisItem, *nextItem;
    irq_mask_t s = irq_disable();

    thisNode = evtq->allocList.next;
    while (thisNode != NULL) {
        nextNode = thisNode->next;
        thisItem = (ALEventListItem *)thisNode;
        nextItem = (ALEventListItem *)nextNode;
        if (thisItem->evt.type == type) {
            if (nextItem != NULL)
                nextItem->delta += thisItem->delta;
            alUnlink(thisItem);
            alLink(thisItem, &evtq->freeList);
        }
        thisNode = nextNode;
    }
    irq_restore(s);
}
