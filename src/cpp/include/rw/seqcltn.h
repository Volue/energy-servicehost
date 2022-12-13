#ifndef __RWSEQCLTN_H__
#define __RWSEQCLTN_H__

/*
 * Abstract base class for sequenceable collections.
 *
 * $Id: seqcltn.h 18623 2000-03-03 08:28:13Z sih $
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
 * This is an abstract base class, inherited by collections whose
 * elements are ordered and accessible by an index.
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:43  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.4  1996/05/07 21:43:25  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.3  1996/02/18 01:46:34  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 19:42:44  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:57:25  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.7  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.6  1994/06/22  23:40:38  vriezen
 * Remove RW_DEFINE_HOME and RW_DECLARE_HOME, no longer needed.
 *
 * Revision 6.5  1994/06/18  00:49:24  myersn
 * move RWSequenceable dtor out-of-line.
 *
 * Revision 6.4  1994/06/16  03:34:55  vriezen
 * Added RW_DECLARE_HOME
 *
 * Revision 6.3  1994/06/13  20:07:00  vriezen
 * Bug #481.  Fixed oper>> when rhs operand is RWCollection*
 *
 * Revision 6.2  1994/04/15  21:12:29  vriezen
 * Backed out (until next release) RW_DEFINE_HOME and RW_DECLARE_HOME support.
 *
 * Revision 6.1  1994/04/15  19:10:56  vriezen
 * Move all files to 6.1
 *
 * Revision 2.7  1994/03/23  21:16:01  vriezen
 * Added RW_DECLARE_HOME macro to RWSequenceable
 *
 * Revision 2.6  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.5  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.4  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 2.3  1993/01/25  18:12:13  keffer
 * RW_NO_CONST_OVERLOADS->RW_NO_CONST_OVERLOAD
 *
 * Revision 2.1  1992/11/19  05:45:01  keffer
 * Introduced new <rw/compiler.h> macro directives
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.2   18 Feb 1992 09:54:44   KEFFER
 * 
 *    Rev 1.1   28 Oct 1991 09:08:22   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.0   28 Jul 1991 08:16:54   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 *
 */

#include "rw/colclass.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWSequenceable : public RWCollection {
  RWDECLARE_ABSTRACT_COLLECTABLE(RWSequenceable)
public:
  virtual                       ~RWSequenceable();

  /************ Virtual functions inherited from RWCollection *************/
  virtual void                  apply(RWapplyCollectable, void*) = 0;
  virtual void                  clear() = 0;
//virtual void                  clearAndDestroy();
//virtual RWBoolean             contains(const RWCollectable*) const;
  virtual size_t                entries() const = 0;    // Total entries
  virtual RWCollectable*        find(const RWCollectable*) const = 0;           // First occurrence
  virtual RWCollectable*        insert(RWCollectable*) = 0;
  virtual RWBoolean             isEmpty() const = 0;
  virtual size_t                occurrencesOf(const RWCollectable*) const = 0;
  virtual RWCollectable*        remove(const RWCollectable*) = 0;       // Remove first occurrence
//virtual void                  removeAndDestroy(const RWCollectable*); 

public:

  /****************  Added virtual functions for RWSequenceables ****************/
  virtual RWCollectable*        append(RWCollectable*) = 0;
  virtual RWCollectable*&       at(size_t) = 0; // Some collections can use as lvalue
#ifndef RW_NO_CONST_OVERLOAD
  virtual const RWCollectable*  at(size_t) const = 0; // Cannot use as lvalue
#endif
  virtual RWCollectable*        first() const = 0;
  virtual size_t                index(const RWCollectable*) const = 0;
  virtual RWCollectable*        insertAt(size_t, RWCollectable*) = 0;
  virtual RWCollectable*        last() const = 0;
  virtual RWCollectable*        prepend(RWCollectable*) = 0;

  // For backwards compatibility:
  virtual RWCollectable*        insertAfter(int, RWCollectable*) = 0;
};

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWSEQCLTN_H__ */
