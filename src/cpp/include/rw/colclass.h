#ifndef __RWCOLCLASS_H__
#define __RWCOLCLASS_H__

/*
 * Base class for the Smalltalk(TM)-like collection classes.
 *
 * $Id: colclass.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.6  1996/05/09 21:46:14  kyle
 * Changed how protected statics are accessed by certain compilers
 * using RW_BAD_STATIC_ACCESS guard
 *
 * Revision 7.5  1996/05/08 18:27:43  kyle
 * Added export pragmas for Mac code fragments
 * Added saveObjtoStreamAddress and saveObjToRWFileAddress because
 * MrCpp doesn't have access to protected functions
 *
 * Revision 7.4  1996/02/18 01:37:17  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.3  1995/12/01 18:57:39  pearson
 * Add RWSExport modifier to RWCollection
 *
 * Revision 7.2  1995/09/05 18:13:28  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  00:36:24  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.4  1994/07/12  18:45:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.3  1994/06/18  03:28:01  myersn
 * add out-of-line virtual dtor.
 *
 * Revision 6.2  1994/06/13  20:07:00  vriezen
 * Bug #481.  Fixed oper>> when rhs operand is RWCollection*
 *
 * Revision 6.1  1994/04/15  19:09:34  vriezen
 * Move all files to 6.1
 *
 * Revision 2.7  1993/09/09  02:38:31  keffer
 * Added member function copyContentsTo().
 *
 * Revision 2.6  1993/04/14  18:38:37  keffer
 * Added declaration for static m.f. saveObjToStream() and
 * saveObjToRWFile()
 *
 * Revision 2.5  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.4  1993/03/17  21:07:06  keffer
 * Return type of binaryStoreSize() is now RWspace
 *
 * Revision 2.2  1993/02/06  03:22:47  keffer
 * Added RWExport tags to RWBag, RWOrdered, etc. declarations.
 *
 *    Rev 1.5   12 Jun 1992 14:35:16   KEFFER
 * Ported to Computer Innovations C++; inserted declarations for RWBag, etc.
 * 
 *    Rev 1.4   25 May 1992 15:46:38   KEFFER
 * CAPACITY and RESIZE are now non-const and can be changed.
 * 
 *    Rev 1.2   28 Oct 1991 09:08:08   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.1   29 Jul 1991 13:53:02   keffer
 * Changed return type of asBag(), asOrderedCollection(), asSet() and
 * asSortedCollection() to their RW names.
 * 
 *    Rev 1.0   28 Jul 1991 08:13:12   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 *
 */

#include "rw/collect.h"

class RWExport RWBag;
class RWExport RWOrdered;
class RWExport RWSet;
class RWExport RWBinaryTree;

/****************************************************************
 *                                                              *
 *                      RWCollection                            *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWSExport RWCollection : public RWCollectable {
  RWDECLARE_ABSTRACT_COLLECTABLE(RWCollection)
public:

  static size_t         DEFAULT_CAPACITY;
  static size_t         DEFAULT_RESIZE;

public:
  virtual ~RWCollection();

  /************* Virtual function inherited from RWCollectable *****************/
  virtual RWspace               binaryStoreSize() const;
  virtual RWClassID             isA() const {return __RWCOLLECTION;}
  virtual void                  restoreGuts(RWvistream&);
  virtual void                  restoreGuts(RWFile&);
  virtual void                  saveGuts(RWvostream&) const;
  virtual void                  saveGuts(RWFile&) const;

  /****************  Added virtual functions for RWCollections ****************/
  virtual void                  apply(RWapplyCollectable, void*) = 0;
  virtual void                  clear() = 0;
  virtual void                  clearAndDestroy();
  virtual RWBoolean             contains(const RWCollectable*) const;
  virtual size_t                entries() const = 0;    // Total entries
  virtual RWCollectable*        find(const RWCollectable*) const = 0;           // First occurrence
  virtual RWCollectable*        insert(RWCollectable*) = 0;
  virtual RWBoolean             isEmpty() const = 0;
  virtual size_t                occurrencesOf(const RWCollectable*) const = 0;
  virtual RWCollectable*        remove(const RWCollectable*) = 0;       // Remove first occurrence
  virtual void                  removeAndDestroy(const RWCollectable*); 

  /************************ Special functions *******************************/
  void                          operator+=(const RWCollection& c); // Add items of c to self
  void                          operator-=(const RWCollection& c); // Remove items of c from self
  void                          addContentsTo(RWCollection*) const;
  void                          copyContentsTo(RWCollection*) const;
  RWBag                         asBag() const;
  RWOrdered                     asOrderedCollection() const;
  RWSet                         asSet() const;
  RWBinaryTree                  asSortedCollection() const;
  RWBinaryTree                  asBinaryTree() const;
  RWCollection*                 select(RWtestCollectable, void*) const;

#ifndef RW_BAD_STATIC_ACCESS
  protected:
#else
// certain compilers cannot gain access to the protected static functions
// so we must provide public methods
  public:
#endif /* RW_NO_ACCESS_TO_PROTECTED */
    static void         saveObjToStream(RWCollectable*, void*);
    static void         saveObjToRWFile(RWCollectable*, void*);
};        

typedef void (*RWapplyKeyAndValue)(RWCollectable*, RWCollectable*, void*);

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWCOLCLASS_H__ */
