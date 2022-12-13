#ifndef __RWBUFPAGE_H__
#define __RWBUFPAGE_H__

/*
 * RWBufferedPageHeap: A virtual page heap accessed through a buffer
 *
 * $Id: bufpage.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.4  1996/05/08 18:53:36  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.3  1996/02/18 01:36:17  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 18:11:45  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  00:36:11  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  18:45:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:09:31  vriezen
 * Move all files to 6.1
 *
 * Revision 2.4  1994/02/15  18:37:29  jims
 * Change return types from unsigned to size_t
 *
 * Revision 2.3  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.2  1993/03/23  02:45:12  keffer
 * Changed variable names; eliminated int to unsigned conversions.
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 * 
 * Revision 1.0   11 Mar 1992 14:09:58  keffer
 * Initial revision.
 */

#include "rw/vpage.h"
STARTWRAP
#include <stddef.h>
ENDWRAP

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWBufferedPageHeap : public RWVirtualPageHeap {
  unsigned              nBuffers_;      // Number of buffers (each is pageSize() big)
  RWvoid*               buffers_;       // Points to an array of pointers to buffers
  RWHandle*             handles_;       // Page handle associated with each buffer
  short*                lockCounts_;    // Lock count for each buffer
  unsigned*             age_;           // Age of buffer since accessed
  RWBoolean*            dirty_;         // Whether this buffer has changed since swap in
protected:
  size_t                ageAndFindHandle(RWHandle);     // Find slot for given handle and age all slots
  size_t                findHandle(RWHandle);           // Find slot for given handle
  size_t                findUnusedSlot();               // Find an unused slot
  size_t                swapPageIn(RWHandle);           // Swap in page with given handle
  size_t                swapOutLRUSlot();               // Swap out the Least Recently Used page
  virtual RWBoolean     swapIn(RWHandle, void*)  = 0;   // Supplied by specializing class
  virtual RWBoolean     swapOut(RWHandle, void*) = 0;
public:
  RWBufferedPageHeap(unsigned pgsize, unsigned nbufs=10);
  virtual               ~RWBufferedPageHeap();

  RWBoolean             isValid()               {return buffers_!=NULL;}

  // Inherited from RWVirtualPageHeap:
  virtual RWHandle      allocate()           = 0;       // Allocate a page
  virtual void          deallocate(RWHandle);           // Deallocate it
  virtual void          dirty(RWHandle);                // Declare page as dirty
  virtual void*         lock(RWHandle);                 // Lock a page
  virtual void          unlock(RWHandle);               // Unlock a page
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWBUFPAGE_H__ */
