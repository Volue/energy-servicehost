/*
 *  libmms_platform_includes.h
 *
 *  This file is part of libmms/libtase2.
 *
 *  Copyright 2013-2019 MZ Automation GmbH
 *
 *  All rights reserved.
 */

#ifndef LIBMMS_PLATFORM_INCLUDES_H_
#define LIBMMS_PLATFORM_INCLUDES_H_

#include "libmms_common_api.h"

#include "string_utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "platform_endian.h"

#define LIBMMS_VERSION "2.2.0"

#ifndef CONFIG_DEFAULT_MMS_VENDOR_NAME
#define CONFIG_DEFAULT_MMS_VENDOR_NAME "MZ Automation"
#endif

#ifndef CONFIG_DEFAULT_MMS_MODEL_NAME
#define CONFIG_DEFAULT_MMS_MODEL_NAME "libmms"
#endif

#ifndef CONFIG_DEFAULT_MMS_REVISION
#define CONFIG_DEFAULT_MMS_REVISION LIBMMS_VERSION
#endif

#if (DEBUG != 1)
#define NDEBUG 1
#endif

#include <assert.h>

#include "lib_memory.h"

#endif /* LIBMMS_PLATFORM_INCLUDES_H_ */
