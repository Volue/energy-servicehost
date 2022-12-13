#ifndef __RWCOLLECT_H__
#define __RWCOLLECT_H__

/*
 * Declarations for RWCollectables: an abstract base class.
 *
 * $Id: collect.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.30  1996/09/24 17:21:45  kyle
 * Scopus 4816: make nilStoreSize() _explicitly_ inline.
 *
 * Revision 7.29  1996/08/27 17:55:33  griswolf
 * Scopus #3233: Allow nonpublic RWCollectable constructors.
 *
 * Revision 7.28  1996/07/18 04:15:47  hart
 * ObjectStore changes
 *
 * Revision 7.27  1996/05/15 20:12:29  griswolf
 * Scopus 3271: Shut up a silly warning.
 *
 * Revision 7.26  1996/05/08 18:54:04  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.25  1996/04/26 22:37:53  groening
 * removed the RW_BROKEN_TOKEN_PASTE wrapper.
 *
 * Revision 7.24  1996/03/15 05:06:49  kevinj
 * Bug #2805: Added RWEXPORT_RETURN(x) to new extraction and insertion
 * operators to make them visible to DLL's import libraries.
 *
 * Revision 7.23  1996/02/27 19:11:59  pearson
 * Fix typo
 *
 * Revision 7.22  1996/02/21 19:24:20  griswolf
 * Add mutex to nextAtom() method for multi-thread library.
 *
 * Revision 7.21  1996/02/18 01:37:25  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.20  1996/02/01 00:51:48  griswolf
 * Scopus 2509: Add static accessor classIsA() returning the class ID.
 *
 * Revision 7.19  1996/01/29 19:50:45  kyle
 * Add RW_BAD_STATIC_DESTRUCTORS for Millenium, perhaps others.
 *
 * Revision 7.18  1995/12/14 18:01:21  jims
 * Create RWDEFINE_ABSTRACT_COLLECTABLE to avoid inlined try statements
 *
 * Revision 7.17  1995/12/01 18:56:54  pearson
 * Add RWSExport modifier to RWCollectable
 *
 * Revision 7.16  1995/10/26 23:52:52  griswolf
 * Fix typo.
 *
 * Revision 7.15  1995/10/26  23:46:34  griswolf
 * Scopus #2193: Better technique for partial object retoration into
 * pointer.
 *
 * Revision 7.14  1995/10/24  20:06:12  griswolf
 * Scopus #2193: Address memory leak if an exception is thrown during
 * recursiveRestoreFrom, by adding static tryRecursiveRestore methods.
 *
 * Revision 7.13  1995/10/23  17:43:03  pearson
 * Add check for RW_TRAILING_RWEXPORT to support Watcom C++ 10.5 DLL's
 *
 * Revision 7.12  1995/09/05  18:14:41  jims
 * Use new copyright macro
 *
 * Revision 7.11  1995/06/29  00:25:23  griswolf
 * Scopus #788: provide a static nilStoreSize() method.
 *
 * Revision 7.10  1995/06/26  19:18:12  griswolf
 * Scopus #1874: Pass RWStringID by const reference so that Borland will
 * expand classID() inline. (And trade dereference for refcount costs).
 *
 * Revision 7.9  1995/06/23  23:06:40  jims
 * Take isAtom out of line
 *
 * Revision 7.8  1995/06/22  20:44:24  griswolf
 * fix friend RWInit(className) typo, remove tool7magic.
 *
 * Revision 7.7  1995/05/25  23:10:15  griswolf
 * Change some global constants to RWCollectable static consts for scoping.
 * Change scoped enum values to static consts to control sizeof(thatValue).
 *
 * Revision 7.6  1995/05/18  20:44:04  griswolf
 * scopus 921: string type classID for RWCollectables
 *
 * Revision 7.5  1995/05/16  20:47:47  jims
 * Add function to DECLARE_COLLECTABLE to avoid hiding op<< for RWFile
 *
 * Revision 7.4  1995/04/18  20:32:07  griswolf
 * moved some global << and >> methods into RWDECLARE_COLLECTABLE to help
 * provide disambiguation between various (v)streams.
 *
 * Revision 7.3  1994/10/21  23:17:34  vriezen
 * Re-enable commented out statement.
 *
 * Revision 7.2  1994/10/21  23:16:15  vriezen
 * irrelevent formatting.
 *
 * Revision 7.1  1994/10/16  00:51:18  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.6  1994/07/12  18:45:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.1.7.7  1994/10/06  20:48:53  kevinj
 * Temporarily commented out #define RWnilCollectable so that
 * library would compile.
 *
 * Revision 6.5  1994/07/11  19:14:18  vriezen
 * Using RW_INLINE_FRIEND. Bug# 670
 *
 * Revision 6.4  1994/06/13  20:11:51  vriezen
 * Bug #481.  Fixed oper>> when rhs operand is RWCollection*
 *
 * Revision 6.3  1994/06/13  20:07:28  jims
 * Now always defines RWnilCollectable as a function call that returns
 * the nil collectable object.  Set on first reference to avoid static init
 *
 * Revision 6.1.7.6  1994/07/11  19:17:24  vriezen
 * Using RW_INLINE_FRIEND. Bug# 670
 *
 * Revision 6.1.7.5  1994/06/13  20:14:56  vriezen
 * Bug #481.  Fixed oper>> when rhs operand is RWCollection*
 *
 * Revision 6.1.7.4  1994/06/13  20:09:25  jims
 * Same as 6.3
 *
 * Revision 6.3  1994/06/13  20:07:28  jims
 * Now always defines RWnilCollectable as a function call that returns
 * the nil collectable object.  Set on first reference to avoid static init
 *
 * Revision 6.2  1994/05/16  18:23:45  jims
 * Port to Win32 DLL
 *
 * Revision 6.1  1994/04/15  19:09:39  vriezen
 * Move all files to 6.1
 *
 * Revision 2.14  1993/09/03  02:08:13  keffer
 * Macro _CLASSDLL is now named _RWTOOLSDLL
 *
 * Revision 2.13  1993/08/05  11:49:12  jims
 * Distinguish between using a WIN16 DLL from a WIN32 DLL by
 * checking for __WIN16__
 *
 * Revision 2.12  1993/08/03  18:40:35  dealys
 * Ported to MPW C++ 3.3 -- RW_BROKEN_TOKEN_PASTE
 *
 * Revision 2.11  1993/04/22  20:38:48  keffer
 * Persistence operators now store and restore nil pointers.
 *
 * Revision 2.10  1993/04/12  12:28:28  jims
 * Use global nil object for MULTI_THREAD, but not for Windows 3.x DLL
 *
 * Revision 2.9  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.8  1993/04/09  02:50:00  keffer
 * Added support for operator<< and >> for virtual streams and RWFile.
 *
 * Revision 2.7  1993/04/08  16:44:42  keffer
 * Cleanup and removed V4.X support.
 *
 * Revision 2.6  1993/03/17  21:07:06  keffer
 * Return type of binaryStoreSize() is now RWspace
 *
 * Revision 2.3  1993/03/13  00:17:34  keffer
 * UNDEFINED_REFERENCE_BUG becomes RW_UNDEFINED_REFERENCE_BUG
 *
 * Revision 2.2  1993/02/03  00:01:56  jims
 * Removed #include directive for procinit.h
 *
 *    Rev 1.12   09 Sep 1992 12:47:10   KEFFER
 * Changed macro "Init" to "RWInit" to avoid conflict with JPI header.
 * 
 *    Rev 1.6   21 Feb 1992 12:29:26   KEFFER
 * nil collectable now defined when using the DLL.
 * 
 *    Rev 1.4   13 Nov 1991 11:07:42   keffer
 * More robust DLL code; No longer declares GVector(RWCollectable); store and
 * read tables now maintained by a separate manager.
 * 
 */

