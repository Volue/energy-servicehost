
/*
 * RWTRegularExpressionImp definitions for RE Implementation class.
 *
 * $Id: rei.cc 18623 2000-03-03 08:28:13Z sih $
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
#   define RW_TEMPLATE_DECLTR__RW_TREGEXP
#   define RW_SCOPE_DECLTR__RW_TREGEXP
#   define RW_CONDITIONAL_TYPENAME 
#else
#   define RW_TEMPLATE_DECLTR__RW_TREGEXP template<class charT>
#   define RW_SCOPE_DECLTR__RW_TREGEXP RWTRegularExpressionImp<charT>::
#   define RW_CONDITIONAL_TYPENAME typename
#endif


#ifndef RW_GOT_INLINES_FOR_CLASS_BODY_REI_CC
RW_TEMPLATE_DECLTR__RW_TREGEXP
RW_SCOPE_DECLTR__RW_TREGEXP RWTRegularExpressionImp(const charT* reStringPtr)
        : compiled_(FALSE),
#   if defined(RW_NO_POSIX_RE)
      fastRePtr_(NULL), symbolSetVector_(4),
#   endif
    rePtr_(new regex_t),
    maxState_(~(size_t)1), getNextSymbol_(~(size_t)0), nextState_(0),
    nonacceptingState_(0), reString_(reStringPtr)
{
  compile();
}

RW_TEMPLATE_DECLTR__RW_TREGEXP
#if defined(RW_BROKEN_TEMPLATE_TYPEDEFS)
RW_SCOPE_DECLTR__RW_TREGEXP  RWTRegularExpressionImp(const RWTRegularExpressionTraits<charT>::tStringT& reString)
#else
 RW_SCOPE_DECLTR__RW_TREGEXP RWTRegularExpressionImp(const stringT& reString)
#endif

        : compiled_(FALSE),
#   if defined(RW_NO_POSIX_RE)
      fastRePtr_(NULL), symbolSetVector_(4),
#   endif
    rePtr_(new regex_t),
    maxState_(~(size_t)1), getNextSymbol_(~(size_t)0), nextState_(0),
    nonacceptingState_(0), reString_(reString)
  {
    compile();
  }



RW_TEMPLATE_DECLTR__RW_TREGEXP
RW_SCOPE_DECLTR__RW_TREGEXP RWTRegularExpressionImp
(const RWTRegularExpressionImp<charT>&   other)
        : compiled_(FALSE),
#   if defined(RW_NO_POSIX_RE)
      fastRePtr_(NULL), symbolSetVector_(4),
#   endif
    rePtr_(new regex_t),
    maxState_(~(size_t)1), getNextSymbol_(~(size_t)0), reString_(other.reString_)
{
  compile();
}

#endif /* RW_SHUT_INLINES_GUARD_OFF__REI_CC */



/*
 *  The remainder of this file is for member definitions that will never 
 *  be seen from inside a class body.
 */

#ifndef RW_GET_INLINES_FOR_CLASS_BODY

#include "rw/compiler.h"
#if !defined(RW_NO_STL) && !defined(RW_NO_EXCEPTIONS)

#include "rw/re.h"

#if !defined(RW_COMPILE_INSTANTIATE)
# include "rw/reposix1.cc"
# include "rw/reposix2.cc"
#endif

/////////////////////////////////////////////////////////////////////////////
//
// Constructors and Destructors
//
/////////////////////////////////////////////////////////////////////////////

// this guard is used for normal (i.e., "out of class body") instantiations
#ifndef __RWREI_CC__
#define __RWREI_CC__ 1



template<class charT>
RWTRegularExpressionImp<charT>::RWTRegularExpressionImp()
        : compiled_(FALSE),
#   if defined(RW_NO_POSIX_RE)
      fastRePtr_(NULL), symbolSetVector_(4),
#   endif
    rePtr_(new regex_t),
    maxState_(~(size_t)1), getNextSymbol_(~(size_t)0), nextState_(0),
    nonacceptingState_(0)
{
  clear();
}



template<class charT>
RWTRegularExpressionImp<charT>::~RWTRegularExpressionImp()
{
  delete fastRePtr_;
  fastRePtr_ = NULL;
  regfree(rePtr_);
  rePtr_ = NULL;
}

/////////////////////////////////////////////////////////////////////////////
//
// operator=
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWTRegularExpressionImp<charT>&
RWTRegularExpressionImp<charT>::operator=
  (const RWTRegularExpressionImp<charT>& other)
{
  reString_ = other.reString_;
  compile();
  return *this;
}

