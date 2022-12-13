#ifndef __RWMEMPOOL_H__
#define __RWMEMPOOL_H__

/*
 * RWMemoryPool: Manages small object pool via inheritance
 *
 * $Id: mempool.h 18623 2000-03-03 08:28:13Z sih $
 *
 * Copyright (c) 1989-1999 Rogue Wave Software, Inc.  All Rights Reserved.
 *
 * This computer software is owned by Rogue Wave Software, Inc. and is
 * protected by U.S. copyright laws and other laws and by international
 * treaties.  This computer software is furnished by Rogue Wave Software,
 * Inc. pursuant to a written license agreement and may be used, copied,
 * transmitted, and stored only in accordance with the terms of such
 * license and with the inclusion of the above copyright notice.  This
 * computer software or any other copies thereof may not be provided or
 * otherwise made available to any other person.
 *
 * U.S. Government Restricted Rights.  This computer software is provided
 * with Restricted Rights.  Use, duplication, or disclosure by the
 * Government is subject to restrictions as set forth in subparagraph (c)
 * (1) (ii) of The Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013 or subparagraphs (c) (1) and (2) of the
 * Commercial Computer Software – Restricted Rights at 48 CFR 52.227-19,
 * as applicable.  Manufacturer is Rogue Wave Software, Inc., 5500
 * Flatiron Parkway, Boulder, Colorado 80301 USA.
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:42  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.10  1996/08/22 00:22:50  hart
 * changed RW_DONT_USE_MEMORY_POOL to RW_DONT_USE_MEMPOOL
 *
 * Revision 7.9  96/07/20  03:07:13  03:07:13  hart (Marlene Hart)
 * corrected objectstore change for multithread
 * 
 * Revision 7.8  1996/07/18 04:20:22  hart
 * ObjectStore changes
 *
 * Revision 7.7  1996/05/07 22:49:43  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.6  1996/02/18 01:45:18  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.5  1995/09/05 19:10:36  jims
 * Use new copyright macro
 *
 * Revision 7.4  1995/07/26  08:36:10  jims
 * Re-align PPP directives atop file
 *
 * Revision 7.3  1995/04/19  17:14:59  jims
 * Add static mf releaseCachedMemory
 *
 * Revision 7.2  1995/04/12  20:07:55  griswolf
 * remove reference to RWMEMCK
 *
 * Revision 7.1  1994/10/16  02:52:08  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.2  1994/05/12  18:05:49  vriezen
 * Adjusted #if in .h and .cpp file to test same conditions. Bug #416
 *
 * Revision 6.1  1994/04/15  19:10:28  vriezen
 * Move all files to 6.1
 *
 * Revision 2.9  1994/03/23  00:56:26  vriezen
 * Add trailing #endif
 *
 * Revision 2.8  1994/03/22  21:40:23  vriezen
 * Added RW_DONT_USE_MEMORY_POOL option
 *
 * Revision 2.7  1993/11/08  09:11:13  jims
 * Port to ObjectStore
 *
 * Revision 2.6  1993/09/03  02:08:13  keffer
 * Macro _CLASSDLL is now named _RWTOOLSDLL
 *
 * Revision 2.5  1993/08/05  11:49:12  jims
 * Distinguish between using a WIN16 DLL from a WIN32 DLL by
 * checking for __WIN16__
 *
 * Revision 2.4  1993/03/17  18:41:25  keffer
 * TCC_DELETE_SIZE_BUG -> RW_TCC_DELETE_SIZE_BUG
 *
 * Revision 2.3  1993/02/18  17:00:47  keffer
 * Constants no longer declared static.
 *
 * Revision 2.2  1993/01/28  01:54:00  myersn
 * define macro RWMemoryPool_OPTION for MT-safe use
 *
 * Revision 2.1  1993/01/15  22:23:49  keffer
 * Tools.h++ V5.2 alpha
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.9   22 Jun 1992 17:52:28   KEFFER
 * Static data no longer visible when compiling or using the Tools.h++ DLL.
 * 
 *    Rev 1.8   28 May 1992 10:38:10   KEFFER
 * Now compatible with RWMEMCK.
 * 
 *    Rev 1.5   17 Oct 1991 09:13:00   keffer
 * Changed include path to <rw/xxx.h>
 * 
 *    Rev 1.3   31 Aug 1991 20:40:28   keffer
 * Now includes <stddef.h>, with a STARTWRAP wrapper
 * 
 *    Rev 1.1   24 Jul 1991 13:06:44   keffer
 * Added pvcs keywords
 */

/*
 * This class maintains a pool of free memory allocations for small objects.
 * Objects inheriting from it will be managed by it.  It can greatly improve
 * the performance of objects that are rapidly allocated and deleted, such
 * as nodes in a linked list or binary tree.  It can also help memory
 * fragmentation.
 *
 * Two static variables affect its performance:
 *
 *  RWMAXPOOLS:     The number of different sizes managed.  The
 *                  default (5) will manage objects with sizes
 *                  from 1 to 10 bytes big.  Objects bigger than this
 *                  will be handled by the operating system.
 *
 *  RWPOOLSIZE:     The maximum number of objects retained within a size class.
 *                  Excess objects are returned to the operating system.
 */

#ifdef RW_DONT_USE_MEMPOOL
#  define RWMemoryPool_OPTION
#else

#ifndef __RWDEFS_H__
#  include "rw/defs.h"
#endif

STARTWRAP
#include <stddef.h>     /* Looking for size_t*/
ENDWRAP

const int RWPOOLSIZE = 5;
const int RWMAXPOOLS = 5;

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWMemoryPool {
// If not a 16bitDLL and not memcheck and not debug
// If following changes, also change #if in mempool.cpp
#if !(defined(_RWTOOLSDLL) && defined(__WIN16__)) && !defined(RWDEBUG) 
  static void*  stash[RWMAXPOOLS][RWPOOLSIZE];  // The private stash
  static short  nstash[RWMAXPOOLS]; // Number of free objects in each size.
#endif
public:
#ifdef RW_TCC_DELETE_SIZE_BUG
  ~RWMemoryPool() { }   // Superfluous destructor required for Borland bug
#endif
  static void   releaseCachedMemory(); // drain memory pool

// workaround for false new/delete mismatch
#if !defined(RW_FALSE_NEW_DELETE_MISMATCH) 
  void*         operator new(size_t);
#else
  void*         operator new(size_t, size_t t = 0);
#endif
  void          operator delete(void*, size_t);

};

#ifndef RW_USE_OBJECTSTORE

// Multi-threaded programs can't afford conflicts in the static
// RWMemoryPool free-block pool.  We inherit differently because
// otherwise every derived object would grow by (up to) 4 bytes,
// for no reason.

#ifdef RW_MULTI_THREAD
#define RWMemoryPool_OPTION
#else
#define RWMemoryPool_OPTION   : public RWMemoryPool
#endif

#else /* Turn off memory pooling for ObjectStore version */
#define RWMemoryPool_OPTION
#endif

#endif /* RW_DONT_USE_MEMORY_POOL */

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWMEMPOOL_H__ */

