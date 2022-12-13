#ifndef __RWDEFS_H__
#define __RWDEFS_H__

/*
 * Common definitions
 *
 * $Id: defs.h 18623 2000-03-03 08:28:13Z sih $
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
*/



#ifndef __RWCOMPILER_H__
# include "rw/compiler.h"       /* Set compiler-specific flags */
#endif

STARTWRAP
#include <stddef.h>             /* Looking for size_t */
ENDWRAP


// NEW 
// This macro is introduced for compatibility between code bases using
// both tools7 and tools8.  The issue is tools8 is inside the namespace
// roguewave.  This is mostly solved by the compatibility headers,
// however, for forward declarations, however, the tools8 names must
// be prefaced with the 'roguewave::' scope.  With this macro the
// same forward declarations can work using tools7 and tools8.
#define RW_FWD_CLASS(RWNAME) RWNAME


/*
 * The following two macros are used to help resolve ambiguities between
 * standard library namespace names and other names 
 */

#if (defined(RW_NAMESPACE_REQUIRED) && !defined(RW_NO_STL)) || defined(RW_BCB_NEW_IOSTREAMS)
#    if defined(RW_NO_IOSTD)
#        define RW_SL_IO_STD(RW_NAME)         RW_NAME
#        if defined(__DECCXX) && defined(RW_RWV2X_STDLIB)
#            ifndef RW_HEADER_WITH_EXTENSION
#                include <utility>
#            else
#                include <utility.hpp>
#            endif  /* RW_HEADER_WITH_EXTENSION */
#            define RW_SL_STD_USING_REL_OPS   using namespace std::rel_ops;
#        else
#            define RW_SL_STD_USING_REL_OPS
#        endif
#    else
#       define RW_SL_IO_STD(RW_NAME)    std::RW_NAME
#       if !defined(RW_NO_RELOPS_NAMESPACE)
#            define RW_SL_STD_USING_REL_OPS   using namespace rel_ops;
#       else
#            define RW_SL_STD_USING_REL_OPS
#       endif 
#    endif
#    define RW_SL_STD(RW_NAME)          std::RW_NAME
#    define RW_SL_STD_GLOBAL(RW_NAME)   std::RW_NAME
#    define RW_SL_STD_USING             using namespace std;
#else
#    define RW_SL_STD(RW_NAME)          RW_NAME
#    define RW_SL_IO_STD(RW_NAME)       RW_NAME
#    define RW_SL_STD_GLOBAL(RW_NAME)   ::RW_NAME
#    define RW_SL_STD_USING
#    define RW_SL_STD_USING_REL_OPS
#endif



//
// Macro for forming or omitting default template arguments in constructors
//

#ifndef RW_NO_DEFAULT_TEMPLATE_ARGS
#  define RW_DEFAULT_ARG(n) = n
#else
#  define RW_DEFAULT_ARG(n) 
#endif


/*
 * In rare instances, the following few lines may have to be reworked
 * to deal with naming conflicts.
 */

/*
 * Can't use bool for RWBoolean and stay link compatible. But should
 * adopt bool as soon as it becomes a distinct type, since that is the
 * moment when link compatibility is no issue (new compiler requires
 * recompile). 
 */
#ifndef RW_NO_BOOL
typedef bool RWBoolean;
#  ifndef TRUE
#    define TRUE  true
#    define FALSE false
#  endif
#else
typedef int RWBoolean;
#  ifndef TRUE
#    define TRUE  1
#    define FALSE 0
#  endif
#endif

#define rwnil   0
#define RWNIL   -1L

/*************************************************************************
**************************************************************************
**                                                                      **
**              From here on, it's pretty much boilerplate              **
**              and rarely requires any tuning.                         **
**                                                                      **
**************************************************************************
**************************************************************************/

/*
 *     D E B U G G I N G
 *
 * Use -DRWDEBUG to compile a version of the libraries to debug
 * the user's code.  This will perform pre- and post-condition checks
 * upon entering routines, but will be larger and run more slowly.
 *
 * VERY IMPORTANT!  *All* code must be compiled with the same flag.
 */

#if defined(RDEBUG) && !defined(RWDEBUG)
#  define RWDEBUG 1
#endif

