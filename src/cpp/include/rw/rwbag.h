#ifndef __RWBAG_H__
#define __RWBAG_H__

/*
 * Declarations for RWBag --- an unordered collection.
 *
 * $Id: rwbag.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.5  1996/07/18 04:25:51  hart
 * ObjectStore changes
 *
 * Revision 7.4  1996/05/07 22:24:25  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.3  1996/02/18 01:46:13  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 19:32:06  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:54:31  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.4  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.3  1994/06/18  02:22:57  myersn
 * add out-of-line virtual dtor.
 *
 * Revision 6.2  1994/06/16  04:34:44  myersn
 * add buckets() member for use with resize().
 *
 * Revision 6.1  1994/04/15  19:10:43  vriezen
 * Move all files to 6.1
 *
 * Revision 2.7  1994/01/13  07:55:07  jims
 * Override removeAndDestroy
 *
 * Revision 2.6  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.5  1993/07/19  20:45:26  keffer
 * friend classes now use elaborated-type-specifier (ARM Sec. 11.4)
 *
 * Revision 2.4  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.3  1993/03/17  21:05:21  keffer
 * Return type of binaryStoreSize() is now RWspace
 *
 * Revision 2.2  1993/02/07  18:45:31  keffer
 * Added default value of 0 to resize().
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 * 
 *    Rev 1.4   04 Aug 1992 19:00:26   KEFFER
 * Added "virtual" keyword to destructor for documentation purposes.
 * 
 *    Rev 1.3   22 May 1992 17:04:14   KEFFER
 * Now uses RWDECLARE_COLLECTABLE() macro
 * 
 *    Rev 1.2   18 Feb 1992 09:54:36   KEFFER
 * 
 *    Rev 1.1   28 Oct 1991 09:08:20   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.0   28 Jul 1991 08:16:18   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 *
 */

#include "rw/hashdict.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWBagIterator;

/****************************************************************
 *                                                              *
 *                      RWBag                                   *
 *                                                              *
 ****************************************************************/

class RWExport RWBag : public RWCollection
{

  friend class RWExport RWBagIterator;
  RWDECLARE_COLLECTABLE(RWBag)

public:

  RWBag(size_t n = RWCollection::DEFAULT_CAPACITY);
  RWBag(const RWBag&);
  virtual ~RWBag();

  /******************** Member operators ****************************/
  void                  operator=(const RWBag&);
  RWBoolean             operator==(const RWBag&)const;

  /****************** Virtual member functions *******************/
  virtual void                  apply(RWapplyCollectable, void*);
//virtual RWspace               binaryStoreSize() const;
  virtual void                  clear();
  virtual void                  clearAndDestroy();
//virtual int                   compareTo(const RWCollectable*) const;
//virtual RWBoolean             contains(const RWCollectable*) const;
  virtual size_t                entries() const         {return totalEntries;}
  virtual RWCollectable*        find(const RWCollectable*) const;
//virtual size_t                hash() const;
  virtual RWCollectable*        insert(RWCollectable*);
  virtual RWBoolean             isEmpty() const {return totalEntries==0;}
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  virtual size_t                occurrencesOf(const RWCollectable*) const;
  virtual RWCollectable*        remove(const RWCollectable*);//"leftmost" match
  virtual void                  removeAndDestroy(const RWCollectable*); 
//virtual void                  restoreGuts(RWvistream&);
//virtual void                  restoreGuts(RWFile&);
//virtual void                  saveGuts(RWvostream&) const;
//virtual void                  saveGuts(RWFile&) const;

/********************* Special functions **********************************/

  RWCollectable*                insertWithOccurrences(RWCollectable*, size_t);
  void                          resize(size_t n = 0);
  size_t                        buckets() const { return contents.buckets(); }

private:

  size_t                totalEntries;
  RWHashDictionary      contents;       // Dictionary of number of occurrences.

  void                  deepenTally();
};

class RWExport RWBagIterator : public RWIterator
{

public:

  RWBagIterator(const RWBag&);
  virtual ~RWBagIterator();

  /**************** Virtual function inherited from RWIterator ***************/
  virtual RWCollectable*        findNext(const RWCollectable*);
  virtual RWCollectable*        key() const;            // Return current item
  virtual RWCollectable*        operator()();
  virtual void                  reset();

private:

  RWHashDictionaryIterator      contentsIterator;
  int                           count;
  RWCollectable*                currentItem;

  RW_TYPESPEC  /* This macro usually expands to nothing */

};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWBAG_H__ */
