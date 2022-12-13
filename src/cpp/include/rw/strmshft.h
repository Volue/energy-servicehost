#ifndef __RW_VSTREAM_SHIFT_H__
#define __RW_VSTREAM_SHIFT_H__

/*
 * Macros for use with virtual streams
 *
 * $Id: strmshft.h 18623 2000-03-03 08:28:13Z sih $
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
 * The following macros are provided for use when compilers complain of
 * ambiguity between RWvostream,Collectable and RWderivedStream,builtin.
 * Rogue Wave "vstreamable" class headers now use these macros to provide
 * complete dis-ambiguation for the streams and classes that ship with
 * the Tools.h++ library.
 *
 * Since the functions are global inlines, the macros should be expanded
 * in a header file that can be #included only once per compilation unit.
 * 
 *  // Here is an example
 *  // You need not provide a full declaration of the stream class here
 *  class RWExport myOStream; // derived from RWvostream
 *
 *  // You need not provide a full declaration of the insertable class here
 *  class RWExport myClass;   // can be inserted into an RWvostream
 *
 *  // use the macro to provide an inline definition of the << operator
 *  RW_PROVIDE_DVSTREAM_INSERTER(myOStream,myClass)
 *
 * Similar code can be used to provide extractor methods.
 */

#include "rw/compiler.h"

class RWExport RWvostream;
class RWExport RWvistream;
#define RW_PROVIDE_DVSTREAM_INSERTER(DerivedOstream,vstreamable)         \
inline RWvostream& operator<<(DerivedOstream& dos, const vstreamable& ct)\
        { return RW_AWFUL_OCAST(dos) << ct; }

#define RW_PROVIDE_DVSTREAM_EXTRACTOR(DerivedIstream,vstreamable)        \
inline RWvistream& operator>>(DerivedIstream& dis, vstreamable& ct)      \
        { return RW_AWFUL_ICAST(dis) >> ct; }

#endif /* __RW_VSTREAM_SHIFT_HELP__ */
