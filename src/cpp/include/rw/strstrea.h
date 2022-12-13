#ifndef __RWSTRSTREA_H__
#define __RWSTRSTREA_H__

/***************************************************************************
 *
 * strstrea.h
 * Includes either strstream.h or strstrea.h, depending
 * on the compiler.
 *
 * $Id: strstrea.h 18623 2000-03-03 08:28:13Z sih $
 * $Id: strstrea.h 18623 2000-03-03 08:28:13Z sih $
 *
 ***************************************************************************
 *
 * Copyright (c) 1991-1999 Rogue Wave Software, Inc.  All Rights Reserved.
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
 * This file exists because on the Microsoft NT compiler, trying
 * to include <strstream.h> fails.  For some stupid reason, the
 * NT compiler doesn't find strstrea.h, even though the headers are
 * stored on a fat file system.
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:43  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 1.2.1.2  1996/05/20 19:53:34  bass
 * check for __WIN16__ or __MSDOS__ needed if compiling under Win95
 *
 * Revision 1.2.1.1  1996/05/15 20:32:26  bass
 * start new branch for Math.h++ 5.2
 *
 * Revision 1.4  1996/03/25 19:41:47  tibbi
 * added the copyright macros with the mathyrs.str file
 *
 * Revision 1.3  1995/08/10 18:23:43  steves
 * add OS/2 to the list of os's that need to include strstrea.h
 *
 * Revision 1.2  1993/10/04  21:44:59  alv
 * ported to Windows NT
 *
 * Revision 1.1  1993/10/01  22:20:09  alv
 * Initial revision
 *
 **************************************************************************/

#include "rw/defs.h"   /* Need to know about __WIN32__ */

#ifndef RW_NO_IOSTD
#  include <strstream>
#else
#  if defined(__WIN32__) || defined(__WIN16__) || defined(__MSDOS__) || defined(__OS2__)
#    include <strstrea.h>
#  else
#    include <strstream.h>
#  endif
#endif

#endif /* __RWSTRSTREA_H__ */
