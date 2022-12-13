/***************************************************************************
 *
 * epersist.cc
 *
 * $Id: epersist.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 1.1.1.1  2000/03/02 21:40:40  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.2  1996/03/18 06:49:49  jims
 * Add header
 *
 *
 */

#ifdef RW_NO_STL
// provide functions so we can use apply() to stream data out
#  ifndef RW_NO_REF_TO_REF
template<class T>
void rwInsertInStream(T& item, void*x) {
  *(RWvostream*)x << item;
}

template<class T>
void rwInsertInFile(T& item, void*x) {
  *(RWFile*)x << item;
}
#  else
template<class T>
void rwInsertInStream(T item, void*x) {
  *(RWvostream*)x << item;
}

template<class T>
void rwInsertInFile(T item, void*x) {
  *(RWFile*)x << item;
}
#  endif /* RW_NO_REF_TO_REF */

#endif /* RW_NO_STL */