#ifndef __RWTOOLDEFS_H__
#  include "rw/tooldefs.h"
#endif
#ifndef __RWMEMPOOL_H__
#  include "rw/mempool.h"
#endif
#ifndef __RWGENERIC_H__
#  include "rw/generic.h"               /* Looking for name2() */
#endif
#ifdef RW_UNDEFINED_REFERENCE_BUG
#  include "rw/rstream.h"
#endif
#ifndef __RW_STRINGID_H__
#include "rw/stringid.h"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

/* some of the following global functions are needed below  */
/* old version */
extern RWEXPORT_RETURN(void)            rwAddToFactory(RWuserCreator, RWClassID);
extern RWEXPORT_RETURN(RWCollectableP)  rwCreateFromFactory(RWClassID);
extern RWEXPORT_RETURN(void)            rwRemoveFromFactory(RWClassID);
/* new version */
extern RWEXPORT_RETURN(void)            rwAddToFactory(RWuserCreator, RWClassID, RWStringID);
extern RWEXPORT_RETURN(RWCollectableP)  rwCreateFromFactory(RWStringID);
extern RWEXPORT_RETURN(void)            rwRemoveFromFactory(RWStringID);
extern RWEXPORT_RETURN(RWClassID)       rwGetClassID(RWStringID);
extern RWEXPORT_RETURN(RWStringID)      rwGetStringID(RWClassID);
/* miscellany */
extern RWEXPORT_RETURN(void)            rwDestroy(RWCollectable*, void*);
extern RWEXPORT_RETURN(RWBoolean)       rwIsEqualFun(const void* a, const void* b);

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

