#ifndef __RWRSTREAM_H__
#define __RWRSTREAM_H__

/*
 * Includes either stream.h or iostream.h, depending
 * on the compiler.
 *
 * $Id: rstream.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/defs.h"

#  ifdef __GLOCK__
#    include <iostream.hxx>
#  else
#    ifndef RW_NO_IOSTD
#      include <iostream>
#    else

#      include <iostream.h>
#    endif
#  endif

inline RW_SL_IO_STD(istream)& rwEatwhite(RW_SL_IO_STD(istream)& s) {return s >> RW_SL_IO_STD(ws);}

#ifdef RW_BOOL_NEEDS_IO

inline RW_SL_IO_STD(ostream)& operator<<(RW_SL_IO_STD(ostream)& os, bool b) 
    { os << ( b ? '1' : '0'); return os;}
inline RW_SL_IO_STD(istream)& operator>>(RW_SL_IO_STD(istream)& is, bool &b) 
    { char c; is >> c; b = (c == '1' ? true : false); return is;}
#endif

#endif /* __RWRSTREAM_H__ */
