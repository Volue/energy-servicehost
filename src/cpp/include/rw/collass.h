#ifndef __RWCOLLASS_H__
#define __RWCOLLASS_H__

/*
 * Declarations for RWCollectable Associations, used in dictionaries.
 *
 * $Id: collass.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.5  1996/05/08 18:53:57  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.4  1996/02/18 01:37:20  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.3  1995/09/05 18:13:46  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/07/21  17:55:27  griswolf
 * deprecated and made setValue() private. Very likely memory leak.
 *
 * Revision 7.1  1994/10/16  00:36:36  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.4  1994/07/12  18:45:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.3  1994/06/20  16:34:41  nevis
 * Added out of line virtual destructor to IDAssoc to match
 * ctass.C...
 *
 * Revision 6.2  1994/06/18  03:11:05  myersn
 * add out-of-line virtual dtor.
 *
 * Revision 6.1  1994/04/15  19:09:36  vriezen
 * Move all files to 6.1
 *
 * Revision 2.4  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.3  1993/07/03  23:55:20  keffer
 * Removed declaration for unneeded function rwDestroyAssociation()
 *
 * Revision 2.2  1993/03/17  21:05:21  keffer
 * Return type of binaryStoreSize() is now RWspace
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.5   25 May 1992 15:47:12   KEFFER
 * 
 *    Rev 1.4   22 May 1992 17:04:08   KEFFER
 * Now uses RWDECLARE_COLLECTABLE() macro
 * 
 *    Rev 1.3   29 Apr 1992 14:51:18   KEFFER
 * Hashing now uses chaining to resolve collisions
 * 
 *    Rev 1.2   18 Feb 1992 09:54:12   KEFFER
 * 
 *    Rev 1.1   28 Oct 1991 09:08:10   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.0   28 Jul 1991 08:13:26   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 *
 */

/*
 * Binds a "key" and a "value" together.  The key and value must be RWCollectable.
 */

#include "rw/collect.h"

/****************************************************************
 *                                                              *
 *                      RWCollectableAssociation                *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWCollectableAssociation : public RWCollectable {

  RWDECLARE_COLLECTABLE(RWCollectableAssociation)

public:

  RWCollectableAssociation();
  RWCollectableAssociation(RWCollectable* k, RWCollectable* v){ky=k; val=v;}
  virtual ~RWCollectableAssociation();

  RWCollectable*                key()   const                   {return ky;}
  RWCollectable*                value() const                   {return val;}
  RWCollectable*                value(RWCollectable*);

  /* Inherited from class RWCollectable: */
  virtual RWspace               binaryStoreSize() const;
  virtual int                   compareTo(const RWCollectable*) const;
  virtual unsigned              hash() const;
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  virtual void                  restoreGuts(RWvistream&);
  virtual void                  restoreGuts(RWFile&);
  virtual void                  saveGuts(RWvostream&) const;
  virtual void                  saveGuts(RWFile&) const;

protected:

  RWCollectable*                ky;
  RWCollectable*                val;
private: /* and deprecated */
  void                          setValue(RWCollectable* v)      {val=v;}
};


/****************************************************************
 *                                                              *
 *                      RWCollectableIDAssociation              *
 *                                                              *
 ****************************************************************/

class RWExport RWCollectableIDAssociation : public RWCollectableAssociation {

  RWDECLARE_COLLECTABLE(RWCollectableIDAssociation)

public:
   RWCollectableIDAssociation();
   RWCollectableIDAssociation(RWCollectable *k, RWCollectable*v) :
      RWCollectableAssociation(k,v) { }
  virtual ~RWCollectableIDAssociation();

   /*** Override these methods in order to use addresses, not values ***/

   virtual unsigned             hash() const;
   virtual int                  compareTo(const RWCollectable*) const;
   virtual RWBoolean            isEqual(const RWCollectable* c) const;
};
      
#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWCOLLASS_H__ */