/************************************************************************
 *                                                                      *
 * The macro RWDECLARE_ABSTRACT_COLLECTABLE should be included in the   *
 * declaration of any abstract class that derives from RWCollectable.   *
 *                                                                      *
 ************************************************************************/

#define RWDECLARE_ABSTRACT_COLLECTABLE(className)                             \
  public:                                                                     \
    friend RWEXPORT_RETURN(RWvistream&) operator>>(RWvistream&,className*&);  \
    friend RWEXPORT_RETURN(RWFile&) operator>>(RWFile&,className*&);          \
    RW_INLINE_FRIEND RWvostream& operator<<(RWvostream& v,const className& Cl)\
      { Cl.recursiveSaveOn(v); return v; }                                    \
    RW_INLINE_FRIEND RWFile&     operator<<(RWFile& f,const className& Cl)    \
      { Cl.recursiveSaveOn(f); return f; }                                    \
    RW_INLINE_FRIEND RWvistream& operator>>(RWvistream& v, className& Cl)     \
      { RWCollectable::recursiveRestoreFrom(v,&Cl); return v; }               \
    RW_INLINE_FRIEND RWFile& operator>>(RWFile& f, className& Cl)             \
      { RWCollectable::recursiveRestoreFrom(f,&Cl); return f; }               \
    RW_TYPESPEC  /* This macro usually expands to nothing */


#define RWDEFINE_ABSTRACT_COLLECTABLE(className)             \
 RWEXPORT_RETURN(RWvistream&)                                \
 operator>>(RWvistream& s, className*& pCl)                  \
 {                                                           \
   RWCollectable* p=0;                                       \
   RW_TRYSTART RWCollectable::tryRecursiveRestore(s,p);      \
   RW_TRYEND(pCl = (className*)p) pCl = (className*)p;       \
   return s;                                                 \
 }                                                           \
 RWEXPORT_RETURN(RWFile&)                                    \
 operator>>(RWFile& f, className*& pCl)                      \
 {                                                           \
   RWCollectable* p=0;                                       \
   RW_TRYSTART RWCollectable::tryRecursiveRestore(f,p);      \
   RW_TRYEND(pCl = (className*)p) pCl = (className*)p;       \
   return f;                                                 \
 }

