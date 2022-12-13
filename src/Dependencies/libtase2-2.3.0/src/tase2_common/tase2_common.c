/*
 *  tase2_common.c
 *
 *  This file is part of libtase2
 *
 *  Copyright 2018 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#include "tase2_common_internal.h"

const char*
Tase2_Library_getVersionString()
{
    return TASE2_LIBRARY_VERSION_STRING;
}

int
Tase2_Library_getMajorVersion()
{
    return TASE2_LIBRARY_VERSION_MAJOR;
}

int
Tase2_Library_getMinorVersion()
{
    return TASE2_LIBRARY_VERSION_MINOR;
}

int
Tase2_Library_getPatchVersion()
{
    return TASE2_LIBRARY_VERSION_PATCH;
}
