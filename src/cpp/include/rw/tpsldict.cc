/*
 * Template definitions for RWTPtrSlistDictionary<K,V>
 * and RWTPtrSlistDictionaryIterator<K,V>
 *
 * $Id: tpsldict.cc 18623 2000-03-03 08:28:13Z sih $
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
 ****************************************************************************/

// Copy constructor (some of these names get pretty bloody long, eh?)
template <class KP, class VP>
RWTPtrSlistDictionary<KP,VP>::RWTPtrSlistDictionary(const RWTPtrSlistDictionary<KP,VP>& d)  {

  // Cast away "constness", which we will honor anyway...
  RWTPtrSlistDictionaryIterator<KP,VP> next((RWTPtrSlistDictionary<KP,VP>&)d);

  while (++next)
    insertKeyAndValue(next.key(), next.value());

  RWPOSTCONDITION(entries()==d.entries());
}

template <class KP, class VP> RWTPtrSlistDictionary<KP,VP>&
RWTPtrSlistDictionary<KP,VP>::operator=(const RWTPtrSlistDictionary<KP,VP>& d)
{
  if (this!=&d)
  {

    clear();

    // Cast away "constness", which we will honor anyway...
    RWTPtrSlistDictionaryIterator<KP,VP> next((RWTPtrSlistDictionary<KP,VP>&)d);

    while (++next)
      insertKeyAndValue(next.key(), next.value());

  }
  RWPOSTCONDITION(entries()==d.entries());
  return *this;
}

template <class KP, class VP> VP*&
RWTPtrSlistDictionary<KP,VP>::operator[](KP* key)
{
  RWTPtrAssocLink<KP,VP>* assoc = findLink(key);

  if (assoc==rwnil)
    RWTIsvSlist<RWTPtrAssocLink<KP,VP> >
      ::insert(assoc = RW_NEW(this,(RWTPtrAssocLink<KP,VP>::get_os_typespec()))
                RWTPtrAssocLink<KP,VP>(key));

  RWPOSTCONDITION(*key == *(assoc->key_));
  return assoc->value_;
}

template <class KP, class VP> void
RWTPtrSlistDictionary<KP,VP>::applyToKeyAndValue(void (*applyFun)(KP*,VP*&,void*), void* a)
{
  RWTPtrAssocLink<KP,VP>* link = this->firstLink();
  while (link != this->tailLink())
  {
    applyFun(link->key_, link->value_, a);
    link = link->next();
  }
}

template <class KP, class VP> void
RWTPtrSlistDictionary<KP,VP>::clearAndDestroy()
{
  RWTPtrAssocLink<KP,VP>* link = this->firstLink();
  while (link != this->tailLink())
  {
    RWTPtrAssocLink<KP,VP>* next = link->next();
    delete link->key_;
    delete link->value_;
    delete link;
    link = next;
  }
  this->init();
}

template <class KP, class VP> RWBoolean
RWTPtrSlistDictionary<KP,VP>::contains(const KP* key) const
{
  return findLink(key)!=rwnil;
}

template <class KP, class VP> KP*
RWTPtrSlistDictionary<KP,VP>::find(const KP* key) const
{
  RWTPtrAssocLink<KP,VP>* assoc = findLink(key);
  return assoc ? assoc->key_ : rwnil;
}

template <class KP, class VP> KP*
RWTPtrSlistDictionary<KP,VP>::findKeyAndValue(const KP* key, VP*& retVal) const
{
  RWTPtrAssocLink<KP,VP>* assoc = findLink(key);
  if (assoc)
  {
    retVal = assoc->value_;
    RWPOSTCONDITION( *key == *assoc->key_ );
    return assoc->key_;
  }
  return rwnil;
}

template <class KP, class VP> VP*
RWTPtrSlistDictionary<KP,VP>::findValue(const KP* key) const
{
  RWTPtrAssocLink<KP,VP>* assoc = findLink(key);
  return assoc ? assoc->value_ : rwnil;
}

template <class KP, class VP> KP*
RWTPtrSlistDictionary<KP,VP>::remove(const KP* key)
{
  RWTPtrAssocLink<KP,VP>* assoc = removeLink(key);
  if (assoc)
  {
    KP* retKey = assoc->key_;
    delete assoc;
    RWPOSTCONDITION(*retKey == *key);
    return retKey;
  }
  return rwnil;
}

template <class KP, class VP> KP*
RWTPtrSlistDictionary<KP,VP>::removeKeyAndValue(const KP* key, VP*& retVal)
{
  RWTPtrAssocLink<KP,VP>* assoc = removeLink(key);
  if (assoc)
  {
    KP* retKey = assoc->key_;
    retVal     = assoc->value_;
    delete assoc;
    RWPOSTCONDITION(*retKey == *key);
    return retKey;
  }
  return rwnil;
}


// Protected functions ---

template <class KP, class VP> RWTPtrAssocLink<KP,VP>*
RWTPtrSlistDictionary<KP,VP>::findLink(const KP* key) const
{
  RWTPtrAssocLink<KP,VP>* link = this->firstLink();
  while (link != this->tailLink())
  {
    if (*link->key_ == *key)
      return link;
    link = link->next();
  }
  return rwnil;
}


template <class KP, class VP> RWTPtrAssocLink<KP,VP > *
RWTPtrSlistDictionary<KP,VP>::removeLink(const KP* key)
{
  RWPRECONDITION(this->lastLink()->next()==this->tailLink());

  // Set up a sentinel (casting away const: DO NOT MODIFY *newTail.key_):
  RWTPtrAssocLink<KP,VP> newTail((KP*) key);
  this->lastLink()->next_ = &newTail;

  // Now search for it:
  RWTPtrAssocLink<KP,VP>* link = this->headLink();
  while (!(*link->next()->key_ == *key))
    link = link->next();

  // Reset the pointer in the last link:
  this->lastLink()->next_ = this->tailLink();

  if (link != this->lastLink())
  {
    RWPOSTCONDITION(*link->next()->key_ == *key);
    //return removeRight(link);
    return ((RWTPtrAssocLink<KP,VP> *) removeRight(link));
  }
  return rwnil;
}

// BUG 117
template <class KP, class VP>
RWTPtrSlistDictionaryIterator<KP,VP>::RWTPtrSlistDictionaryIterator
    (const RWTPtrSlistDictionaryIterator<KP,VP>& sdi)
  : RWTIsvSlistIterator<RWTPtrAssocLink<KP,VP> >(sdi)
{ ; }