#if defined(RWDEBUG)
#  ifndef RWBOUNDS_CHECK
#    define RWBOUNDS_CHECK 1    /* Turn on bounds checking when debugging. */
#  endif
STARTWRAP
#  include "rw/rwassert.h"
ENDWRAP
#  define RWPRECONDITION(a)     assert( (a) != 0 ) /* Check pre- and post-conditions */
#  define RWPOSTCONDITION(a)    assert( (a) != 0 )
#ifdef RW_NO_NESTED_QUOTES
#  define RWPRECONDITION2(a,b)  assert( (a) != 0 )
#  define RWPOSTCONDITION2(a,b) assert( (a) != 0 )
#else
#  define RWPRECONDITION2(a,b)  assert((b, (a) !=0))
#  define RWPOSTCONDITION2(a,b) assert((b, (a) !=0))
#endif
#  define RWASSERT(a)           assert( (a) != 0 )
#else
#  define RWPRECONDITION(a)
#  define RWPOSTCONDITION(a)
#  define RWPRECONDITION2(a,b)
#  define RWPOSTCONDITION2(a,b)
#  define RWASSERT(a)
#endif

/* Disable USL hook for multi-thread mode: pthread.h defines it too:  */
/* which might make RW use its multithread code in only some obj files.*/
/* #if defined(_REENTRANT) && !defined(RW_MULTI_THREAD)*/
/* # define RW_MULTI_THREAD 1                          */
/* #endif                                              */

#ifdef RW_MULTI_THREAD
#define RWGUARD(mutex)  RWGuard rwdummyguard(mutex)
#else
#define RWGUARD(mutex)
#endif
  
/*
 *     D L L  &  W I N D O W S - S P E C I F I C   C O D E
 *
 * Enable or disable, as necessary, for Microsoft Windows
 */
#if defined(__OS2__) && defined(__TURBOC__) && defined(__DLL__)
#  define BOS2DLL 1
#endif

#if defined(__WIN16__) || defined(__WIN32__) || defined(BOS2DLL)
#  include "rw/rwwind.h"
#  ifdef BOS2DLL
#    undef BOS2DLL
#  endif
#else
   /* Disable Windows hacks if we are not compiling for Windows: */
#  define RWExport
#  define RWGExport
#  define RWTExport
#  define rwtexport
#  define rwexport
#  define RWSExport
#endif


#ifdef RW_TRAILING_RWEXPORT
# define RWEXPORT_RETURN(f) f rwexport
# define RWTEXPORT_RETURN(f) f rwtexport
#else
# define RWEXPORT_RETURN(f) rwexport f
# define RWTEXPORT_RETURN(f) rwtexport f
#endif

/*
 * The following allows getting the declaration for RTL classes
 * right without having to include the appropriate header file
 * (in particular, istream & ostream).
 */
#ifdef __TURBOC__
#  include <_defs.h>    /* Looking for _CLASSTYPE */
#  define _RWCLASSTYPE _CLASSTYPE
#else
#  define _RWCLASSTYPE
#endif

/* No RCS for MS-DOS (it has enough memory problems already!): */
#ifdef __MSDOS__
#define RCSID(a)
#define RW_RCSID(a)
#else
#define RCSID(a) static const char rcsid[] = a
#define RW_RCSID(a) static const char rcsid[] = a
#endif

/* Enable near/far pointers if we are using segmented architecture: */
#if (defined(__MSDOS__) || defined(I8086)) && !defined(__HIGHC__)
# define RW_SEGMENTED_ARCHITECTURE
# if defined(RW_DOUBLE_UNDERBAR_POINTER_QUALIFIERS)
#   define rwnear __near
#   define rwfar  __far
#   define rwhuge __huge
# else
#   define rwnear near
#   define rwfar  far
#   define rwhuge huge
# endif
#else
#  define rwnear
#  define rwfar
#  define rwhuge
#endif

typedef unsigned char  RWByte;    /* Bitflag atomics. */
typedef unsigned short RWClassID; /* Class ID tag. */
typedef unsigned short RWErrNo;   /* Error number */
typedef long           RWoffset;  /* Used for file offsets */

#ifdef __cplusplus

/*
 *   C + +   S P E C I F I C   D E F I N I T I O N S
 */

/*
 * Wrapper classes to avoid unwanted type conversion.
 */
class RWSize_T
{
public:
  RWSize_T(size_t n) : N_(n) {;}
  size_t value() const           {return N_;}
private:
  size_t N_;
};

class RWCS_T
{
public:
  RWCS_T(const char * str) : str_(str) {;}
  const char* value() const     {return str_;}
private:
  const char* str_;
};

enum RWUninitialized { rwUninitialized };
enum RWSeverity {RWWARNING, RWDEFAULT, RWFATAL};

const RWErrNo  RWSUCCESS        = 0;
const size_t RWDEFAULT_CAPACITY = 64; /* Default collection class capacity */
const size_t RWDEFAULT_RESIZE   = 64; /* Default collection class resize */
const size_t RWBITSPERBYTE      = 8;  /* Number of bits per byte */
const size_t RW_NPOS            = ~(size_t)0;

