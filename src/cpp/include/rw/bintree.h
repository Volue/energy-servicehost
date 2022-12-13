#ifndef __RWBINTREE_H__
#define __RWBINTREE_H__

/*
 * Binary tree of pointers to RWCollectable objects
 *
 * $Id: bintree.h 18623 2000-03-03 08:28:13Z sih $
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
 *
 */

#include "rw/colclass.h"
#include "rw/iterator.h"
#include "rw/mempool.h"
#include "rw/gqueue.h"
#include "rw/gstack.h"
STARTWRAP
#include <stddef.h>
ENDWRAP

// Forward declarations:
class RWExport RWTreeNode;
class RWExport RWGQueue(RWTreeNode);
//////////////////////////////////////////////////////////////////////
// the following declaration is deprecated along with some protected
// member functions that will go away in a subsequent release.
class RWExport RWGQueue(RWCollectable);
//////////////////////////////////////////////////////////////////////
declare(RWGStack,RWTreeNode);

/****************************************************************
 *                                                              *
 *                      RWBinaryTree                            *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWBinaryTree : public RWCollection {
friend class RWExport RWBinaryTreeIterator;

  RWDECLARE_COLLECTABLE(RWBinaryTree)

public:

  RWBinaryTree();
  RWBinaryTree(const RWBinaryTree&);
  virtual ~RWBinaryTree();

  void                          operator=(const RWBinaryTree&);

  // Subset of bt
#ifndef RW_GLBL_SCP_BUG2
  RWBoolean         operator<=(const RWBinaryTree&) const;
#else
// We must define an operator<=() that won't be overriden by ::operator<=()
  RWBoolean         rwBinaryTree_operator_le(const RWBinaryTree& bt) const;
#endif

  RWBoolean                     operator==(const RWBinaryTree& bt) const;

  // Special member function to balance a binary tree:
  void                          balance();
  // nodes from root to far leaf. 1-item tree has height 1;
  size_t                        height() const;

/********  Standard Member Functions for Collection Classes ********/  
  virtual void                  apply(RWapplyCollectable, void*);
  virtual void                  clear();
//virtual void                  clearAndDestroy();
//virtual RWBoolean             contains(const RWCollectable*) const;
  virtual size_t                entries() const;                        // Total entries
  virtual RWCollectable*        find(const RWCollectable*) const;               // First occurrence
  virtual RWCollectable*        insert(RWCollectable*);
  virtual RWBoolean             isEmpty() const {return root==rwnil;}
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  virtual size_t                occurrencesOf(const RWCollectable*) const;
  virtual RWCollectable*        remove(const RWCollectable*);           // Remove first occurrence
//virtual void                  removeAndDestroy(const RWCollectable*); 
  virtual void                  saveGuts(RWvostream&) const;
  virtual void                  saveGuts(RWFile&) const;

protected:
  void                          applyChildren(const RWTreeNode*, RWapplyCollectable, void*);
  RWTreeNode*                   balanceChildren(size_t, RWGQueue(RWTreeNode)&);
  void                          deleteChildren(RWTreeNode*);
  RWCollectable*                deleteNode(RWTreeNode* victim, RWTreeNode* parent);
  void                          insertChildrenOf(const RWTreeNode*);
  void                          insertNode(RWTreeNode*, const RWTreeNode*);
  void                          levelApply(RWapplyCollectable, void*);
  size_t                        nodeHeight(RWTreeNode*) const;
  void                          queueNodes(RWTreeNode*,
                                           RWGQueue(RWTreeNode)& uniq,
                                           RWGQueue(RWTreeNode)& dupq
                                          );
//////////////////////////////////////////////////////////////////////
// The following functions are now unused by internal code, and 
// will go away in a subsequent release.
protected:
  RWTreeNode*       balanceChildren(size_t, RWGQueue(RWCollectable)&);
  void              balanceUnique();
  void              countChildren(const RWTreeNode*, size_t&) const;
//////////////////////////////////////////////////////////////////////
private:

#ifdef RDEBUG
  void  printChildren(const RWTreeNode*, ostream&, size_t&, char) const;
  friend ostream& operator<<(ostream&, const RWBinaryTree& bt);
#endif
  RWTreeNode*                   root;      // root = top-level item in tree
};    

/****************************************************************
 *                                                              *
 *              RWBinaryTreeIterator                            *
 *                                                              *
 ****************************************************************/

class RWExport RWBinaryTreeIterator : public RWIterator {
protected:
  const RWBinaryTree*           tree;           // Binary tree for this iterator.
  const RWTreeNode*             here;           // Current node.
  RWGStack(RWTreeNode)          stack;
private:
  void                          descendLeft();
public:
  RWBinaryTreeIterator(const RWBinaryTree&);
  virtual ~RWBinaryTreeIterator();

/*********** Virtual functions inherited from class RWIterator ***********/
  virtual RWCollectable*        findNext(const RWCollectable*);         // Find next matching item
  virtual RWCollectable*        key() const;                            // Return current value
  virtual RWCollectable*        operator()();                           // Advance iterator
  virtual void                  reset();
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};


/****************************************************************
 *                                                              *
 *                      RWTreeNode                              *
 *                                                              *
 ****************************************************************/

// Node in binary tree:
class RWExport RWTreeNode   RWMemoryPool_OPTION {
private:
friend class RWExport RWBinaryTree;
friend class RWExport RWBinaryTreeIterator;
  RWTreeNode*           right;  // Pointer to right node.
  RWTreeNode*           left;   // Pointer to left node.
  RWCollectable*        e;      // Pointer to RWCollectable object.
private:
  // Private constructor:
  RWTreeNode(RWCollectable* a, RWTreeNode* p=rwnil, RWTreeNode* n=rwnil) 
    { e = a; left = p; right = n; }
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};


//Special fix for Borland 5.0, this is related to bug #2437.  Borland 5.0
//will not choose class scope over global scope ever, but will recognize
//this as a function template specialization.

#if !defined(RW_NO_STL) && defined(RW_GLOBAL_BEFORE_CLASS_SCOPE)
inline bool operator<=
(const RWBinaryTree& wasThis, const RWBinaryTree& btr)
{
#  ifndef RW_GLBL_SCP_BUG2
    return wasThis.operator<=(btr);
#  else
    return wasThis.rwBinaryTree_operator_le(btr);
#  endif  /* RW_GLBL_SCP_BUG2 */
}
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWBINTREE_H__ */

