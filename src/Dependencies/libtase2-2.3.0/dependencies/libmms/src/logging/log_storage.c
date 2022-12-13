/*
 *  log_storage.c
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "logging_api.h"

void
LogStorage_setMaxLogEntries(LogStorage self, int maxEntries)
{
    self->maxLogEntries = maxEntries;
}

uint64_t
LogStorage_addEntry(LogStorage self, uint64_t timestamp)
{
    return self->addEntry(self, timestamp);
}

bool
LogStorage_addEntryData(LogStorage self, uint64_t entryID, const char* dataRef, uint8_t* data, int dataSize, uint8_t reasonCode)
{
    return self->addEntryData(self, entryID, dataRef, data, dataSize, reasonCode);
}

bool
LogStorage_getEntries(LogStorage self, uint64_t startingTime, uint64_t endingTime,
        LogEntryCallback entryCallback, LogEntryDataCallback entryDataCallback, void* parameter)
{
    return self->getEntries(self, startingTime, endingTime, entryCallback, entryDataCallback, parameter);
}

bool
LogStorage_getEntriesAfter(LogStorage self, uint64_t startingTime, uint64_t entryID,
        LogEntryCallback entryCallback, LogEntryDataCallback entryDataCallback, void* parameter)
{
    return self->getEntriesAfter(self, startingTime, entryID, entryCallback, entryDataCallback, parameter);
}

void
LogStorage_destroy(LogStorage self)
{
    self->destroy(self);
}

bool
LogStorage_getOldestAndNewestEntries(LogStorage self, uint64_t* newEntry, uint64_t* newEntryTime,
        uint64_t* oldEntry, uint64_t* oldEntryTime)
{
    return self->getOldestAndNewestEntries(self, newEntry, newEntryTime, oldEntry, oldEntryTime);
}

