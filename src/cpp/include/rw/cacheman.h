#ifndef __RWCACHEMAN_H__
#define __RWCACHEMAN_H__

/*
 * RWCacheManager --- caches fixed length blocks to and from disk
 *
 * $Id: cacheman.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 1.1.1.1  2000/03/02 21:40:40  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.4  1996/05/08 18:53:47  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.3  1996/02/18 01:35:58  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 18:13:08  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  00:36:16  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  18:45:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:09:33  vriezen
 * Move all files to 6.1
 *
 * Revision 2.4  1994/02/15  18:37:29  jims
 * Change return types from unsigned to size_t
 *
 * Revision 2.3  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.2  1993/03/23  02:21:53  keffer
 * Eliminated int to unsigned conversions.
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.2   18 Feb 1992 09:54:10   KEFFER
 * 
 *    Rev 1.1   28 Oct 1991 09:08:08   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.0   28 Jul 1991 08:12:58   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 *
 */

#include "rw/tooldefs.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWCacheManager
{

public:

  RWCacheManager(RWFile* file, unsigned blocksz, unsigned mxblks = 10);
  ~RWCacheManager();

  RWBoolean             flush();        // Perform any pending writes.
  void                  invalidate();   // Invalidate the entire cache
  RWBoolean             read(RWoffset locn, void* dat);
  RWBoolean             write(RWoffset locn, void* dat);

private:

  RWCacheManager(const RWCacheManager&); // Private to insure no copies
  void                  operator=(const RWCacheManager&); // Ditto
  size_t                ageAndFindSlot(RWoffset);
  RWBoolean             flush(unsigned);
  size_t                getFreeSlot();
  size_t                LRU() const; // Find least recently used block

private:

  RWFile*               theFile_;
  unsigned              maxblocks_; // Max # blocks in buff_er
  unsigned              nused_;     // Number being used.
  unsigned              blocksize_; // Size of a block
  RWoffset*             diskAddrs_; // Its disk address
  unsigned*             useCounts_; // Use count for the block
  char*                 buff_;      // The set of blocks.
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWCACHEMAN_H__ */
