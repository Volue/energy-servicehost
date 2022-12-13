/***************************************************************************
 *
 * ep_cntnr.cc - template definitions for RW_PBase<StdColl,RWColl,DataTypes>
 * 
 * $Id: ep_cntnr.cc 18623 2000-03-03 08:28:13Z sih $
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



#define INFILE_TEMPL_DECL template <class StdColl, class RWColl>
#define PC       RW_PBase<StdColl,RWColl>


/* 
 *  PLEASE NOTE THE ABNORMAL STRUCTURE OF THIS FILE
 *
 *  The file has two major sections. The first section (between the #ifdef and
 *  the #endif for RW_SHUT_INLINES_GUARD_OFF...) contains templated class
 *  member functions which are expressed in such a way that they can be included
 *  either from within a class body or as "inlines" from outside the class body.
 *  The "INLINES_GAURD" assures that they are never seen both ways.
 *
 *  The second section of this file (between the #ifndef and the #endif of the
 *  macro RW_GET_INLINES_FOR_CLASS BODY contains the remaining templated class
 *  member functions which can only be seen from outside the class body.
 *
 *  Please modify or add new member definitions to this file with this scheme in
 *  mind. Placing new members in the wrong section will most assuredly break this
 *  scheme. If members can ever be seen from within a class body they must be placed
 *  in the first section.
 *
 *  NOTE: This scheme is commented in greatest detail in files slist.cc and slist.h.
 */


/*  
 *  This section of the include file is for members that might in some cases be 
 *  included from within their own class bodies.  Their function signatures and 
 *  template designators are constructed with macros whose expansion depends
 *  upon the macro RW_GET_INLINES_FOR_CLASS_BODY. (see below)
 *
 *  The guard RW_SHUT_INLINES_GUARD_OFF__SLIST_CC helps to assure that the
 *  compiler will NOT see this section twice, once from within a class body
 *  and once from outside the class.
 */
#ifdef RW_GET_INLINES_FOR_CLASS_BODY
#   define RW_TEMPLATE_DECLTR__RW_PBASE
#   define RW_SCOPE_DECLTR__RW_PBASE
#   define RW_CONDITIONAL_TYPENAME 
#else
#   define RW_TEMPLATE_DECLTR__RW_PBASE template<class StdColl, class RWColl>
#   define RW_SCOPE_DECLTR__RW_PBASE RW_PBase<StdColl,RWColl>::
#   define RW_CONDITIONAL_TYPENAME typename
#endif

#if (!defined(RW_GOT_INLINES_FOR_CLASS_BODY_EP_CNTNR_CC) && !defined(__SUNPRO_CC)) || (defined(RW_GET_INLINES_FOR_CLASS_BODY) && defined(__SUNPRO_CC))
RW_TEMPLATE_DECLTR__RW_PBASE
RW_CONDITIONAL_TYPENAME RW_SCOPE_DECLTR__RW_PBASE size_type
RW_SCOPE_DECLTR__RW_PBASE index(
	bool (*fn)(RW_CONDITIONAL_TYPENAME RW_SCOPE_DECLTR__RW_PBASE value_type, void*),
	 void* x
	 ) const
{
  const_my_iter_ i = begin();
  size_type idx = 0;
  for(/**/; i != end(); ++i) {
    if((*fn)(*i,x))
      return idx;
    ++idx;
  }
  return npos;
}



RW_TEMPLATE_DECLTR__RW_PBASE
RW_CONDITIONAL_TYPENAME RW_SCOPE_DECLTR__RW_PBASE size_type
RW_SCOPE_DECLTR__RW_PBASE occurrencesOf(
		bool (*fn)(RW_CONDITIONAL_TYPENAME RW_SCOPE_DECLTR__RW_PBASE value_type,void*),
		 void* x
		 ) const
{
  size_type ret = 0;
  const_my_iter_ iter = begin();
  while(iter != end()) {
    if((*fn)(*iter,x)) ++ret;
    ++iter;
  }
  return ret;
}


#endif // ifndef RW_GOT_INLINES_FOR_CLASS_BODY_EP_CNTNR_CC




/*
 *  The remainder of this file is for member definitions that will never 
 *  be seen from inside a class body.
 */

#if !defined(RW_GET_INLINES_FOR_CLASS_BODY) && !defined(__SUNPRO_CC)

INFILE_TEMPL_DECL
void
PC::apply(void (*fn)(typename PC::value_type,void*),void *x) {
  my_iter_ iter = begin();
  while(iter != end()) {
    (*fn)(*iter,x);
    ++iter;
  }
}

INFILE_TEMPL_DECL
void
PC::apply(void (*fn)(typename PC::reference,void*),void *x) {
  my_iter_ iter = begin();
  while(iter != end()) {
    (*fn)(*iter,x);
    ++iter;
  }
}

INFILE_TEMPL_DECL
bool
PC::contains(bool(*fn)(typename PC::value_type,void*),void* x) const {
  const_my_iter_ iter = begin();
  while(iter != end()) {
    if((*fn)(*iter,x)) break;
    ++iter;
  }
  return iter != end();
}
#endif // ifndef RW_GET_INLINES_FOR_CLASS_BODY


#ifndef RW_GET_INLINES_FOR_CLASS_BODY
INFILE_TEMPL_DECL
typename PC::size_type PC::npos = ~(typename PC::size_type)0;

#endif // ifndef RW_GET_INLINES_FOR_CLASS_BODY

#undef INFILE_TEMPL_DECL
#undef PC
#undef RW_TEMPLATE_DECLTR__RW_PBASE
#undef RW_SCOPE_DECLTR__RW_PBASE
#undef RW_CONDITIONAL_TYPENAME 








