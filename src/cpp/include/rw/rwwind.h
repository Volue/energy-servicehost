#ifndef __RWWIND_H__
#define __RWWIND_H__

/*
 * Microsoft Windows related directives.
 *
 * $Id: rwwind.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 1.1.1.1  2000/03/02 21:40:43  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.17  1996/09/27 18:50:22  hart
 *  VisualAge 3.5 support for DLL's changed
 *
 * Revision 7.16  1996/07/18 04:25:51  hart
 * ObjectStore changes
 *
 * Revision 7.15  1996/04/12 16:48:51  griswolf
 * And... be sure exes using 32-bit DLLs do not see exportish things
 * either.
 *
 * Revision 7.14  1996/04/08 17:04:01  griswolf
 * And... be sure that 16-bit non-DLLs do not see any exportish things.
 *
 * Revision 7.13  1996/04/04 18:15:08  griswolf
 * Correct the "exportness" of global template functions for 16-bit DLLs.
 *
 * Revision 7.12  1996/03/12 23:54:18  pearson
 * Remove redefiniton of RWTExport and RWGExport
 *
 * Revision 7.11  1996/03/07 02:24:22  pearson
 * Make Watcom dll's like Microsoft's for version 10.5
 *
 * Revision 7.10  1996/02/18 01:46:32  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.9  1995/12/01 18:56:02  pearson
 * Make Symantec like MSVC except use declspec(dllimport) with classes which
 * have static data members initialized in DLL code
 *
 * Revision 7.8  1995/11/22 20:04:43  pearson
 * Revise RWExport related definitions for Symantec DLL's
 *
 * Revision 7.7  1995/10/23 17:36:29  pearson
 * Revise to handle Watcom C++ 10.5 dlls
 *
 * Revision 7.6  1995/09/20  08:05:59  jims
 * Define RWGExport for Microsoft when building DLL, otherwise do not
 * define it
 *
 * Revision 7.5  1995/09/18  19:33:46  jims
 * Turn off RWGExport in DLLs for Microsoft along with turning off
 * RWTExport
 *
 * Revision 7.4  1995/09/05  19:42:32  jims
 * Use new copyright macro
 *
 * Revision 7.3  1995/07/26  09:36:52  jims
 * Use _import instead of huge for Borland
 *
 * Revision 7.2  1995/07/26  06:40:43  jims
 * Scopus #1114: define RWGExport and RWTExport depending on
 * _RWEXPORT_INSTANTIATED
 *
 * Revision 7.1  1994/10/16  02:57:20  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.2  1994/05/16  18:07:38  jims
 * Port to Win32 DLL
 *
 * Revision 6.1  1994/04/15  19:10:55  vriezen
 * Move all files to 6.1
 *
 * Revision 2.6  1993/09/11  10:32:21  jims
 * Move toward support for DLLs under Win32
 *
 * Revision 2.5  1993/09/03  02:08:13  keffer
 * Macro _CLASSDLL is now named _RWTOOLSDLL
 *
 * Revision 2.4  1993/08/05  11:44:46  jims
 * Port to WIN32
 *
 * Revision 2.3  1993/04/10  23:48:21  jims
 * No longer sets RW_MULTI_THREAD flag for DLL situation
 *
 * Revision 2.2  1993/02/07  17:41:54  keffer
 * Moved the definition for _RWCLASSTYPE from rwwind.h to
 * defs.h; this allows the library to be used with far virtual
 * tables by non-Windows code.
 *
 * Revision 2.1  1993/01/15  22:23:49  keffer
 * Tools.h++ V5.2 alpha
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.4   22 Jun 1992 17:52:50   KEFFER
 * Now allows a DLL to be built that uses the Tools.h++ DLL.
 * 
 *    Rev 1.3   28 May 1992 11:04:28   KEFFER
 * Can now build static Windows library using Microsoft C/C++ 7.0
 * 
 *    Rev 1.2   17 Mar 1992 12:00:14   KEFFER
 * Fine-tuned.
 * 
 *    Rev 1.1   18 Feb 1992 09:54:42   KEFFER
 * 
 *    Rev 1.0   17 Nov 1991 13:40:14   keffer
 * Initial revision.
 */

