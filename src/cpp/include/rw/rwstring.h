#ifndef __RWSTRING_H__
#define __RWSTRING_H__

/*
 * RWString: Backwards compatibility declaration
 *
 * $Id: rwstring.h 18623 2000-03-03 08:28:13Z sih $
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
 * This file is provided for backwards compatibility.  Note, however, that
 * the copy constructor and assignment operator of RWString used reference
 * semantics while the new RWCString uses copy semantics.
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:42  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.3  1996/02/18 01:46:26  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.2  1995/09/05 19:41:47  jims
 * Use new copyright macro
 *
 * Revision 7.1  1994/10/16  02:57:03  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.4  1994/07/18  06:17:36  jims
 * Back out previous change; update test suite instead.
 *
 * Revision 6.3  1994/07/13  04:21:08  nevis
 * #define RWString to RWCString for MetaWare to help
 * solve problems compiling older test suite code
 *
 * Revision 6.2  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:10:51  vriezen
 * Move all files to 6.1
 *
 * Revision 2.2  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 * 
 *    Rev 1.8   01 Mar 1992 15:43:34   KEFFER
 * Now just a typedef to RWCString.
 */

#include "rw/cstring.h"

typedef RWCString RWString;

#endif  /* __RWSTRING_H__ */
