#ifndef __RWREGEXP_H__
#define __RWREGEXP_H__

/*
 * Declarations for class RWCRegexp --- Regular Expression
 *
 * $Id: regexp.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.7  1996/08/28 19:27:12  hart
 * ODI change
 *
 * Revision 7.6  1996/07/18 04:16:54  hart
 * ObjectStore changes
 *
 * Revision 7.5  1996/05/07 22:24:56  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.4  1996/03/16 14:39:23  jims
 * Use RWSExport for RWCString so that Symantec properly
 * handles static data members in 32-bit DLLs
 *
 * Revision 7.3  1996/02/18 01:45:38  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 19:19:01  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:54:18  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:10:39  vriezen
 * Move all files to 6.1
 *
 * Revision 2.9  1994/01/18  23:37:39  myersn
 * make enumeration RWCRegexp::statVal match return values from rwmakepat().
 *
 * Revision 2.8  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.7  1993/05/19  00:01:25  keffer
 * Constructor takes const char*
 *
 * Revision 2.6  1993/05/14  00:20:27  myersn
 * add RWCRegexp(const RWCString&) constructor.
 *
 * Revision 2.4  1993/02/14  05:25:40  myersn
 * change indices from int to size_t, with RW_NPOS as the bad index.
 *
 * Revision 2.3  1993/01/29  20:28:44  myersn
 * change pattern string type to allow 8-bit clean matching.
 *
 * Revision 2.1  1992/11/16  04:10:24  keffer
 * Changed name from RWRegexp to RWCRegexp, leaving a typedef
 * for backwards compatibility.
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.3   04 Mar 1992 10:22:14   KEFFER
 * RWString -> RWCString
 * 
 *    Rev 1.1   28 Oct 1991 09:08:20   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.0   28 Jul 1991 08:16:10   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 *
 */

#ifndef __RWTOOLDEFS_H__
# include "rw/tooldefs.h"
#endif

#if defined(RWTOOLS) && (RWTOOLS >= 0x0600)
  typedef unsigned short RWPatternType;
#define RWPatternType_typespec RWAlloc::typespec_ushort
#else
  typedef unsigned char  RWPatternType;
#define RWPatternType_typespec RWAlloc::typespec_uchar
#endif

class RWSExport RWCString;

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWCRegexp
{
public:
  enum statVal {OK=0, ILLEGAL, NOMEM, TOOLONG RWOS_HETERO_ENUM_PAD};
  RWCRegexp(const char*);
  RWCRegexp(const RWCString&);
  RWCRegexp(const RWCRegexp&);
  ~RWCRegexp();

  RWCRegexp&    operator=(const RWCRegexp&);
  RWCRegexp&    operator=(const RWCString&);    // Recompiles pattern
  RWCRegexp&    operator=(const char*);         // Recompiles pattern
  size_t        index(const RWCString& str, size_t* len, size_t start=0) const;
  statVal       status();       // Return & clear status
private:
  void          copyPattern(const RWCRegexp&);
  void          genPattern(const char*);
  RWPatternType* thePattern_;   // Compiled pattern
  statVal       stat_;          // Status
  static const unsigned maxpat_;        // Max length of compiled pattern
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};

typedef RWCRegexp RWRegexp;     // For backwards compatibility

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWREGEXP_H__ */
