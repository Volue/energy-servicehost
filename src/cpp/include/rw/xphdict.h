#ifndef __RWXPHDICT_H__
#define __RWXPHDICT_H__
/*
 * RWTPtrHashDictionary: A parameterized hashing dictionary using pointers
 *                       to keys of type K and pointers to values of type V.
 *
 * $Id: xphdict.h 18623 2000-03-03 08:28:13Z sih $
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
 * Assumes that KP has:
 *
 *   - well-defined equality semantics (KP::operator==(const KP&)).
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:45  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.6  1996/07/18 20:51:01  hart
 * removed guard around include thashfun.h
 *
 * Revision 7.5  1996/07/18 04:03:18  hart
 * ObjectStore changes
 *
 * Revision 7.4  1996/05/26 23:48:21  hart
 * restructured hashIndex returns
 *
 * Revision 7.3  1996/05/07 18:00:55  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.2  1996/05/02 23:34:09  griswolf
 * Patch to allow degenerate (0 buckets) hash tables Scopus 3146
 *
 * Revision 7.1  1995/11/17 08:53:49  jims
 * Merge old and new families of templates
 *
 * Revision 7.3  1995/09/05 20:22:17  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/04/18  08:34:42  jims
 * Scopus TAN ID# 82: Use RWTExport to use templates with Tools DLL under
 * MSVC 2.x
 *
 * Revision 7.1  1994/10/16  03:00:35  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.4  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.3  1994/06/21  00:34:06  myersn
 * move virtual dtor out-of-line.
 *
 * Revision 6.2  1994/06/15  21:29:52  myersn
 * add buckets() member for use with resize(size_t).
 *
 * Revision 6.1  1994/04/15  19:47:50  vriezen
 * Move all files to 6.1
 *
 * Revision 2.13  1994/01/13  12:08:58  jims
 * KP* val ---> const KP* val   in hashIndex
 *
 * Revision 2.12  1994/01/12  03:09:17  jims
 * Add constness to T* parameters where appropriate
 *
 * Revision 2.11  1994/01/03  23:33:28  jims
 * ObjectStore version: Add #include <ostore/ostore.hh>
 *
 * Revision 2.10  1993/12/31  00:56:30  jims
 * ObjectStore version: add get_os_typespec() static member function
 *
 * Revision 2.9  1993/11/08  12:17:42  jims
 * Port to ObjectStore
 *
 * Revision 2.8  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.7  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.6  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 2.5  1993/02/17  19:07:23  keffer
 * Now uses RWTPtrVector<T> instead of RWTValVector<T> for the buckets
 *
 * Revision 2.4  1993/02/12  00:59:40  keffer
 * Ported to the IBM xlC compiler
 *
 * Revision 2.3  1993/02/07  22:32:54  keffer
 * Added copy constructor and assignment operator for iterator
 *
 * Revision 2.2  1993/01/29  20:02:12  keffer
 * Ported to cfront V3.0
 *
 *    Rev 1.0   02 Mar 1992 16:10:54   KEFFER
 * Initial revision.
 */

#include "rw/thashfun.h"
#include "rw/tpsldict.h"
#include "rw/tpvector.h"

template <class KP, class VP> class RWTExport RWTPtrHashDictionaryIterator;

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template <class KP, class VP> class RWTExport RWTPtrHashDictionary
{

public:

  // Constructors:
  RWTPtrHashDictionary
  (
    RW_HASH_FUNC(KP,hashFun),   // Hashing function
    size_t size = RWDEFAULT_CAPACITY	// No. of buckets
  );

  RWTPtrHashDictionary
  (
    const RWTPtrHashDictionary<KP,VP>&
  );

  ~RWTPtrHashDictionary();

  // Operators:
  RWTPtrHashDictionary<KP,VP> &	operator=(const RWTPtrHashDictionary<KP,VP>&);

  // Look up key, add if not there:
  VP*&			operator[](KP* key);

  // Member functions:
  void			applyToKeyAndValue(void (*applyFun)(KP*,VP*&,void*), void*);

  void			clear();
  void			clearAndDestroy();

  RWBoolean		contains(const KP*) const;	// Contain key?

  size_t		entries() const		{return nitems_;}

  RWBoolean		isEmpty() const		{return nitems_==0;}

  KP*			find(const KP* key) const;
  VP*			findValue(const KP* key) const;
  KP*			findKeyAndValue(const KP* key, VP*& retVal) const;

  void			insertKeyAndValue(KP* key, VP* value)
	{ (*this)[key] = value;}

  KP*			remove(const KP* k);

  void			resize(size_t); // Change # of buckets
  size_t        	buckets() const // report # of buckets
        { return table_.length(); }

protected:

  size_t		hashIndex(const KP* val) const
  {
    return (0 == table_.length()) ? 0 : (size_t)(*hashFun_)(*val) % table_.length();
  }

  RWTPtrVector<RWTPtrSlistDictionary<KP,VP> > table_;
  size_t		nitems_;
  RW_HASH_FUNC(KP,hashFun_);

private:

friend class RWTPtrHashDictionaryIterator<KP,VP>;

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

/****************************************************************
 *								*
 *	Declarations for RWTPtrHashDictionaryIterator<KP,VP>	*
 *								*
 ****************************************************************/

template <class KP, class VP>
class RWTExport RWTPtrHashDictionaryIterator
{

public:

  RWTPtrHashDictionaryIterator(RWTPtrHashDictionary<KP,VP>& s);
  RWTPtrHashDictionaryIterator(const RWTPtrHashDictionaryIterator<KP,VP>& h);
  ~RWTPtrHashDictionaryIterator() {delete iterator_;}

  RWTPtrHashDictionaryIterator<KP,VP>&
		operator=(const RWTPtrHashDictionaryIterator<KP,VP>& h);

  RWBoolean		operator++();		// Advance and test
  KP*			operator()()
    { return ++(*this)? iterator_->key() : rwnil; }

  RWTPtrHashDictionary<KP,VP>* container() const	{return myDict_;}

  KP*			key() const		{return iterator_->key();}

  void			reset();
  void			reset(RWTPtrHashDictionary<KP,VP>& s);

  VP*			value() const		{return iterator_->value();}

private:

  RWTPtrHashDictionary<KP,VP>* myDict_;
  size_t idx_;
  RWTPtrSlistDictionaryIterator<KP,VP>*	iterator_;

  void			nextChain();		// Advance to the next chain

  // Disallow postfix increment.  Unless we hide it, some compilers will
  // substitute the prefix increment operator in its place.
  RWBoolean		operator++(int);

  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("xphdict.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
# include "rw/xphdict.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWXPHDICT_H__ */