/********

****NOTE****: friend code above is duplicated in next macro.  This
allows future additions to RWDECLARE_ABSTRACT_COLLECTABLE to be
independant of RWDECLARE_COLLECTABLE.  (The alternative of having
RWDECLARE_COLLECTABLE invoke RWDECLARE_ABSTRACT_COLLECTABLE did not
seem right.  Having both macros invoke a third (i.e DECLARE_COMMON)
seems like too much.)

*******/
#  define RWInit(className)     name2(RWInitCtor,className)
#  define rwCreateFN(className) name2(rwCreate,className)

/************************************************************************
 *                                                                      *
 * The macro RWDECLARE_COLLECTABLE should be included in the declaration*
 * of any class that derives from RWCollectable.                        *
 *                                                                      *
 ************************************************************************/

#define RWDECLARE_COLLECTABLE(className)                                \
  private:                                                              \
    friend struct RWInit(className);                                    \
    friend RWCollectable* rwCreateFN(className) ();                     \
    static RWClassID            myAtom;                                 \
  public:                                                               \
    virtual RWCollectable*      newSpecies() const;                     \
    virtual RWCollectable*      copy() const;                           \
    virtual RWClassID           isA() const;                            \
    static  RWClassID           classIsA();                             \
    friend RWEXPORT_RETURN(RWvistream&)                                 \
    operator>>(RWvistream& s, className*& pCl);                         \
    friend RWEXPORT_RETURN(RWFile&)                                     \
    operator>>(RWFile& f,     className*& pCl);                         \
    RW_INLINE_FRIEND                                                    \
    RWvostream& operator<<(RWvostream& v,const className& Cl)           \
      { Cl.recursiveSaveOn(v); return v; }                              \
    RW_INLINE_FRIEND                                                    \
    RWFile&     operator<<(RWFile& f,const className& Cl)               \
      { Cl.recursiveSaveOn(f); return f; }                              \
    RW_INLINE_FRIEND                                                    \
    RWvistream& operator>>(RWvistream& v, className& Cl)                \
      { RWCollectable::recursiveRestoreFrom(v,&Cl); return v; }         \
    RW_INLINE_FRIEND                                                    \
    RWFile& operator>>(RWFile& f, className& Cl)                        \
      { RWCollectable::recursiveRestoreFrom(f,&Cl); return f; }         \
    RW_TYPESPEC  /* This macro usually expands to nothing */

/* See NOTE above !!! */

/************************************************************************
 *                                                                      *
 * For each class derived from RWCollectable, EXACTLY ONE macro:        *
 *   RWDEFINE_COLLECTABLE(className, RWClassID)         or              *
 *   RWDEFINE_NAMED_COLLECTABLE(className, RWStringID,                  *
 *                                                                      *
 * should be included in ONE ".cpp" file to implement several common    *
 * functions for classing deriving from RWCollectable.  These macros    *
 * serve several purposes:                                              *
 * 1) To provide a definition for newSpecies().                         *
 * 2) To provide a definition for copy().                               *
 * 3) To provide a definition for isA().                                *
 * 4) To define a "creator function" to be inserted into the            *
 *    one-of-a-kind global RWFactory pointed to by theFactory.          *
 * 5) For classes with RWStringIDs, to provide a map between the        *
 *    RWClassID and the RWStringID, (which also lives in the unique     *
 *    global RWFactory.                                                 *
 *                                                                      *
 * RWDEFINE_COLLECTABLE is the version 6 macro. If you use this macro,  *
 *    you will be defining an RWClassID that is "forever" associated    *
 *    with your class. The stringID() method will return the token      *
 *    which is the class's name. Method isA() will work as before.      *
 *                                                                      *
 * RWDEFINE_NAMED_COLLECTABLE is provided so you can give your class    *
 *    your choice of RWStringID. If you use this macro, your class may  *
 *    have a different RWClassID (returned from isA()) with each run    *
 *    of the executable, but it will be "forever" associated with the   *
 *    RWStringID. The isA() method will return some RWClassID (the      *
 *    same throughout any given run of the executable) while the method *
 *    stringID() will return the same RWStringID every time.            *
 *                                                                      *
 ************************************************************************/

