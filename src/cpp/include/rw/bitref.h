#ifndef __RWBITREF_H__
#define __RWBITREF_H__

/*
 * A reference to a bit in a bit vector.
 *
 * $Id: bitref.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.6  1996/07/18 04:15:21  hart
 * ObjectStore changes
 *
 * Revision 7.5  1996/05/08 18:53:06  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.4  1996/02/17 23:17:10  griswolf
 * Replace all tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.3  1995/09/05 18:06:46  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/07/11  00:11:14  pearson
 * (#1779) Added cast to RWByte to assignment to msk_ in constructor,
 * per customer's request.  This fixed the level 4 warning generated
 * by MSVC 2.x
 *
 * Revision 7.1  1994/10/16  00:34:44  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  18:45:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:09:25  vriezen
 * Move all files to 6.1
 *
 * Revision 2.11  1993/12/29  00:06:54  keffer
 * Tightened up type of constants, to avoid compiler warnings.
 *
 * Revision 2.10  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.9  1993/07/31  01:37:52  keffer
 * Added declarations for rwFirstTrue() and rwFirstFalse().
 *
 * Revision 2.8  1993/07/19  20:45:26  keffer
 * friend classes now use elaborated-type-specifier (ARM Sec. 11.4)
 *
 * Revision 2.7  1993/07/07  00:26:04  keffer
 * Copy constructor is now public to accommodate RWGBitVec(sz).
 *
 * Revision 2.6  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.4  1993/02/08  18:19:23  keffer
 * RWor, RWand, and RWxor now exported from DLL.
 *
 * Revision 2.3  1993/01/28  01:53:04  myersn
 * derive from RWMemoryPool via the macro RWMemoryPool_OPTION for MT-safety
 *
 * Revision 2.2  1993/01/27  19:18:15  keffer
 * Now use RWor, RWand, and RWxor always.
 */

/*
 * Although this class has two public constructors, it is not intended
 * to be used by itself, but rather as an lvalue to the BitVec 
 * and GBitVec(sz) classes.  Do not "construct" a RWBitRef directly.
 * The constructor must be public to accomodate GBitVec(sz) whose name
 * is not known in advance, hence it can't be made a friendly class.
 */

#include "rw/tooldefs.h"
#include "rw/mempool.h"

class RWExport RWBitVec;

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWBitRef   RWMemoryPool_OPTION {
public:

  RWBitRef(RWByte* p, size_t j)
    { pt_ = (j>>3) + p;   msk_ = (RWByte)(((RWByte)1) << (7&j)); }

  RWBitRef(const RWBitRef& br)
    { pt_=br.pt_; msk_=br.msk_; }

  operator      RWBoolean() const
    { return ((*pt_ & msk_) != 0); }

  RWBoolean     operator=(RWBoolean i)
    { if(i) *pt_ |= msk_;
    else    *pt_ &= ~msk_;
    return i; }
	
  RWBitRef&     operator=(RWBitRef& br)
    {
	  if (!(this == &br))
	    *this = (RWBoolean)br;
	  return *this;
	}

  void          operator&=(RWBoolean i) {if (!i) *pt_ &= ~msk_;}
  void          operator|=(RWBoolean i) {if  (i) *pt_ |=  msk_;}
  void          operator^=(RWBoolean i) {if  (i) *pt_ ^=  msk_;}
private:
  RWByte*       pt_;    // Pointer to the referenced byte
  RWByte        msk_;   // Mask for the bit within the byte

friend class RWExport RWBitVec;

  RW_TYPESPEC  /* This macro usually expands to nothing */
};

// Useful declarations:
extern void rwexport RWor  (RWByte*, const RWByte*, size_t);
extern void rwexport RWxor (RWByte*, const RWByte*, size_t);
extern void rwexport RWand (RWByte*, const RWByte*, size_t);
extern size_t rwexport rwFirstTrue (const RWByte*, size_t);
extern size_t rwexport rwFirstFalse(const RWByte*, size_t);

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWBITREF_H__ */
