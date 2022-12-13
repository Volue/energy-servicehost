#ifndef __RWTSD_H__
#define __RWTSD_H__

/*
 * Declarations for Task Specific Data calls to rwtsd.dll
 *
 * $Id: rwtsd.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.3  1996/02/18 01:46:30  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 19:42:18  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:57:16  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.2  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:10:54  vriezen
 * Move all files to 6.1
 *
 * Revision 1.9  1993/11/22  17:47:04  jims
 * Restore file to sane state (same as rev 1.6)
 *
 * Revision 1.6  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 1.5  1993/08/05  11:40:24  jims
 * Remove exitProc and GlobalRelease... function
 *
 * Revision 1.4  1993/02/10  02:45:35  jims
 * Now compiles under STRICT; RWTSDKEY typedef changed to unsigned long
 *
 * Revision 1.3  1993/02/05  07:55:14  jims
 * Cosmetic changes
 *
 * Revision 1.2  1993/02/03  20:15:55  jims
 * Added test to see if header file already included.
 * Added header and log information
 */

#include <windows.h>

typedef unsigned long RWTSDKEY;

RWTSDKEY FAR PASCAL 
RWGetTaskSpecificKey();

int FAR PASCAL 
RWSetTaskSpecificData(RWTSDKEY hKey, void FAR*);

void FAR* FAR PASCAL 
RWGetTaskSpecificData(RWTSDKEY hKey);

void FAR* FAR PASCAL 
RWReleaseTaskSpecificData(RWTSDKEY hKey);

#endif  /* __RWTSD_H__ */
