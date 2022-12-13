#ifndef __RWBTREE_H__
#define __RWBTREE_H__

/*
 * RWBTree -- in memory B-Tree.
 *
 * $Id: btree.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.10  1996/07/18 04:15:21  hart
 * ObjectStore changes
 *
 * Revision 7.9  1996/06/12 18:03:48  tuck
 * Fixed problem where there was a missing "defined" after #
 *
 * Revision 7.8  1996/05/08 18:35:54  kyle
 * Added export pragmas for Mac code fragments
 * Added global operator<= (template specialization) as needed.
 *
 * Revision 7.7  1996/03/15 17:35:57  kyle
 * Added comment concering RW_GLOBAL_BEFORE_CLASS_SCOPE which affects
 * Mac Symantec too.
 *
 * Revision 7.6  1996/03/07 19:54:15  kevinj
 * Bug #2684: Added RW_GLOBAL_BEFORE_CLASS_SCOPE to make inline
 * specialized template operator<= for times when <utility>'s
 * templatized operator<= is preferred member operator<=.
 *
 * Revision 7.5  1996/02/18 01:35:05  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.4  1996/01/29 20:28:12  pearson
 * Specialize global template function operator<= for Borland 5.0
 *
 * Revision 7.3  1995/09/05 18:09:39  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/09/01  00:34:34  griswolf
 * Scopus #1057: Make RWBTree re-entrant. Also, cache entries().
 *
 * Revision 7.1  1994/10/16  00:36:07  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  18:45:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:09:30  vriezen
 * Move all files to 6.1
 *
 * Revision 2.6  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.5  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.4  1993/03/20  22:02:16  keffer
 * Eliminated int to unsigned type conversions.
 *
 * Revision 2.3  1993/03/13  02:14:22  keffer
 * keyP removed in favor of RWCollectableP
 *
 * Revision 2.2  1993/01/28  01:56:00  myersn
 * derive from RWMemoryPool via the macro RWMemoryPool_OPTION for MT-safety
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.5   04 Aug 1992 18:55:48   KEFFER
 * Added "virtual" keyword to destructor for documentation purposes.
 * 
 *    Rev 1.4   22 May 1992 17:04:08   KEFFER
 * Now uses RWDECLARE_COLLECTABLE() macro
 * 
 *    Rev 1.3   04 Mar 1992 09:07:24   KEFFER
 * Changed nil to rwnil
 * 
 *    Rev 1.2   18 Feb 1992 09:54:10   KEFFER
 * 
 *    Rev 1.1   28 Oct 1991 09:08:08   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.0   28 Jul 1991 08:12:46   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 */

#include "rw/colclass.h"
#include "rw/mempool.h"

// Set the (half)order of the B-Tree:
#ifdef RDEBUG
  const unsigned rworder  = 2; /* heavy excercise if RDEBUG! */
#else
  const unsigned rworder  = 50; /* realistic "production" value */
#endif
  const unsigned rworder2 = 2*rworder;

/****************************************************************
 *                                                              *
 *                      RWBTreeNode                             *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWBTreeNode   RWMemoryPool_OPTION {
friend class RWExport RWBTree;

  RWBTreeNode();                        // Private constructors.
  RWBTreeNode(RWCollectable*);
  unsigned      counter;                // count of [rworder2] fields in use.
  RWCollectableP key[rworder2];         // Array of pointers keys.
  RWBTreeNode*  next[rworder2+1];       // Array of pointers to child nodes.
  unsigned      binarySearch(const RWCollectable*) const;
  void          initialize();
  RWBoolean     subSetOf(const RWBTree& bt) const;
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};

/****************************************************************
 *                                                              *
 *                      RWBTree                                 *
 *                                                              *
 ****************************************************************/

class RWExport RWBTree : public RWCollection {

  RWDECLARE_COLLECTABLE(RWBTree)

public:
  RWBTree();
  virtual ~RWBTree();
  RWBTree(const RWBTree&);

  void                          operator=(const RWBTree&);
  // operator <= means "subsetOf()"
#ifndef RW_GLBL_SCP_BUG2
  RWBoolean         operator<=(const RWBTree& bt) const;
#else
// We must define an operator<=() that won't be overriden by ::operator<=()
  RWBoolean         rwBTree_operator_le(const RWBTree& bt) const;
#endif
  RWBoolean                     operator==(const RWBTree& bt) const;

// Special member function to return the height of the B-tree.
  unsigned                      height() const;

/************ Standard Collection classes functions **************/
  virtual void                  apply(RWapplyCollectable, void*);
  virtual void                  clear();
//virtual void                  clearAndDestroy();
//virtual RWBoolean             contains(const RWCollectable*) const;
  virtual size_t                entries() const;
  virtual RWCollectable*        find(const RWCollectable*) const;
  virtual RWCollectable*        insert(RWCollectable*);
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  virtual RWBoolean             isEmpty() const {return root==rwnil;}
  virtual size_t                occurrencesOf(const RWCollectable*) const;
  virtual RWCollectable*        remove(const RWCollectable*);
//virtual void                  removeAndDestroy(const RWCollectable*);

private:

  RWBTreeNode*          root;           // root = first node in tree.
  // the following two pointers were used in a way that broke re-
  // entrancy. We have retained "dummyPtr" so that the binary image 
  // will be the same size as before. We have retained "sizeHack" for
  // the same reason, and are also using it to store the current number
  // of items stored, so we can avoid walking the whole tree to compute
  // entries();
  RWCollectable*        dummyPtr;   // was RWCollectable* tempKey;
  char*                 sizeHack;   // was RWBTreeNode*   tempNode;

protected:

  // Apply to all children
  void                  apl(RWBTreeNode*, RWapplyCollectable, void*);
  // Delete all children.
  void                  del(RWBTreeNode*);
  // insert recursively
  int                   ins(RWCollectable* a, RWBTreeNode*,
                            RWCollectable*&, RWBTreeNode*&);
  // remove all children of this node
  int                   rem(const RWCollectable* a, RWBTreeNode*, RWCollectable*&);
};    

/*
 *  Fix for Borland 5.0, Symantec Power Mac: Related to bug #2437.  
 *  They always prefer global scope over class scope, and see
 *  this hack as a function template specialization of op<=.
 */
 
#ifndef RW_NO_STL
# if defined(RW_GLOBAL_BEFORE_CLASS_SCOPE) || defined( RW_GLBL_SCP_BUG )
inline bool operator<=(const RWBTree& x, const RWBTree& y)
{
#  ifndef RW_GLBL_SCP_BUG2
    return x.operator<=(y);
#  else
    return x.rwBTree_operator_le(y);
#  endif  /* RW_GLBL_SCP_BUG2 */
}
# endif /* RW_GLOBAL_BEFORE_CLASS... */
#endif  /* RW_NO_STL */

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWBTREE_H__ */