inline double        rwmax(double        a, double        b) {return a>b? a : b;}
inline double        rwmin(double        a, double        b) {return a<b? a : b;}
inline int           rwmax(int           a, int           b) {return a>b? a : b;}
inline int           rwmin(int           a, int           b) {return a<b? a : b;}
inline unsigned      rwmax(unsigned      a, unsigned      b) {return a>b? a : b;}
inline unsigned      rwmin(unsigned      a, unsigned      b) {return a<b? a : b;}
inline unsigned long rwmax(unsigned long a, unsigned long b) {return a>b? a : b;}
inline unsigned long rwmin(unsigned long a, unsigned long b) {return a<b? a : b;}

#ifndef RW_NO_IOSTD
  #include <iosfwd>
#else
  class _RWCLASSTYPE istream;
  class _RWCLASSTYPE ostream;
  class _RWCLASSTYPE ios;
#endif
class RWExport RWvistream;
class RWExport RWvostream;
class RWExport RWFile;
class RWExport RWErrObject;
#ifdef RW_MULTI_THREAD
class RWMutex;
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif
void rwexport RWThrow(RWErrObject a ...);   /* Raise an exception/error */
#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif
/*  For backwards compatibility: */
void RWError(RWSeverity, const char*, const char*);

/*
 * Class ID definitions for Core.h++ classes:
 */
#define __GLOBAL                        0xf000
#define __RWBISTREAM                    0xf001
#define __RWBOSTREAM                    0xf002
#define __RWCLIPSTREAMBUF               0xf003
#define __RWDDESTREAMBUF                0xf004
#define __RWERROBJECT                   0xf005
#define __RWFIXEDMEMORY                 0xf006
#define __RWPISTREAM                    0xf007
#define __RWPOSTREAM                    0xf008
#define __RWVISTREAM                    0xf009
#define __RWVOSTREAM                    0xf00a
/* For historical reasons: */
#define __RWFILE                        0x8020

#ifdef RW_NO_FRIEND_INLINE_DECL
#  define RW_INLINE_FRIEND friend
#else
# ifdef RW_FRIEND_THEN_INLINE
#  define RW_INLINE_FRIEND friend inline
# else
#  define RW_INLINE_FRIEND inline friend
# endif
#endif

#ifdef RW_NO_FRIEND_INLINE_TMPL_DECL
#  define RW_INLINE_TMPL_FRIEND friend
#else
# ifdef RW_FRIEND_THEN_INLINE
#  define RW_INLINE_TMPL_FRIEND friend inline
# else
#  define RW_INLINE_TMPL_FRIEND inline friend
# endif
#endif

/*
 * RW_TRYSTART and RW_TRYEND may be used to hide the code that keeps
 * resources from leaking if an exception is thrown in code between the
 * time of resource allocation and resource release (or resouce transfer
 * to the caller.) Although Stroustrup calls this kind of code "primitive" 
 * (section9.4 of the Grey book), it is well suited to a situation where
 * not all users will have exceptions. (The other alternative is to build 
 * resource-wrapper classes on the stack that will be automatically un-
 * wound if an exception is thrown. While this technique is in some sense 
 * "cleaner" it does add some complexity and a bit of extra code.)
 */
#if defined(RW_NO_EXCEPTIONS)
# define RW_TRYSTART
# define RW_TRYEND(a)
#else
# define RW_TRYSTART try {
# define RW_TRYEND(a) } catch(...) { a; throw; }
#endif
/* 
 * RW_DECLARE_HOME is used at the start of some classes to declare a
 * non-inline virtual function where there would otherwise be none.  This
 * allows many compilers to establish a "home" for the virtual function
 * table.  Otherwise, a virtual function table is built in every
 * compilation unit which includes the class declaration.  RW_DEFINE_HOME
 * is used to generate the implementation of the function.
 * 
 * The purpose of using macros is to hide the fact that these are
 * functions, since they are of no functional use.  Also, it
 * automatically includes the name of the class in the function name
 * which resolves name conflicts when a class inherits from two classes, both
 * with RW_DECLARE_HOME in them. (i.e RWXDRistream)
 */

/* Do not move to top of file; uses "rwexport" from above: */
#include "rw/generic.h"  

#  define RW_DECLARE_HOME(classname) virtual void name2(__RWhome,classname)();
#  define RW_DEFINE_HOME(classname)   void classname::name2(__RWhome,classname)() {}


#ifdef RW_BROKEN_CPP_TOKENS
/* get token-pasting and generic macros */
#  include "rw/generic.h"
#endif

/* 
 * get macros and details of allocation for special circumstances
 * (such as use with ObjectStore)
 */

#  include "rw/rwalloc.h"

#endif /* if C++ */

#endif /* __RWDEFS_H__ */