template<class charT> RWTRegularExpressionImp<charT>&
RWTRegularExpressionImp<charT>::operator=(const charT* reStringCharArray)
{
  reString_ = reStringCharArray;
  compile();
  return *this;
}

/////////////////////////////////////////////////////////////////////////////
//
// clear
//
//  Clears the entire nondeterministic finite automaton (NFA).
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> void
RWTRegularExpressionImp<charT>::clear()
{
  compiled_ = FALSE;
  unmatchedOpenParen_ = 0;
  rePtr_->re_nsub = 0;

# if defined(RW_NO_POSIX_RE)
  if(fastRePtr_ != NULL) {
    delete fastRePtr_;
    fastRePtr_ = NULL;
  }
        nextState_ = 0;
  nonacceptingState_ = 0;
  symbolSetVector_.clear();
  rePtr_->subexpression.clear();
  rePtr_->node.clear();
# endif
}

/////////////////////////////////////////////////////////////////////////////
//
//  compile
//
//  Compiles regular expressions using regcomp().
//  Preprocesses RE's to replace escape sequences with literal characters.
//
/////////////////////////////////////////////////////////////////////////////

#include <ctype.h>

template<class charT> void
RWTRegularExpressionImp<charT>::compile()
{
  clear();

#if defined(RW_NO_POSIX_RE)
  // If regular expression can be handled by old "fast" RE class RWCRegexp,
  //  then don't use POSIX functions.

  compiled_ = FALSE;
  RWBoolean charTrue = RWTRegularExpressionTraits<charT>::isChar();

  if (charTrue)
    // RWTRegularExpression<char>, a.k.a. RWCRExpr
  {
    const char*   cp = (const char*)reString_.data();
    const size_t  len(reString_.length());
    const char*   endCp = cp + len;              // Past end of string data.
    size_t        i = 0;                         // for() index.

    if (*cp != star() && *cp != plus() && *cp != questionMark())  // BAD_REPEAT
    {
      // Scan for symbols not in or treated differently in RWCRegexp;
      //  if found, then continue.
      for (; i < len; i++)
      {
        if    // Symbols treated differently in RWCRegexp.
        (
          *cp == openBrace()      || *cp == closeBrace()      ||
          *cp == openParen()      || *cp == closeParen()      ||
          *cp == hat()            || *cp == dollar()          ||
          *cp == bar()            || *cp == backslash()       ||
          (
            *cp == openBracket()  &&  cp < endCp              &&
            (*(cp + 1) == colon() || *(cp + 1) == dot() || *(cp + 1) == equal())
          )                                                   ||
          (*cp && !isprint(*cp))
        )
          break;

        if (*cp == dash() && cp < endCp && *(cp+1) == dash())
          break;  // BAD_CHAR_RANGE syntax error.

        if    // BAD_REPEAT syntax error.
        (
          (*cp == star() || *cp == plus() || *cp == questionMark()) &&
          cp < endCp                                                &&
          (
            *(cp + 1) == star() || *(cp + 1) == plus()              ||
            *(cp + 1) == questionMark()
          )
        )
          break;

        cp++;
      } // for
    } // if
    
    if (i >= len)     // No extended RE symbols found; use RWCRExpr.
    {
      compiled_ = TRUE;
      fastRePtr_ = new RWCRegexp((const char*)reString_.data());

      // Warning: Only call fastRePtr_->status() once because
      // RWCRegexp::status() resets status to OK.

      switch (fastRePtr_->status()) {
      case RWCRegexp::OK:
                  status_ = RWTRegularExpression<charT>::OK;
				  return;
      case RWCRegexp::TOOLONG: break;
	  default:
        status_ = RWTRegularExpression<charT>::BAD_PATTERN;
		return;
	  }

       // if error was just toolong, try with new re class
	   delete fastRePtr_;
	   fastRePtr_ = NULL;
	   compiled_ = FALSE;
    }
  }

#endif  // NO_RE_POSIX

  int error  = RW_REGCOMP(rePtr_, reString_, REG_EXTENDED);

  if (!error)
  {
    status_ = RWTRegularExpression<charT>::OK;
    compiled_ = TRUE;
  }
  else  // Convert POSIX.2 error code to RWTRegularExpression<charT> error code.
  {
    clear();

    if (error == REG_ECOLLATE)
      status_ = RWTRegularExpression<charT>::BAD_COLLATING_ELEMENT;
    else if (error == REG_ECOLLATE)
      status_ = RWTRegularExpression<charT>::BAD_COLLATING_ELEMENT;
    else if (error == REG_ECTYPE)
      status_ = RWTRegularExpression<charT>::BAD_CHAR_CLASS_TYPE;
    else if (error == REG_EESCAPE)
      status_ = RWTRegularExpression<charT>::TRAILING_BACKSLASH;
    else if (error == REG_EBRACK)
      status_ = RWTRegularExpression<charT>::UNMATCHED_BRACKET;
    else if (error == REG_EPAREN)
      status_ = RWTRegularExpression<charT>::UNMATCHED_PARENTHESIS;
    else if (error == REG_EBRACE)
      status_ = RWTRegularExpression<charT>::UNMATCHED_BRACE;
    else if (error == REG_BADBR)
      status_ = RWTRegularExpression<charT>::BAD_BRACE;
    else if (error == REG_ERANGE)
      status_ = RWTRegularExpression<charT>::BAD_CHAR_RANGE;
    else if (error == REG_ESPACE)
      status_ = RWTRegularExpression<charT>::OUT_OF_MEMORY;
    else if (error == REG_BADRPT)
      status_ = RWTRegularExpression<charT>::BAD_REPEAT;
    else if (error == RWTRegularExpression<charT>::NOT_SUPPORTED)
      status_ = RWTRegularExpression<charT>::NOT_SUPPORTED;
    else
      status_ = RWTRegularExpression<charT>::BAD_PATTERN;
  }
}

