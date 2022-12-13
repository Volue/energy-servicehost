#ifndef __RWTHASHFUN_H__
#define __RWTHASHFUN_H__

/*
 * RWTHashFun: A parameterized wrapper for hash functions for use
 *             with the ObjectStore version of Tools.h++
 *
 * $Id: thashfun.h 18623 2000-03-03 08:28:13Z sih $
 *
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
 ****************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:43  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.2  1996/10/08 23:45:31  kyle
 * ObjectStore update
 *
 * Revision 7.1  1996/07/18 20:45:08  hart
 * OBjectStore change
 *
 * Revision 7.1  1996/07/18 04:35:29  hart
 * ObjectStore changes
 *
 * Revision 1.1.1.1.1.1  1995/08/25  21:10:21  gmf
 * 	* Make it compile.
 *
 * Revision 1.1.1.1  1995/05/25  13:58:47  adama
 * Migrate changes from ClearCase to TRC.
 *
 * Revision 1.1  1993/11/08  11:03:06  jims
 * Initial revision
 *
 */

#include "rw/compiler.h"

#if defined(RW_USE_OBJECTSTORE) || defined(RW_USE_OBJECTSTORE_PSE)
#define RW_HASH_FUNC(TP,NAME) RWTHashFun<TP>* NAME

template <class T>
class RWTHashFun {
  virtual unsigned hashFun(const T&) = 0;
public:
  unsigned operator()(const T& p) { return hashFun(p); }
};

#else
#define RW_HASH_FUNC(TP,NAME) unsigned (*NAME)(const TP&)
#endif

#endif /* __RWTHASHFUN_H__ */

