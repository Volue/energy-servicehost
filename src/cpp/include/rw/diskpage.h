#ifndef __RWDISKPAGE_H__
#define __RWDISKPAGE_H__

/*
 * RWDiskPageHeap
 *
 *       : Specializing buffered page heap that swaps pages out to disk.
 *
 * $Id: diskpage.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.4  1996/05/08 16:59:47  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.3  1996/02/18 01:39:52  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 18:29:19  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:48:33  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  18:46:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:09:48  vriezen
 * Move all files to 6.1
 *
 * Revision 2.2  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.0   11 Mar 1992 14:09:58   KEFFER
 * Initial revision.
 */

#include "rw/bufpage.h"
#include "rw/bitvec.h"
STARTWRAP
#include <stdio.h>
ENDWRAP

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWDiskPageHeap : public RWBufferedPageHeap {

public:

  RWDiskPageHeap(const char* filename=0, unsigned nbufs=10, unsigned pgsize=512);
  virtual               ~RWDiskPageHeap();

  RWBoolean             isValid() const {return tempfp_!=0;}

  // Inherited from RWPageBuffer:
  virtual RWHandle      allocate();
  virtual void          deallocate(RWHandle);

protected:

  enum HandleStatus { NotUsed, NoSwapSpace, HasSwapSpace };

  RWBoolean             allocateDiskPage(RWHandle);
  RWBoolean             handleValid(RWHandle);
  RWoffset              offsetOfHandle(RWHandle);
  void                  resize(unsigned);

  // Inherited from RWBufferedPageHeap:
  virtual RWBoolean     swapIn(RWHandle, void*);
  virtual RWBoolean     swapOut(RWHandle, void*);

private:

  static const unsigned         initialPages_;
  static const unsigned         initialHandles_;
  static const unsigned         pageIncrement_;
  static const unsigned         handleIncrement_;
  RWBitVec                      freePageMap_;   // Bit flags for free disk pages
  unsigned*                     handleMap_;     // Array that maps from handle number to disk page
  HandleStatus*                 handleStatus_;  // Status of each slot in handleMap_
  unsigned                      nHandles_;      // Length of handleMap_ and handleStatus_
  FILE*                         tempfp_;

};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWDISKPAGE_H__ */