/////////////////////////////////////////////////////////////////////////////
//
// index
//
//  Returns the index of the leftmost longest instance in searchString that
//  matches the regular expression compiled in self or RW_NPOS if there is no
//  such match.
//
/////////////////////////////////////////////////////////////////////////////

  template<class charT> size_t
  RWTRegularExpressionImp<charT>::index
  (
#if defined(RW_BROKEN_TEMPLATE_TYPEDEFS)
    const RWTRegularExpressionTraits<charT>::tStringT&
                    searchString, // String to be searched.
#else
    const stringT&  searchString, // String to be searched.
#endif
    size_t*         len,          // Pointer to length of the matching string.
    size_t          start         // Start of string matched.
  )
{

#if defined(RW_NO_POSIX_RE)
  // Use "fast" RE class if allowed.
  if (fastRePtr_)
  {
    size_t      tLen;
    size_t      result(fastRePtr_->index(searchString, &tLen, start));
    
    if (len)
      *len = tLen;
    
    return result;
  }
#endif

  const charT*  searchStringPtr = searchString.data() + start;
  size_t        searchStringLength(searchString.length() - start);
  regmatch_t    match[MAX_NO_OF_SUBEXPRESSIONS];
  errorType     error;

  for (int i(0); i < MAX_NO_OF_SUBEXPRESSIONS; i++)
  {
    match[i].rm_so = (regoff_t)-1;
    match[i].rm_eo = (regoff_t)-1;
  }

  error =
    (errorType)RW_REGEXEC
      (
       rePtr_, searchStringPtr, searchStringLength,
       MAX_NO_OF_SUBEXPRESSIONS, match, 0x0
       );
  
  if (error)
  {
    if (error == REG_NOMATCH)
      status_ = RWTRegularExpression<charT>::NO_MATCH;
    //
    // dso: Cast quiets egcs 1.1.2 compiler warning about the comparison 
    //      between an errorType and a statusType.
    //
    else if (error == ( errorType ) RWTRegularExpression<charT>::NOT_SUPPORTED)
      status_ = RWTRegularExpression<charT>::NOT_SUPPORTED;
    else
      status_ = RWTRegularExpression<charT>::BAD_PATTERN;
    
    if (len)
      *len = 0;
    
    return RW_NPOS;
  }
  else if (len)
    *len = match[0].rm_eo - match[0].rm_so;
  
  return match[0].rm_so + start;
}

#endif // !defined(RW_NO_STL)

#endif // __RWREI_CC__

#endif /* RW_GET_INLINES_FOR_CLASS_BODY */

#undef RW_TEMPLATE_DECLTR__RW_TREGEXP
#undef RW_SCOPE_DECLTR__RW_TREGEXP
#undef RW_CONDITIONAL_TYPENAME 
