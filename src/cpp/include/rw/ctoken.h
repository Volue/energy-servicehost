#ifndef __RWCTOKEN_H__
#define __RWCTOKEN_H__

/*
 * RWCTokenizer --- converts strings into sequences of tokens
 *
 * $Id: ctoken.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.6  1996/07/18 04:33:02  hart
 * ObjectStore changes
 *
 * Revision 7.5  1996/05/08 16:59:31  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.4  1996/02/18 01:39:47  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.3  1995/09/05 18:27:53  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/07/07  23:00:56  griswolf
 * Scopus #1356: correct handling for embedded nulls.
 *
 * Revision 7.1  1994/10/16  01:00:32  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  18:46:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.2  1994/06/22  22:24:53  vriezen
 * Bug# 553.  Use value semantics on constructor
 *
 * Revision 6.1  1994/04/15  19:09:46  vriezen
 * Move all files to 6.1
 *
 * Revision 1.2  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.1  1993/02/11  00:51:04  myersn
 * Initial revision
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.4   01 Mar 1992 15:45:24   KEFFER
 * Now uses RWCString instead of RWString.
 * 
 *    Rev 1.2   28 Oct 1991 09:08:26   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.1   22 Aug 1991 10:43:26   keffer
 * operator()() no longer uses default arguments (as per the ARM).
 * 
 *    Rev 1.0   28 Jul 1991 08:17:36   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 *
 */

#include "rw/cstring.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWCTokenizer {
  const RWCString       theString;
  const char*           place;
public:
  RWCTokenizer(const RWCString& s);     // Construct to lex a string

  // Advance to next token, delimited by s:
  RWCSubString          operator()(const char* s);
  RWCSubString          operator()(const char*, size_t); //allow nulls
  RWCSubString          operator()(); // { return operator()(" \t\n\000",4); }

  RW_TYPESPEC  /* This macro usually expands to nothing */
};

typedef RWCTokenizer RWTokenizer;  // backward compatibility

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWCTOKEN_H__ */
