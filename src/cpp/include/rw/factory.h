#ifndef __RWFACTORY_H__
#define __RWFACTORY_H__

/*
 * RWFactory --- can create an instance of a registered class
 *
 * $Id: factory.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 1.1.1.1  2000/03/02 21:40:41  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.9  1996/07/18 04:30:33  hart
 * ObjectStore changes
 *
 * Revision 7.8  1996/05/07 23:24:09  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.7  1996/03/07 19:54:15  kevinj
 * Bug #2684: Added RW_GLOBAL_BEFORE_CLASS_SCOPE to make inline
 * specialized template operator<= for times when <utility>'s
 * templatized operator<= is used instead of
 * the class member operator<=.
 *
 * Revision 7.6  1996/02/18 01:41:45  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.5  1996/01/29 20:28:45  pearson
 * Specialize global template function operator<= for Borland 5.0
 *
 * Revision 7.4  1996/01/25 20:23:54  pearson
 * #2437 - Override relational operator<= and explicitly forward call to
 * base class because overload resolution chooses global STL definitions
 * which are inadequate for sets, over inherited class members. 
 *
 * Revision 7.3  1995/09/05 18:31:44  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/05/31  20:54:32  griswolf
 * add RWStringID methods
 *
 * Revision 7.1  1994/10/16  02:48:57  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  18:46:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.2  1994/05/16  18:01:21  jims
 * Port to Win32 DLL
 *
 * Revision 6.1  1994/04/15  19:09:55  vriezen
 * Move all files to 6.1
 *
 * Revision 2.6  1993/09/03  02:08:13  keffer
 * Macro _CLASSDLL is now named _RWTOOLSDLL
 *
 * Revision 2.5  1993/08/05  11:49:12  jims
 * Distinguish between using a WIN16 DLL from a WIN32 DLL by
 * checking for __WIN16__
 *
 * Revision 2.4  1993/04/12  12:32:43  jims
 * The Factory now shared among multiple threads within a single process
 *
 * Revision 2.3  1993/03/13  02:21:20  keffer
 * ClassID->RWClassId
 *
 * Revision 2.2  1993/02/03  00:19:55  jims
 * Removed #include directive for procinit.h
 *
 * Revision 2.1  1993/01/15  22:23:49  keffer
 * Tools.h++ V5.2 alpha
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.5   13 May 1992 10:25:46   KEFFER
 * Changed typedef for userCreator to RWuserCreator.
 * 
 *    Rev 1.4   18 Mar 1992 11:28:32   KEFFER
 * Initializes instance manager.
 * 
 *    Rev 1.3   18 Feb 1992 09:54:20   KEFFER
 * 
 *    Rev 1.2   12 Nov 1991 13:15:00   keffer
 * Factory can now live in the DLL.
 * 
 *    Rev 1.1   28 Oct 1991 09:08:14   keffer
 * Changed inclusions to <rw/xxx.h>
 */

#include "rw/stringid.h"
#include "rw/rwset.h"

class RWExport RWCollectable;

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWFactory : public RWSet {
public:
  RWFactory();
  ~RWFactory(); 

  void                  addFunction(RWuserCreator, RWClassID);
  RWCollectable*        create(RWClassID) const; 
  RWuserCreator         getFunction(RWClassID) const;
  void                  removeFunction(RWClassID);

// new functionality: allow string IDs also
  void                  addFunction(RWuserCreator, RWClassID, RWStringID);
  RWCollectable*        create(RWStringID) const;
  RWuserCreator         getFunction(RWStringID) const;
  void                  removeFunction(RWStringID);
  RWStringID            getStringID(RWClassID) const;
  RWClassID             getClassID(RWStringID) const;

//  #2437 - Forward relational operator calls in presence of C++ Std. Lib.
#ifndef RW_NO_STL
  RWBoolean             operator<=(const RWFactory& rws)
                        { return RWSet::operator<=(rws); }
  RWBoolean             operator<=(const RWSet& rws) const
                        { return RWSet::operator<=(rws); }
  RWBoolean             operator<=(const RWHashTable& rwht) const
                        { return RWSet::operator<=(rwht); }
#endif
  RW_TYPESPEC  /* This macro usually expands to nothing */
};

#ifndef RW_TRAILING_RWEXPORT
rwexport RWFactory*     getRWFactory();
#else
RWFactory* rwexport     getRWFactory();
#endif

#if defined(_RWTOOLSDLL) && defined(__WIN16__)
  /* DLL situation; get the proper factory from the instance manager. */
# define theFactory (getRWFactory())
#else
  extern RWFactory* theFactory; // The one-of-a-kind global factory.
#endif

//Special fix for Borland 5.0, this is related to bug #2437.  Borland 5.0
//will not choose class scope over global scope ever, but will recognize
//this as a function template specialization.

#if !defined(RW_NO_STL) && defined(RW_GLOBAL_BEFORE_CLASS_SCOPE)
inline bool operator<=
(const RWFactory& wasThis, const RWFactory& btr)
{
     return wasThis.RWSet::operator<=(btr);
}
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWFACTORY_H__ */
