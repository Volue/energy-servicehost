#ifndef __RWCOLLTIME_H__
#define __RWCOLLTIME_H__
/*
 * RWCollectableTime --- collectable times.
 *
 * $Id: colltime.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.10  1996/07/18 04:15:47  hart
 * ObjectStore changes
 *
 * Revision 7.9  1996/05/09 21:51:08  kyle
 * Changed guard to RW_GLBL_SCP_BUG3
 *
 * Revision 7.8  1996/05/08 16:51:51  kyle
 * Added export pragma for Mac code fragments
 * Added >= time operator for PowerCpp AND no Standard Library
 *
 * Revision 7.7  1996/03/15 20:10:10  kyle
 * Removed RW_TIME_DATE_NEEDS_GE
 *
 * Revision 7.6  1996/03/15 17:28:25  kyle
 * Guarded operator>= with RW_TIME_DATE_NEEDS_GE so to prevent conflicts with
 * standard library.
 *
 * Revision 7.5  1996/03/15 03:08:13  kyle
 * Millenium added operator>=  to RWCollectableTime
 *
 * Revision 7.4  1996/02/18 01:37:33  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.3  1995/09/05 18:15:59  jims
 * Use new copyright macro
 *
 * Revision 7.2  1994/10/21  23:23:38  vriezen
 * Backed out fix for ambiguity.
 *
 * Revision 7.1  1994/10/16  00:55:55  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  18:45:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.1.7.3  1994/06/18  03:12:09  myersn
 * add out-of-line virtual dtor.
 *
 * Revision 6.2  1994/06/18  03:11:05  myersn
 * add out-of-line virtual dtor.
 *
 * Revision 6.1  1994/04/15  19:09:43  vriezen
 * Move all files to 6.1
 *
 * Revision 6.1.7.2  1994/04/25  21:32:53  vriezen
 * Added oper<< and oper>> for RWFile and RWv?stream classes.
 *
 * Revision 6.1.7.1  1994/04/15  22:44:44  vriezen
 * Version 7, Initial code
 *
 * Revision 6.1  1994/04/15  19:09:43  vriezen
 * Move all files to 6.1
 *
 * Revision 2.7  1993/11/08  07:53:42  jims
 * Port to ObjectStore
 *
 * Revision 2.6  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.5  1993/03/31  02:54:58  myersn
 * add constructor from RWCString and RWLocale.
 *
 * Revision 2.4  1993/03/31  01:53:08  myersn
 * fold in RWZone & RWLocale arguments supported by base classes RWDate & RWTime
 *
 * Revision 2.3  1993/03/25  05:32:01  myersn
 * eliminate hourTy, monthTy, etc. references.
 *
 * Revision 2.2  1993/03/17  21:05:21  keffer
 * Return type of binaryStoreSize() is now RWspace
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.4   22 May 1992 17:04:10   KEFFER
 * Now uses RWDECLARE_COLLECTABLE() macro
 * 
 *    Rev 1.3   18 Feb 1992 09:54:16   KEFFER
 * 
 *    Rev 1.2   28 Oct 1991 09:08:12   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.1   09 Oct 1991 18:28:56   keffer
 * Provided RWCollectableTime(const RWTime&) constructor.
 * 
 *    Rev 1.0   28 Jul 1991 08:14:00   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 *
 */

#include "rw/rwtime.h"
#include "rw/collect.h"
#include "rw/zone.h"

/****************************************************************
 *                                                              *
 *                      RWCollectableTime                       *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

#ifdef RW_OBJECTSTORE_HETERO
#define RW_Pad public _os_pad_t<RWCollectableTime,4,0>,
#else
#define RW_Pad
#endif

class RWExport RWCollectableTime : public RWCollectable, RW_Pad public RWTime {

  RWDECLARE_COLLECTABLE(RWCollectableTime)

public:

  RWCollectableTime();
  RWCollectableTime(unsigned long s)       : RWTime(s) { }
  RWCollectableTime(const RWTime& t) : RWTime(t) { }
  RWCollectableTime(unsigned h, unsigned m, unsigned s = 0,
                    const RWZone& zone = RWZone::local())
    : RWTime(h, m, s, zone) { }
  RWCollectableTime(const RWDate& d, unsigned h=0, unsigned m=0, unsigned s=0,
                    const RWZone& zone = RWZone::local())
    : RWTime(d, h, m, s, zone)   { }
  RWCollectableTime(const RWDate& d, const RWCString& str,
                    const RWZone& z = RWZone::local(),
                    const RWLocale& l = RWLocale::global())
    : RWTime(d, str, z, l) {}
  RWCollectableTime(const struct tm* tmb, const RWZone& zone = RWZone::local())
    : RWTime(tmb, zone) { }

#ifdef RW_GLBL_SCP_BUG3 
  RW_INLINE_FRIEND RWBoolean    operator>=(const RWCollectableTime& t1, const RWCollectableTime& t2)
    { return ((RWTime)t1>=(RWTime)t2); }
#endif

  virtual ~RWCollectableTime();
  /* Virtual functions inherited from RWCollectable */
  virtual RWspace               binaryStoreSize() const {return RWTime::binaryStoreSize();}
  virtual int                   compareTo(const RWCollectable*) const;
  virtual unsigned              hash() const;
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  virtual void                  restoreGuts(RWvistream&);
  virtual void                  restoreGuts(RWFile&);
  virtual void                  saveGuts(RWvostream&) const;
  virtual void                  saveGuts(RWFile&) const;
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#undef RW_Pad

#endif /* __RWCOLLTIME_H__ */
