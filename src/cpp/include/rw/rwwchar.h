#ifndef __RWWCHAR_H
#define __RWWCHAR_H

/*
 * Standardizes access to wide character facilities of various vendors.
 *
 * $Id: rwwchar.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.8  1996/10/07 22:41:13  griswolf
 * Fix problem with VisualAge compiler and wide chars.
 *
 * Revision 7.7  1996/02/27 00:52:16  griswolf
 * Scopus #2640: move rwwchar.h to rw/ directory.
 *
 * Revision 7.6  1996/02/22 18:25:15  pearson
 * Port to OS2
 *
 * Revision 7.5  1996/02/05 22:24:10  jims
 * Use new macros from the new config to find declarations
 * such as wcschr, wcslen, etc.
 *
 * Revision 7.4  1996/01/19 01:43:13  jims
 * Add AIX 4.1 to list of platforms that use <wchar.h> for towupper etc.
 *
 * Revision 7.3  1995/09/05 22:51:33  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/06/27  20:11:15  jims
 * Include header file wchar.h for OSF on DEC alpha (#1710)
 *
 * Revision 7.1  1994/10/16  04:27:37  josh
 * Merged 6.1 and 7.0 development trees
 *
 *Revision 6.3  1994/07/12  21:14:53  vriezen
 *Updated Copyright
 *
 * Revision 6.2  1994/06/02  18:57:49  foote
 * Port to Symantec 7.0 on Macintosh
 *
 * Revision 6.1  1994/04/15  19:07:47  vriezen
 * Move all files to 6.1
 *
 * Revision 1.8  1993/09/12  21:23:56  keffer
 * Added a typedef for wint_t.
 *
 * Revision 1.7  1993/09/12  21:10:21  keffer
 * All wide character utility functions are now declared in rwwchar.h
 *
 * Revision 1.6  1993/08/09  03:41:48  myersn
 * add #define for wcscspn
 *
 * Revision 1.5  1993/05/25  18:32:23  keffer
 * Ported to NT
 *
 * Revision 1.4  1993/05/24  19:12:22  keffer
 * Ported to xlC compiler.
 *
 * Revision 1.3  1993/05/01  18:22:59  keffer
 * Maps Sun names to X/Open names.
 */


#ifndef __RWTOOLDEFS_H__
# include "rw/tooldefs.h"
#endif

STARTWRAP
#include <stdlib.h>		/* Looking for wcstombs() & MB_CUR_MAX */
#include <string.h>
ENDWRAP


// Do we have to supply definitions?
#ifdef RW_SUPPLY_WSTR


//
// This part used only if we need to supply definitions
//


// For these, the definitions are supplied elsewhere:
extern "C" wchar_t* rwwcschr  (const wchar_t *wstr, wchar_t wc);
extern "C" wchar_t* rwwcsrchr (const wchar_t *wstr, wchar_t wc);
extern "C" wchar_t* rwwcspbrk (const wchar_t *wstr, const wchar_t *wcset);
extern "C" size_t   rwwcsspn  (const wchar_t *wstr, const wchar_t *wcset);
extern "C" size_t   rwwcscspn (const wchar_t *wstr, const wchar_t *wcset);

#define wcschr  rwwcschr
#define wcsrchr rwwcsrchr
#define wcspbrk rwwcspbrk
#define wcsspn  rwwcsspn
#define wcscspn rwwcscspn


#ifdef __WIN32__

// The following implementations rely on the WIN32 API.

typedef wchar_t wint_t;		// Required for BCC4

#include <windows.h>

inline wchar_t rwtowupper(wchar_t wc)
{
  return LOWORD(CharUpperW((wchar_t*)MAKELONG(wc, 0)));
}

inline wchar_t rwtowlower(wchar_t wc)
{
  return LOWORD(CharLowerW((wchar_t*) MAKELONG(wc, 0)));
}

inline int rwisw_aux(wint_t wc, USHORT ctype)
{
  WORD result;
  BOOL retval;

  // set bits in result according to type of wc:
  retval = GetStringTypeW(CT_CTYPE1, &wc, 1, &result);

  RWASSERT(retval);
  return result & ctype;  // return true if relevant bit is set
}

inline int rwiswspace (wint_t wc) { return rwisw_aux(wc, C1_SPACE); }
inline int rwiswalpha (wint_t wc) { return rwisw_aux(wc, C1_ALPHA); }
inline int rwiswlower (wint_t wc) { return rwisw_aux(wc, C1_LOWER); }
inline int rwiswupper (wint_t wc) { return rwisw_aux(wc, C1_UPPER); }
inline int rwiswdigit (wint_t wc) { return rwisw_aux(wc, C1_DIGIT); }
inline int rwiswxdigit(wint_t wc) { return rwisw_aux(wc, C1_XDIGIT);}

inline size_t rwwcslen(const wchar_t *wstr)
{
  return lstrlenW(wstr);
}

inline wchar_t *rwwcscpy(wchar_t *dest, const wchar_t *src)
{
  return lstrcpyW(dest, src);
} 	

inline int rwwcscoll(const wchar_t *wstr1, const wchar_t *wstr2)
{
  return lstrcmpW(wstr1, wstr2);
}	



#define iswalpha  rwiswalpha
#define iswdigit  rwiswdigit
#define iswlower  rwiswlower
#define iswspace  rwiswspace
#define iswupper  rwiswupper
#define iswxdigit rwiswxdigit
#define towlower  rwtowlower
#define towupper  rwtowupper
#define wcscoll   rwwcscoll
#define wcscpy    rwwcscpy
#define wcslen    rwwcslen

#else

Error!  Unable to supply definitions for wide character functions!

#endif /* __WIN32__ */




#else  /* !RW_SUPPLY_WSTR */


//
// This section is used if compiler-supplied definitions are available.
// Now where are we going to get them?
//

#ifdef RW_USE_WCHEADERS

STARTWRAP
#include <wctype.h>
#include <ctype.h>
#include <widec.h>
#include <string.h>
ENDWRAP

// Map the X/OPEN names to the sun SVR4 names:
#  ifdef sun
#    define wcschr  wschr
#    define wcscoll wscoll
#    define wcscpy  wscpy
#    define wcslen  wslen
#    define wcsncpy wsncpy
#    define wcspbrk wspbrk
#    define wcspbrk wspbrk
#    define wcsrchr wsrchr
#    define wcsspn  wsspn
#    define wcscspn wscspn
#    define wcsxfrm wsxfrm
#  endif

#else
#ifdef RW_WSTR_C_HEADERS

STARTWRAP
#include <ctype.h>
#include <string.h>
ENDWRAP

#else

STARTWRAP
#if defined(RW_WCTYPEH_NEEDED)
#  include <wctype.h>
#endif

#include <wchar.h>
ENDWRAP

#endif
#endif

#endif /* RW_SUPPLY_WSTR */

#endif	/* __RWWCHAR_H */
