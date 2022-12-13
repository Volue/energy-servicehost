#ifndef __RWBTRDICT_H__
#define __RWBTRDICT_H__

/*
 * RWBTreeDictionary --- Dictionary implemented as an in memory B-Tree
 *
 * $Id: btrdict.h 18623 2000-03-03 08:28:13Z sih $#
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
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:40  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.13  1996/07/18 04:15:21  hart
 * ObjectStore changes
 *
 * Revision 7.12  1996/06/12 18:04:30  tuck
 * Fixed problem where there was a missing "defined" after a #
 *
 * Revision 7.11  1996/05/09 00:58:17  griswolf
 * Fix preprocessor problem.
 *
 * Revision 7.10  1996/05/08 18:42:40  kyle
 * Added export pragmas for Mac code fragments
 * Added operator<= that will not be overridden by ::operator<=()
 * and guarded it as RW_GLOBAL_SCP_BUG2
 * Added details to global scope issue and combined it with comment
 * about Borland
 *
 * Revision 7.9  1996/03/15 17:37:38  kyle
 * Added comment that RW_GLOBAL_BEFORE_CLASS_SCOPE also required for
 * Symantec Mac.
 *
 * Revision 7.8  1996/03/07 19:54:15  kevinj
 * Bug #2684: Added RW_GLOBAL_BEFORE_CLASS_SCOPE to make inline
 * specialized template operator<= for times when <utility>'s
 * templatized operator<= is used instead of
 * the class member operator<=.
 *
 * Revision 7.7  1996/02/18 01:34:25  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.6  1996/02/02 23:44:27  griswolf
 * Scopus #2109: Add new select overloaded to accept key and value test.
 * Fix bug reported for btreedictionary, replicate code to hashdictionary.
 *
 * Revision 7.5  1996/01/29 20:28:04  pearson
 * Specialize global template function operator<= for Borland 5.0
 *
 * Revision 7.4  1996/01/25 20:23:32  pearson
 * #2437 - Override relational operator<= and explicitly forward call to 
 * base class because overload resolution prefers global STL definitions
 * which are inadequate for sets, to inherited class members.
 *
 * Revision 7.3  1995/09/15 16:00:41  griswolf
 * Scopus #2145: Override operator== to compare value as well as key.
 *
 * Revision 7.2  1995/09/05  18:08:21  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  00:36:01  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  18:45:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:09:29  vriezen
 * Move all files to 6.1
 *
 * Revision 2.3  1994/03/04  01:04:16  jims
 * Override isEqual member function from RWCollectable to return
 * TRUE or FALSE based on operator==
 *
 * Revision 2.2  1993/09/09  02:38:31  keffer
 * Added copy constructor and assignment operator.
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.4   04 Aug 1992 18:55:48   KEFFER
 * Added "virtual" keyword to destructor for documentation purposes.
 * 
 *    Rev 1.3   22 May 1992 17:04:08   KEFFER
 * Now uses RWDECLARE_COLLECTABLE() macro
 * 
 *    Rev 1.2   18 Feb 1992 09:54:08   KEFFER
 * 
 *    Rev 1.1   28 Oct 1991 09:08:08   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.0   28 Jul 1991 08:12:36   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 *
 */

/*
 * For the storage and retrieval of (key, value) pairs.
 */

#include "rw/btree.h"
#include "rw/colclass.h"

/****************************************************************
 *                                                              *
 *                      RWBTreeDictionary                       *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWBTreeDictionary : public RWBTree {

  RWDECLARE_COLLECTABLE(RWBTreeDictionary)

public:

  RWBTreeDictionary();
  RWBTreeDictionary(const RWBTreeDictionary&);
  void                  operator=(const RWBTreeDictionary&);

  virtual ~RWBTreeDictionary();

  RWBoolean                     operator==(const RWBTreeDictionary&) const;

//#2437 - Forward relational operator calls in presence of C++ Std Lib
#ifndef RW_NO_STL   
# ifndef    RW_GLBL_SCP_BUG2
  RWBoolean                     operator<=(const RWBTreeDictionary& btr) const
                                {  return RWBTree::operator<=(btr); }
# else
  RWBoolean                     rwBTreeDict_operator_le(const RWBTreeDictionary& btr) const
                                {  return RWBTree::rwBTree_operator_le(btr); }
# endif
#endif
 
  void                          applyToKeyAndValue(RWapplyKeyAndValue, void*);
  virtual void                  clear();
  virtual void                  clearAndDestroy();
  virtual RWCollectable*        find(const RWCollectable* key) const;  // Returns value 
  RWCollectable*                findKeyAndValue(const RWCollectable* key, RWCollectable*& value) const;
  RWCollectable*                findValue(const RWCollectable*) const;  // Returns value
  RWCollectable*                findValue(const RWCollectable*, RWCollectable*) const; // Replace value.
  RWCollectable*                insertKeyAndValue(RWCollectable* key, RWCollectable* val);
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  virtual RWCollectable*        remove(const RWCollectable* key); // Returns value
  virtual void                  removeAndDestroy(const RWCollectable*); 
  RWCollectable*                removeKeyAndValue(const RWCollectable* key, RWCollectable*& value);
  RWCollection*                 select(RWtestCollectable tfn, void* x) const;
  RWCollection*                 select(RWtestCollectablePair t, void* x) const;
};

/*
 *  Fix for Borland 5.0, Symantec Power Mac: Related to bug #2437.  
 *  They always prefer global scope over class scope, and see
 *  this hack as a function template specialization of op<=.
 */

#ifndef RW_NO_STL
# if defined(RW_GLOBAL_BEFORE_CLASS_SCOPE) || defined( RW_GLBL_SCP_BUG )
inline bool operator<=
(const RWBTreeDictionary& wasThis, const RWBTreeDictionary& btr)
{
#  ifndef RW_GLBL_SCP_BUG2
     return wasThis.RWBTree::operator<=(btr);
#  else
     return wasThis.RWBTree::rwBTree_operator_le(btr);
#  endif    /* RW_GLBL_SCP_BUG2 */
}
# endif /* RW_GLOBAL_BEFORE_CLASS... */
#endif  /* RW_NO_STL */

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWBTRDICT_H__ */
