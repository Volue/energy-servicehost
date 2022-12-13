#ifndef  __RWIDENDICT_H__
#define  __RWIDENDICT_H__

/*
 * Declarations for RWIdentityDictionary (Hashed Identity Dictionary)
 *
 * $Id: idendict.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.7  1996/05/07 23:24:59  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.6  1996/03/07 19:54:15  kevinj
 * Bug #2684: Added RW_GLOBAL_BEFORE_CLASS_SCOPE to make inline
 * specialized template operator<= for times when <utility>'s
 * templatized operator<= is used instead of
 * the class member operator<=.
 *
 * Revision 7.5  1996/02/18 01:45:03  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.4  1996/01/29 20:28:36  pearson
 * Specialize global template function operator<= for Borland 5.0
 *
 * Revision 7.3  1996/01/25 20:23:42  pearson
 * #2437 - Override relational operator<= and explicitly forward call
 * to base class because overload resolution chooses global STL
 * definitions which are inadequate for sets, over inherited class
 * members.
 *
 * Revision 7.2  1995/09/05 19:07:02  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:51:21  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  18:46:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.2  1994/06/18  01:59:54  myersn
 * add out-of-line virtual dtor.
 *
 * Revision 6.1  1994/04/15  19:10:18  vriezen
 * Move all files to 6.1
 *
 * Revision 2.4  1994/03/04  01:04:16  jims
 * Override isEqual member function from RWCollectable to return
 * TRUE or FALSE based on operator==
 *
 * Revision 2.3  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.2  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.5   25 May 1992 15:50:38   KEFFER
 * Optimized, reducing size.
 * 
 *    Rev 1.4   22 May 1992 17:04:12   KEFFER
 * Now uses RWDECLARE_COLLECTABLE() macro
 * 
 *    Rev 1.3   29 Apr 1992 14:51:18   KEFFER
 * Hashing now uses chaining to resolve collisions
 * 
 *    Rev 1.1   28 Oct 1991 09:08:18   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.0   28 Jul 1991 08:15:20   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 *
 */

#include "rw/hashdict.h"

/****************************************************************
 *                                                              *
 *                      RWIdentityDictionary                    *
 *                                                              *
 ****************************************************************/

/*
 * For the storage and retrieval of (key, value) pairs, using the identity
 * (i.e., the address) of the key.
 */

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWIdentityDictionary : public RWHashDictionary {

  RWDECLARE_COLLECTABLE(RWIdentityDictionary)

public:

  RWIdentityDictionary(size_t N = RWCollection::DEFAULT_CAPACITY);

// #2437 - Forward relational op. calls in presence of C++ Std. Lib
#ifndef RW_NO_STL
  RWBoolean                     operator<=(const RWIdentityDictionary& hdct)
                                { return RWHashDictionary::operator<=(hdct); }
  RWBoolean                     operator<=(const RWIdentityDictionary& hdct) const
                                { return RWHashDictionary::operator<=(hdct); }
  RWBoolean                     operator<=(const RWHashDictionary&) const;
  RWBoolean                     operator<=(const RWSet&) const;
  RWBoolean                     operator<=(const RWHashTable&) const;
#endif

  virtual ~RWIdentityDictionary();

  virtual RWBoolean             isEqual(const RWCollectable*) const;

protected:

  virtual RWCollectableAssociation*
                        findAssociation(const RWCollectable* key) const;
  virtual RWCollectableAssociation*     
                        newAssociation(RWCollectable*, RWCollectable*) const;
  virtual RWCollectableAssociation*
                        removeAssociation(const RWCollectable* key);

};

//Special fix for Borland 5.0, this is related to bug #2437.  Borland 5.0
//will not choose class scope over global scope ever, but will recognize
//this as a function template specialization.

#if !defined(RW_NO_STL) && defined(RW_GLOBAL_BEFORE_CLASS_SCOPE)
inline bool operator<=
(const RWIdentityDictionary& wasThis, const RWIdentityDictionary& btr)
{
     return wasThis.RWHashDictionary::operator<=(btr);
}
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWIDENDICT_H__ */