#ifdef RW_BAD_STATIC_DESTRUCTORS
# define        rwInitDestructor(id, strClassName) \
                        rwRemoveFromFactory(id);
#else
# define        rwInitDestructor(id, strClassName) \
                        rwRemoveFromFactory(id); \
                        rwRemoveFromFactory(RWStringID(strClassName));
#endif // RW_BAD_STATIC_DESTRUCTORS

#define RWDEFINE_COLLECTABLE(className, id)                     \
  RWClassID      className::myAtom = id;                        \
  RWCollectable* className::newSpecies() const                  \
    { return new className; }                                   \
  RWCollectable* className::copy() const                        \
    { return new className(*this); }                            \
  RWClassID      className::isA() const                         \
    { return myAtom; }                                          \
  /*static*/                                                    \
  RWClassID className::classIsA() { return myAtom;}             \
                                                                \
  /* Global function to create an instance of the class:*/      \
  RWCollectable* rwCreateFN(className)()                        \
    { return new className; }                                   \
                                                                \
  /* Static constructor to insert above into factory: */        \
  struct RWInit(className) {                                    \
    RWInit(className)();                                        \
    ~RWInit(className)();                                       \
  };                                                            \
  RWInit(className)::RWInit(className)()                        \
    { rwAddToFactory(rwCreateFN(className), id,                 \
                     RWSTRINGIFY(className)); }                 \
  RWInit(className)::~RWInit(className)()  {                    \
        rwInitDestructor(id, RWSTRINGIFY(className));           \
  }                                                             \
  RWInit(className) name2(rwDummy,className);                   \
                                                                \
  RWEXPORT_RETURN(RWvistream&)                                  \
  operator>>(RWvistream& s, className*& pCl)                    \
  {                                                             \
    RWCollectable* p=0;                                         \
    RW_TRYSTART RWCollectable::tryRecursiveRestore(s,p);        \
    RW_TRYEND(pCl = (className*)p) pCl = (className*)p;         \
    return s;                                                   \
  }                                                             \
  RWEXPORT_RETURN(RWFile&)                                      \
  operator>>(RWFile& f,     className*& pCl)                    \
  {                                                             \
    RWCollectable* p=0;                                         \
    RW_TRYSTART RWCollectable::tryRecursiveRestore(f,p);        \
    RW_TRYEND(pCl = (className*)p) pCl = (className*)p;         \
    return f;                                                   \
  }                                                             \

#ifdef RW_BAD_STATIC_DESTRUCTORS
# define        rwInitNamedDestructor(id, str)                  \
    rwRemoveFromFactory(id);
#else
# define        rwInitNamedDestructor(id, str)                  \
    rwRemoveFromFactory(id);                                    \
    rwRemoveFromFactory(str);
#endif

#define RWDEFINE_NAMED_COLLECTABLE(className, str)              \
  RWClassID      className::myAtom = RWCollectable::nextAtom(); \
  RWCollectable* className::newSpecies() const                  \
    { return new className; }                                   \
  RWCollectable* className::copy() const                        \
    { return new className(*this); }                            \
  RWClassID      className::isA() const                         \
    { return myAtom; }                                          \
  /*static*/                                                    \
  RWClassID className::classIsA() { return myAtom;}             \
                                                                \
  /* Global function to create an instance of the class:*/      \
  RWCollectable* rwCreateFN(className)()                        \
    { return new className; }                                   \
                                                                \
  /* Static constructor to insert above into factory: */        \
  struct RWInit(className) {                                    \
    RWInit(className)();                                        \
    ~RWInit(className)();                                       \
  };                                                            \
  RWInit(className)::RWInit(className)()                        \
   { rwAddToFactory(rwCreateFN(className),className::myAtom,str); } \
  RWInit(className)::~RWInit(className)() {                     \
    rwInitNamedDestructor(className::myAtom,str);               \
  }                                                             \
  RWInit(className) name2(rwDummy,className);                   \
                                                                \
  RWEXPORT_RETURN(RWvistream&)                                  \
  operator>>(RWvistream& s, className*& pCl)                    \
  {                                                             \
    RWCollectable* p=0;                                         \
    RW_TRYSTART RWCollectable::tryRecursiveRestore(s,p);        \
    RW_TRYEND(pCl = (className*)p) pCl = (className*)p;         \
    return s;                                                   \
  }                                                             \
                                                                \
  RWEXPORT_RETURN(RWFile&)                                      \
  operator>>(RWFile& f,     className*& pCl)                    \
  {                                                             \
    RWCollectable* p=0;                                         \
    RW_TRYSTART RWCollectable::tryRecursiveRestore(f,p);        \
    RW_TRYEND(pCl = (className*)p) pCl = (className*)p;         \
    return f;                                                   \
  }                                                             \