/*
 * Special stuff for 16-bit Windows (__WIN16__) 
 * and Windows NT / Win32s (__WIN32__).
 *
 * Under Windows, these header files can be used in two different modes: 
 * as part of a DLL, or as part of a regular (static) library.  
 * For each of these two ways, we can either be COMPILING the library,
 * or USING the library.  This makes a matrix of four possible situations.
 *
 *************************************************************************
 ******************************  DLL *************************************
 *************************************************************************
 *
 *                         Creating a DLL
 *
 * The macro __DLL__ should be define when compiling to create a DLL.
 *
 *     The Borland compilers automatically do this if either the -WD or 
 *     -WDE switch is being used.  In this situation, the macros 
 *     RWExport and rwexport expand to _export.
 *
 *     For other compilers, we must define __DLL__ where appropriate
 *     if the compiler doesn't.  See the example for Microsoft below.
 *
 *     RWExport and rwexport expand to _export by default.  Massage as
 *     necessary for your compiler; again see below for the Microsoft
 *     specific directives.
 *
 *                       Using the resultant DLL
 *
 * In order to use the resultant DLL, you must define the macro _RWTOOLSDLL
 * when compiling.  This will communicate to the Rogue Wave header files
 * your intention to use a DLL version of the Rogue Wave library.
 *
 * If you intend to use a DLL version of the Borland RTL as well, then you
 * should also define the macro _RTLDLL, as per Borland's instructions.
 *
 * You must also specify the -WS switch ("smart callbacks") for the 
 * Borland Compiler.
 *
 *
 *************************************************************************
 ***********************  Static Windows Library **************************
 *************************************************************************
 *
 *
 *                Creating a RW static Windows library
 *
 * If your intention is to create a Rogue Wave library to be used as 
 * as a static Windows library, then one of the macros __WIN16__ or __WIN32__
 * must have been defined in compiler.h (or by the compiler directly).
 *
 * Borland:     __WIN16__ will be defined if both _Windows and __MSDOS__ 
 *              is defined.  _Windows will be defined automatically if 
 *              any -W switch is used.  Borland defines __WIN32__ automatically
 *              in their 32-bit compiler for Windows NT / Win32s.
 *
 * Microsoft:   Microsoft automatically defines _WINDOWS if you use the
 *              /GA (preferred) or /GW switch.  __WIN16__ will be defined 
 *              if both _WINDOWS and _MSDOS are defined.
 *              __WIN32__ will only be defined if either it, or WIN32, 
 *              is explicitly defined on the cl386 command line.
 *
 * 
 *                 Using a RW static Windows library
 *
 * Nothing special needs to be done.  Just link compile with the appropriate
 * compile switches and link in the RW static Windows library.
 *
 */

/*
 * Check for Microsoft C/C++ and massage as necessary.
 */
#if defined(_MSC_VER)
#  if defined(_WINDLL) && !defined(__DLL__)
#    define __DLL__ 1
#  endif
#  if !defined(_export)
#    define _export __export
#  endif
#endif

#if defined(RWDLL)
#  ifndef _RWTOOLSDLL
#    define _RWTOOLSDLL 1
#  endif
#endif

/* For backwards compatibility: */
#if defined(_RWCLASSDLL) && !defined(_RWTOOLSDLL)
# define _RWTOOLSDLL 1
#endif

#if defined(_RWTOOLSDLL)
#  if !defined(__LARGE__) && !defined(__WIN32__) && !defined(BOS2DLL)
#   error Use large or flat memory model to compile or use the Tools.h++ DLL!
#  endif

#  if defined(_RWBUILDDLL)
     // Compiling the Tools.h++ DLL.
#    if !defined(_MSC_VER) && !defined(__SC__) && !defined(__WATCOMC__) && !defined(__IBMCPP__)
#      define RWExport _export                  /* Mark classes as exported */
#      define rwexport _export                  /*    & functions as well   */
#      define RWGExport RWExport
#      define RWTExport RWExport
#      define rwtexport rwexport
#    else /* Microsoft: */
#      define RWExport __declspec(dllexport)
#      define rwexport __declspec(dllexport)
#      define RWGExport RWExport
#      define RWTExport
#      define rwtexport
#    endif
#  else
     // Using the Tools.h++ DLL.
#    if defined(__WIN16__)
#      define RWExport _import  /* Mark classes as huge  */
#      define rwexport far      /*    & functions as far */
#      define rwtexport far
#    elif defined(__WIN32__)
#      if !defined(_MSC_VER) && !defined(__WATCOMC__) && !defined(__IBMCPP__)
#      if defined(__SC__)
#        define RWExport
#        define rwexport
#        define rwtexport
#      else 
#        define RWExport _import
#        define rwexport
#        define rwtexport
#      endif
#      else
#        define RWExport  __declspec(dllimport)
#        define rwexport
#        define rwtexport
#      endif  
#    elif defined(BOS2DLL)
#      define RWExport
#      define rwexport
#      define rwtexport
#    endif /* WIN16 || WIN32 || BOS2DLL */
#  endif /*_RWBUILDDLL or not */
#else
   // Neither compiling, nor using the Tools.h++ DLL.
#  define RWExport
#  define rwexport
#  define rwtexport
#endif

/*
 * If the programmer is building a DLL that uses the Tools DLL,
 * code for the template and generic classes will actually find its
 * way into the programmer's DLL.  To have those classes exported,
 * define _RWEXPORT_INSTANTIATED (feature not available with Microsoft) 
 */

#ifndef _RWBUILDDLL
#  ifdef _RWEXPORT_INSTANTIATED
#    if !defined(_MSC_VER) && !defined(__WATCOMC__) && !defined(__IBMCPP__)
#      define RWGExport _export
#      define RWTExport _export
#    else
#      define RWGExport 
#      define RWTExport /* MS does not allow dllexport for templates */
#    endif
#  else
#    if !defined(_MSC_VER) && !defined(__WATCOMC__) && !defined(__IBMCPP__)
#      define RWGExport RWExport
#      define RWTExport RWExport
#    else
#      define RWGExport
#      define RWTExport 
#    endif
#  endif
#endif


//This definition is for a Symantec C++ 7.2 bug, where the compiler crashes
//when certain classes are explicitly imported

#define RWSExport RWExport
#if defined(__SC__) && !defined(_RWBUILDDLL) && defined(_RWTOOLSDLL)
# define RWSExport __declspec(dllimport)
#endif

#endif /* __RWWIND_H__ */
