#ifndef __RWPVECTOR_H__
#define __RWPVECTOR_H__
/*
 * RWPtrVector: Vector of pointers
 *
 * $Id: pvector.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.8  1996/07/18 04:17:52  hart
 * ObjectStore changes
 *
 * Revision 7.7  1996/05/07 22:50:27  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.6  1996/02/18 01:45:30  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.5  1996/02/05 18:43:24  griswolf
 * Fix typo from previous change.
 *
 * Revision 7.4  1996/02/05 16:36:40  griswolf
 * Scopus 1834: Never allocate using new T[i=0], to avoid deletion bug when
 * T has virtual destructor.
 *
 * Revision 7.3  1995/11/13 12:40:11  jims
 * return by const ref instead of by value
 *
 * Revision 7.2  1995/09/05 19:15:53  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:54:07  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:10:36  vriezen
 * Move all files to 6.1
 *
 * Revision 1.9  1993/11/04  13:15:09  jims
 * Add ppp version header
 *
 * Revision 1.8  1993/11/04  12:44:22  jims
 * Port to ObjectStore
 *
 * Revision 1.7  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.6  1993/05/16  21:14:00  keffer
 * Header file "rw/toolerr.h" no longer included.
 *
 * Revision 1.5  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 1.4  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 1.3  1993/03/15  02:28:02  keffer
 * Corrected error in RWVECTOR_DELETE
 *
 * Revision 1.2  1993/03/14  20:47:26  keffer
 * Removed m.f. ref(); return type of data() is now "void* const*".
 *
 * Revision 1.1  1993/02/17  18:27:09  keffer
 * Initial revision
 *
 */


#ifndef __RWTOOLDEFS_H__
#  include "rw/tooldefs.h"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWPtrVector
{

public:

  RWPtrVector()           : npts_(0), array_(0)        {;}
  RWPtrVector(size_t n) : npts_(n)
          { if (0==n) array_ = 0;
          else array_ = RW_NEW_VEC(this, RWAlloc::typespec_pointer(), n) RWvoid[n]; }
  RWPtrVector(size_t n, void* ival);
  RWPtrVector(const RWPtrVector&);
  ~RWPtrVector()        { if(array_) RWVECTOR_DELETE(npts_) array_; }

  RWPtrVector&  operator=(const RWPtrVector&);
  RWPtrVector&  operator=(void* p);           // All elements initialized to p
  void* const*  data() const {return array_;} // Cannot change vector elements
  size_t        length() const   {return npts_;}
  void          reshape(size_t);
  void          resize(size_t);
  void*&        operator[](size_t i)       { boundsCheck(i); return array_[i];}
  void*const&   operator[](size_t i) const { boundsCheck(i); return array_[i];}

  void*&        operator()(size_t i){
#ifdef RWBOUNDS_CHECK
    boundsCheck(i);
#endif
    return array_[i];
  }

  void*const&   operator()(size_t i) const {
#ifdef RWBOUNDS_CHECK
    boundsCheck(i);
#endif
    return array_[i];
  }

protected:

  void          boundsCheck(size_t i) const;

  size_t        npts_;
  RWvoid*       array_;
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWPVECTOR_H__ */