/////////////////////////////////////////////////////////////////
//                                                             //
//                       RWCollectable                         //
//                                                             //
/////////////////////////////////////////////////////////////////

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWSExport RWCollectable
{

  RWDECLARE_COLLECTABLE(RWCollectable);

public:
  RWCollectable();

// virtual functions
  virtual                       ~RWCollectable();

  virtual RWspace               binaryStoreSize() const;
  virtual int                   compareTo(const RWCollectable*) const;
  virtual unsigned              hash() const;
  virtual RWBoolean             isEqual(const RWCollectable*) const;
  static  inline RWspace        nilStoreSize()
        { return sizeof(RWClassID) + sizeof(int); }
  virtual void                  restoreGuts(RWFile&);
  virtual void                  restoreGuts(RWvistream&);
  virtual void                  saveGuts(RWFile&) const;
  virtual void                  saveGuts(RWvostream&) const;
  RWspace                       recursiveStoreSize() const;
  static RWCollectable*         recursiveRestoreFrom(RWFile&       file,
                                                     RWCollectable*p = 0);
  static RWCollectable*         recursiveRestoreFrom(RWvistream&   vstream,
                                                     RWCollectable*p = 0);
  static void                   tryRecursiveRestore(RWFile&       file,
                                                     RWCollectable*& p);
  static void                   tryRecursiveRestore(RWvistream&   vstream,
                                                     RWCollectable*& p);
  void                          recursiveSaveOn(RWFile&) const;
  void                          recursiveSaveOn(RWvostream&) const;

/** RWStringID section (_no_ virtuals) **/
  RWStringID                    stringID() const
  { return rwGetStringID(isA()); }

  static RWClassID       classID(const RWStringID& id)
  { return rwGetClassID(id); }

  static RWBoolean      isAtom(RWClassID id); 

protected:
#ifdef RW_MULTI_THREAD
  static RWClassID		nextAtom();
#else
  inline static RWClassID       nextAtom()
  {
    // the new atom must be in range [atomBottom,atomTop]
    RWASSERT(RWCollectable::currentAtom_ < atomTop-atomBottom);
    return RWClassID(atomBottom + (RWCollectable::currentAtom_++));
  }
#endif
private:
  static RWClassID      currentAtom_;

// the following are here to provide some name scoping
  static const RWClassID atomBottom; // "smallest unused" RWClassID
  static const RWClassID atomTop;    // "largest unused" RWClassID
  static const long      tool6Magic; // was MAGIC_CONSTANT (for RWFile)
  static const RWClassID dupStridMarker; // seen this StringID before
  static const RWClassID stridMarker; // persisting StringID first time
  static const RWClassID fileRefFlag; // RWFile: seen this object before
};

RWEXPORT_RETURN(RWvostream&)  operator<<(RWvostream& vstream, const RWCollectable* p);
RWEXPORT_RETURN(RWFile&)      operator<<(RWFile& file,        const RWCollectable* p);
extern RWEXPORT_RETURN(RWCollectableP) getRWNilCollectable();

#define RWnilCollectable (getRWNilCollectable()) 

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWCOLLECT_H__ */

