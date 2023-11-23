/* SPDX-FileCopyrightText: (C) 2023 ilmmatias
 * SPDX-License-Identifier: BSD-3-Clause */

#include <rt/list.h>

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function inserts a new entry into a singly linked list.
 *
 * PARAMETERS:
 *     Head - Header entry of the list.
 *     Entry - What we're inserting.
 *
 * RETURN VALUE:
 *     None.
 *-----------------------------------------------------------------------------------------------*/
void RtPushSList(RtSList *Head, RtSList *Entry) {
    Entry->Next = Head->Next;
    Head->Next = Entry;
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function removes the last inserted entry from a singly linked list.
 *
 * PARAMETERS:
 *     Head - Header entry of the list.
 *
 * RETURN VALUE:
 *     NULL if the list was entry, what we removed otherwise.
 *-----------------------------------------------------------------------------------------------*/
RtSList *RtPopSList(RtSList *Head) {
    if (!Head->Next) {
        return NULL;
    }

    RtSList *Entry = Head->Next;
    Head->Next = Entry->Next;
    Entry->Next = NULL;

    return Entry;
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function initializes a doubly linked list.
 *
 * PARAMETERS:
 *     Head - Header entry of the list.
 *
 * RETURN VALUE:
 *     None.
 *-----------------------------------------------------------------------------------------------*/
void RtInitializeDList(RtDList *Head) {
    Head->Next = Head;
    Head->Prev = Head;
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function inserts a new entry at the front of a doubly linked list.
 *
 * PARAMETERS:
 *     Head - Header entry of the list.
 *     Entry - What we're inserting.
 *
 * RETURN VALUE:
 *     None.
 *-----------------------------------------------------------------------------------------------*/
void RtPushDList(RtDList *Head, RtDList *Entry) {
    Entry->Next = Head->Next;
    Entry->Prev = Head;
    Head->Next->Prev = Entry;
    Head->Next = Entry;
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function inserts a new entry at the tail of a doubly linked list.
 *
 * PARAMETERS:
 *     Head - Header entry of the list.
 *     Entry - What we're inserting.
 *
 * RETURN VALUE:
 *     None.
 *-----------------------------------------------------------------------------------------------*/
void RtAppendDList(RtDList *Head, RtDList *Entry) {
    Entry->Next = Head;
    Entry->Prev = Head->Prev;
    Head->Prev->Next = Entry;
    Head->Prev = Entry;
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function removes the entry at the front of a doubly linked list.
 *
 * PARAMETERS:
 *     Head - Header entry of the list.
 *
 * RETURN VALUE:
 *     NULL if the list was entry, what we removed otherwise.
 *-----------------------------------------------------------------------------------------------*/
RtDList *RtPopDList(RtDList *Head) {
    RtDList *Entry = Head->Next;
    Head->Next = Entry->Next;
    Head->Next->Prev = Head;
    return Entry;
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function removes the entry at the tail of a doubly linked list.
 *
 * PARAMETERS:
 *     Head - Header entry of the list.
 *
 * RETURN VALUE:
 *     NULL if the list was entry, what we removed otherwise.
 *-----------------------------------------------------------------------------------------------*/
RtDList *RtTruncateDList(RtDList *Head) {
    RtDList *Entry = Head->Prev;
    Head->Prev = Entry->Prev;
    Entry->Prev->Next = Head;
    return Entry;
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function removes the given entry from the doubly linked list it is currently associated
 *     with.
 *
 * PARAMETERS:
 *     Entry - What we're trying to remove.
 *
 * RETURN VALUE:
 *     None.
 *-----------------------------------------------------------------------------------------------*/
void RtUnlinkDList(RtDList *Entry) {
    Entry->Prev->Next = Entry->Next;
    Entry->Next->Prev = Entry->Prev;
}
