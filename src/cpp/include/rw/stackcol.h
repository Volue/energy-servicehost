#ifndef  __RWSTACKCOL_H__
#define  __RWSTACKCOL_H__

/*
 * Declarations for a Stack of RWCollectables,
 *   implemented as a Singly-linked list.
 *
 * $Id: stackcol.h 18623 2000-03-03 08:28:13Z sih $
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
 * Commercial Computer Software ? Restricted Rights at 48 CFR 52.227-19,
 * as applicable.  Manufacturer is Rogue Wave Software, Inc., 5500
 * Flatiron Parkway, Boulder, Colorado 80301 USA.
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:43  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.4  1996/05/07 21:44:24  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.3  1996/02/18 01:46:41  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 19:43:40  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:57:44  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:11:02  vriezen
 * Move all files to 6.1
 *
 * Revision 2.8  1994/03/04  01:04:16  jims
 * Override isEqual member function from RWCollectable to return
 * TRUE or FALSE based on operator==
 *
 * Revision 2.7  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.6  1993/04/13  23:05:00  keffer
 * Now allows functions find(), contains().
 *
 * Revision 2.5  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
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
 *    Rev 1.4   22 May 1992 17:04:18   KEFFER
 * Now uses RWDECLARE_COLLECTABLE() macro
 * 
 *    Rev 1.3   04 Mar 1992 09:04:48   KEFFER
 * nil changed to rwnil
 * 
 *    Rev 1.1   28 Oct 1991 09:08:24   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.0   28 Jul 1991 08:17:26   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 *
 */

/*
 * Restricted interface to class RWSlistCollectables to implement a
 * last in first out (LIFO) stack.
 */

#include "rw/slistcol.h"

/****************************************************************
 *                                                              *
 *              RWSlistCollectablesStack                        *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWSlistCollectablesStack : public RWSlistCollectables {

  RWDECLARE_COLLECTABLE(RWSlistCollectablesStack)

public:

  RWSlistCollectablesStack();
  RWSlistCollectablesStack(RWCollectable* a) : RWSlistCollectables(a) { }

  /************* Virtual functions inherited from RWSlistCollectables ***********/
//virtual void                  apply(RWapplyCollectable ap, void* x) 
//virtual RWCollectable*&       at(size_t i)                    {return RWSlistCollectables::at(i);}
//virtual const RWCollectable*  at(size_t) const                {return rwnil;}
//virtual void                  clear();
//virtual void                  clearAndDestroy();
//virtual RWBoolean             contains(const RWCollectable* a) const;
//RWBoolean                     containsReference(const RWCollectable* a) const'
//virtual size_t                entries() const;
//virtual RWCollectable*        find(const RWCollectable*) const
//RWCollectable*                findReference(const RWCollectable*) const
//virtual RWCollectable*        first() const   // Same as top()
//virtual RWCollectable*        get()           // Same as pop()
//virtual size_t                index(const RWCollectable*) const
  virtual RWCollectable*        insert(RWCollectable* a)        // Same as push()
    { return prepend(a); }
//virtual RWBoolean             isEmpty() const 
  virtual RWBoolean             isEqual(const RWCollectable*) const;
//virtual RWCollectable*        last() const    // Return value at bottom of stack
//virtual size_t                occurrencesOf(const RWCollectable* a) const
//size_t                        occurrencesOfReference(const RWCollectable* a) const
//virtual RWCollectable*        prepend(RWCollectable*);
  virtual void                  restoreGuts(RWvistream&);
  virtual void                  restoreGuts(RWFile&);
  virtual RWCollectable*        remove(const RWCollectable*) // Treated as pop(); argument ignored
    {return RWSlistCollectables::get();}

  /********************* Special functions **************************/
  void                          push(RWCollectable* a)  {RWSlistCollectables::prepend(a);}
  RWCollectable*                pop()                   {return RWSlistCollectables::get();}
  RWCollectable*                top() const             {return RWSlistCollectables::first();}

private:

  /******************** Disallowed functions ************************/
  virtual RWCollectable*        append(RWCollectable*)          {return rwnil;}
  RWCollectable*                removeReference(const RWCollectable*)     {return rwnil;  }
  virtual void                  removeAndDestroy(const RWCollectable*)    {;}
};                                                              

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWSTACKCOL_H__ */
