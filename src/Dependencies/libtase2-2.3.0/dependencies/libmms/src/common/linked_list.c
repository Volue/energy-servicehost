/*
 *  linked_list.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "linked_list.h"
#include "libmms_platform_includes.h"

LinkedList
LinkedList_getLastElement(LinkedList list)
{
    while (list->next != NULL) {
        list = list->next;
    }
    return list;
}

LinkedList
LinkedList_create()
{
    LinkedList newList = (LinkedList) GLOBAL_MALLOC(sizeof(struct sLinkedList));

    if (newList) {
        newList->data = NULL;
        newList->next = NULL;
    }

    return newList;
}

/**
 * Destroy list (free). Also frees element data with helper function.
 */
void
LinkedList_destroyDeep(LinkedList list, LinkedListValueDeleteFunction valueDeleteFunction)
{
    LinkedList nextElement = list;
    LinkedList currentElement;

    do {
        currentElement = nextElement;
        nextElement = currentElement->next;

        if (currentElement->data != NULL)
            valueDeleteFunction(currentElement->data);

        GLOBAL_FREEMEM(currentElement);
    }
    while (nextElement != NULL);
}

void
LinkedList_destroy(LinkedList list)
{
    LinkedList_destroyDeep(list, Memory_free);
}

/**
 * Destroy list (free) without freeing the element data
 */
void
LinkedList_destroyStatic(LinkedList list)
{
    LinkedList nextElement = list;
    LinkedList currentElement;

    do {
        currentElement = nextElement;
        nextElement = currentElement->next;
        GLOBAL_FREEMEM(currentElement);
    }
    while (nextElement != NULL);
}

int
LinkedList_size(LinkedList list)
{
    LinkedList nextElement = list;
    int size = 0;

    while (nextElement->next != NULL) {
        nextElement = nextElement->next;
        size++;
    }

    return size;
}

void
LinkedList_add(LinkedList list, void* data)
{
    LinkedList newElement = LinkedList_create();

    if (newElement) {
        newElement->data = data;

        LinkedList listEnd = LinkedList_getLastElement(list);

        listEnd->next = newElement;
    }
}

LinkedList
LinkedList_append(LinkedList list, void* data)
{
    LinkedList newElement = LinkedList_create();

    if (newElement) {
        newElement->data = data;

        LinkedList listEnd = LinkedList_getLastElement(list);

        listEnd->next = newElement;
    }

    return newElement;
}

bool
LinkedList_addEx(LinkedList list, void* data)
{
    LinkedList newElement = LinkedList_create();

    if (newElement) {
        newElement->data = data;

        LinkedList listEnd = LinkedList_getLastElement(list);

        listEnd->next = newElement;

        return true;
    }
    else {
        return false;
    }
}

bool
LinkedList_contains(LinkedList list, void* data)
{
    LinkedList currentElement = list->next;

    while (currentElement != NULL) {
        if (currentElement->data == data)
            return true;

        currentElement = currentElement->next;
    }

    return false;
}

bool
LinkedList_remove(LinkedList list, void* data)
{
    LinkedList lastElement = list;

    LinkedList currentElement = list->next;

    while (currentElement != NULL) {
        if (currentElement->data == data) {
            lastElement->next = currentElement->next;
            GLOBAL_FREEMEM(currentElement);
            return true;
        }

        lastElement = currentElement;
        currentElement = currentElement->next;
    }

    return false;
}

LinkedList
LinkedList_insertAfter(LinkedList list, void* data)
{
    LinkedList originalNextElement = LinkedList_getNext(list);

    LinkedList newElement = LinkedList_create();

    newElement->data = data;
    newElement->next = originalNextElement;

    list->next = newElement;

    return newElement;
}

LinkedList
LinkedList_getNext(LinkedList list)
{
    return list->next;
}

LinkedList
LinkedList_get(LinkedList list, int index)
{
    LinkedList element = LinkedList_getNext(list);

    int i = 0;

    while (i < index) {
        element = LinkedList_getNext(element);

        if (element == NULL)
            return NULL;

        i++;
    }

    return element;
}

void*
LinkedList_getData(LinkedList self)
{
    return self->data;
}

void
LinkedList_printStringList(LinkedList list)
{
    LinkedList element = list;

    int elementCount = 0;

    while ((element = LinkedList_getNext(element)) != NULL) {
        char* str = (char*) (element->data);
        printf("%s\n", str);
        elementCount++;
    }
}



