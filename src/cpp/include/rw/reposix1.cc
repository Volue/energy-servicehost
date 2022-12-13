/*
 * Definitions for POSIX services for the parameterized regular expression
 * class, Part 1: regcomp() and related functions.
 *
 * NOTE:  Do NOT change the signature of any of the POSIX.2 member functions
 * in this file except in response to changes in the POSIX standard for RE's.
 * If you do, then you will be unable to use POSIX RE services in the future.
 *
 * $Id: reposix1.cc 18623 2000-03-03 08:28:13Z sih $
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
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:42  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.32  1996/08/09 19:12:22  hart
 * HP (aCC compiler) port
 *
 * Revision 7.31  1996/03/14 22:55:44  pearson
 * #2828 - Add #ifdef __TURBOC__ to kill compiler warning re: unreachable code
 *
 * Revision 7.30  1996/02/19 12:34:12  jims
 * Port to Stdlib 1.2
 *
 * Revision 7.29  1996/02/18 01:45:49  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.28  1995/12/12 18:23:55  jims
 * Remove once-only inclusion guard for .cc file
 *
 * Revision 7.27  1995/10/24 00:14:30  jims
 * Port to msvc 4.0
 *
 * Revision 7.26  1995/09/22  20:14:06  kevinj
 * Clean up loose ends.
 *
 * Revision 7.25  1995/09/05  19:21:45  jims
 * Use new copyright macro
 *
 * Revision 7.24  1995/08/30  00:19:35  kevinj
 * Added RW_NO_EXCEPTIONS.
 *
 * Revision 7.23  1995/06/27  23:40:49  kevinj
 * #1876: Added RW_NO_STL to guard against futile compilation of classes
 * that depend on the existence of C++ Standard Library containers
 * and iterators.
 *
 * Revision 7.22  1995/01/26  17:39:40  kevinj
 * Bug #1038: Infinite loop on some pattern matching.
 *
 * Revision 7.21  1995/01/25  00:24:03  kevinj
 * Bug #1037: Removed all escape sequence processing. Escape sequence
 * processing will now be done either by the C++ compiler (in the case of
 * literal strings) or by the user (in the case of command line arguments).
 * This was judged to be less troublesome than the previous approach, used
 * by RWCRegexp, that required users to enter RE patterns like "\\n" in order
 * to match newlines.
 *
 * Revision 7.20  1995/01/23  23:14:45  kevinj
 * Fixed Bugs 1035, 1036.
 *
 * Revision 7.19  1995/01/23  18:47:54  kevinj
 * Bug #1034 Fixed.
 *
 * Revision 7.18  1995/01/23  18:18:59  kevinj
 * Bug #1033 fixed.
 *
 * Revision 7.17  1995/01/18  00:19:05  kevinj
 * rwRegexec speed optimization.
 *
 * Revision 7.17  1995/01/18  00:19:05  kevinj
 * rwRegexec speed optimization.
 *
 * Revision 7.16  1995/01/10  21:12:06  kevinj
 * Ported to Solaris Cafe.
 *
 * Revision 7.16  1995/01/10  21:12:06  kevinj
 * Ported to Solaris Cafe.
 *
 * Revision 7.15  1994/12/30  23:56:44  kevinj
 * Added RW_NO_POSIX_RE.  Made classes MT safe.  Commented and cleaned up code.
 * Combined NFA arrays in regex_t into one array called "node".
 * Removed RWREMatch.
 *
 * Revision 7.15  1994/12/30  23:56:44  kevinj
 * Added RW_NO_POSIX_RE.  Made classes MT safe.  Commented and cleaned up code.
 * Combined NFA arrays in regex_t into one array called "node".
 * Removed RWREMatch.
 */

#include "rw/rei.h"

#if !defined(RW_NO_STL) && !defined(RW_NO_EXCEPTIONS)

// Below enables users to use regular expression bindings
// included with compiler.
#if defined(RW_NO_POSIX_RE)

/////////////////////////////////////////////////////////////////////////////
//
// build
//
//  Builds a new node in the RE finite state machine.
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> void
RWTRegularExpressionImp<charT>::build
(
  const charT     symbol,       // Input that fires transition.
  const size_t    transition1,  // 1st transition on input.
  const size_t    transition2,  // 2nd transition on input.
  const size_t    setIndex,     // Character set.
  const RWBoolean matchAny,     // Match any character (".")
  const int       anchor        // Match anchored to front/back.
 )
{
  RWPRECONDITION(rePtr_);
  
  RWRENode<charT>
    node(symbol, transition1, transition2, setIndex, matchAny, anchor);
  rePtr_->node.insert(node);
  nextState_++;
}

/////////////////////////////////////////////////////////////////////////////
// set
//  Sets a state in the RE finite state machine.
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> void
RWTRegularExpressionImp<charT>::set
(
  const size_t     stateIndex,   // Index of state node.
  const charT      symbol,       // Input that fires transition.
  const size_t     transition1,  // First transition to next state.
  const size_t     transition2,  // 2nd transition to next state.
  const size_t     setIndex,     // Character set.
  const RWBoolean  matchAny,     // Match any character (".")
  const int        anchor        // Match anchored to front/back.
)
{
  RWPRECONDITION(rePtr_);
  RWPRECONDITION(stateIndex < maxState_);

  RWRENode<charT>
    node (symbol, transition1, transition2, setIndex, matchAny, anchor);
  rePtr_->node[stateIndex] = node;
}

/////////////////////////////////////////////////////////////////////////////
//
// regcomp
//
//  Compiles regular expression into a nondeterministic finite automaton.
//  Syntax is based on ISO/IEC 9945-2 (POSIX.2) with extensions.
//  Related functions follow this function in this section.
//
//  This function can be replaced by POSIX.2 system services.
//
//  Returns:
//    0           if OK
//    errorType   if not OK.
//
//      NFA is built with one symbol lookahead and each state transitions to a
//      state that has not yet been built.
//
//      NFA recursive-parser functions return first state of last construction or
//  zero if unsuccessful.
//
//  Extended Regular Expression (ERE) Syntax:
//
//  TOKENS:
//  bar               := '|'
//  className         := one of at least the following:
//                      "alnum", "alpha", "blank", "cntrl", "digit", "graph",
//                      "lower", "print", "punct", "space", "upper", "xdigit"
//  closeBrace        := '}'
//  closeBracket      := ']'
//  closeParen        := ')'
//  collatingElement  := single character that is not a metacharacter
//  colon             := ':'
//  comma             := ','
//  dash              := '-'
//  dollar            := '$'
//  dot               := '.'
//  epsilon           := no symbol needed for transformation.
//  equal             := '='
//  hat               := '^'
//  metacharacter     := ^, -, ] (See ISO/IEC 9945-2 Section 2.8.5.1)
//  openBrace         := '{'
//  openBracket       := '['
//  openParen         := '('
//  ordinaryChar      := not a specialChar
//  plus              := '+'
//  questionMark      := '?'
//  quotedChar        := one of: \^ \. \[ \] \$ \( \) \| \* \+ \? \{ \} \\  
//  repeatCount       := 0 <= repeatCount <= RE_DUP_MAX
//  specialChar       := one of: ^ . [ $ ( ) | * + ? { \  
//                      outside of bracketExpression.
//  star              := '*'
//
//  GRAMMAR:
//  bracketExpression := openBracket bracketList closeBracket
//                      | openBracket hat bracketList closeBracket
//  bracketList       := followList | followList dash
//  characterClass    := colon className colon
//  collatingSymbol   := dot collatingElement dot | dot metacharacter dot
//  endRange          := collatingElement | collatingSymbol
//  equivalenceClass  := equal collatingElement equal
//
//  ere               := ereBranch ereRest
//  ereRest           := bar ereBranch ereRest | epsilon
//              Above two rules eliminate left-recursion in:
//  ere               := ereBranch | ere bar ereBranch
//
//  ereBranch         := ereExpression ereBranchRest
//  ereBranchRest     := ereExpression ereBranchRest | epsilon
//              Above two rules eliminate left-recursion in:
//  ereBranch         := ereExpression | ereBranch ereExpression
//
//  ereExpression     := ereExpressionToken ereExpressionRest
//  ereExpressionRest := ereRepeat ereExpressionRest
//                      | ereExpressionToken ereExpressionRest | epsilon
//      ereExpressionToken:= oneCharEre | hat | dollar | openParen ere closeParen
//              Above three rules eliminate left-recursion in:
//              ereExpression     := oneCharEre | hat | dollar
//                                                                                                      | openParen ere closeParen
//                                      | ereExpression ereRepeat
//
//  ereRepeat         := star | plus | questionMark
//                      | openBrace repeatCount closeBrace
//                      | openBrace repeatCount comma closeBrace
//                      | openBrace repeatCount comma repeatCout closeBrace
//
//  expressionTerm    := rangeExpression | singleExpression
//    Above rule eliminates RE compile errors on rangeExpressions because
//    singleExpression matched first in:
//      expressionTerm    := singleExpression | rangeExpression
//
//      followList                              := expressionTerm followListRest
//      followListRest          := expressionTerm followListRest | epsilon
//              Above two rules eliminate left-recursion in:
//              followList        := expressionTerm | followList expressionTerm
//
//  oneCharEre        := ordinaryChar | quotedChar | dot | bracketExpression
//  rangeExpression   := startRange endRange | startRange dash
//  singleExpression  := endRange | characterClass | equivalenceClass
//  startRange        := endrange dash
/////////////////////////////////////////////////////////////////////////////

template<class charT> int
RWTRegularExpressionImp<charT>::rwRegcomp
        (
         const stringT& pattern       // RE to be compiled.
        )
{
  RWPRECONDITION(pattern.length()!=0);
  token_ = pattern;
  
  try
  {
    token_++;
    
    if (token_.isEmpty())
      throw RWRECompileError(REG_BADPAT);


    // Build initial state.
    size_t              firstState(0);
    addSubexpression(firstState, 0);
                build(nullCharT(), firstState + 1, firstState + 1);

    // Start compiling regular expression.
    nonacceptingState_ = 0;
    ere(RWRERange(next1(firstState)));

    // here all tokens should have been eaten
    if (!token_.isEmpty()) {
      throw RWRECompileError(REG_BADPAT);
    }

    // Build a state that pretends that it's a transition on an input symbol
    // that isn't in the input alphabet.  This is part of how you get the
    // longest leftmost substring in a nondeterministic finite automaton.
    // See rwRegexec() and ereRepeat() for more information.
    nonacceptingState_ = nextState_;
    build(nullCharT(), nextState_ + 1, nextState_ + 1);

    // Build final (accepting) state.
    subexpression(0, firstState, nextState_);
                build(nullCharT());
  }
  
  catch (RWRECompileError error)
  {return error.no;}
  
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
//                  REGULAR EXPRESSION GRAMMAR
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//
//  bracketExpression := openBracket bracketList closeBracket
//                      | openBracket hat bracketList closeBracket
//  (aka character set)
/////////////////////////////////////////////////////////////////////////////


template<class charT> size_t
RWTRegularExpressionImp<charT>::bracketExpression()
{
  size_t        result(0);

  if (token_() == openBracket())
  {
    token_++;

    if (token_.isEmpty())
      throw RWRECompileError(REG_BADPAT);
    
    if(!symbolSetString_.isNull())
      symbolSetString_.resize(0);
    
    if(token_() == hat())
    {
      symbolSetString_.append(hat());
      token_++;
    }
    
    bracketListStartPos_ = token_.position();  
        // used in CollatingElement(). ] is not a meta char if found first

    if (!bracketList())
      throw RWRECompileError(REG_BADPAT);
    
    if (token_() == closeBracket())
    {
      result = nextState_;
      build(nullCharT(), nextState_ + 1, nextState_ + 1,addSymbolSet());
      // Must be nullCharT input for rwRegexec.
      token_++;

      if(!symbolSetString_.isNull())
        symbolSetString_.resize(0);
      
      return result;
    }
    
    throw RWRECompileError(REG_EBRACK);
  } // if (token_() == openParen)
  
  return result;
}

/////////////////////////////////////////////////////////////////////////////
//
//  bracketList       := followList | followList dash
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWBoolean
RWTRegularExpressionImp<charT>::bracketList()
{
  if (followList())
  {
    if (token_() == dash())
    {
      symbolSetString_.append(dash());
      token_++;
    }

    return TRUE;
  }
  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
//  characterClass    := colon className colon
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWBoolean
RWTRegularExpressionImp<charT>::characterClass()
{
  if (token_() == colon())
  {
    while
    (
      token_() != colon() && token_() != closeBracket() &&
      !token_.isEmpty()
    )
      token_++;  // Character class not supported so consume tokens.

    if (token_() == colon())
      throw RWRECompileError(RWTRegularExpression<charT>::NOT_SUPPORTED);

    else if (token_() == closeBracket())
      return FALSE;   // Not a character class.

    throw RWRECompileError(REG_BADPAT);
  }
  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
//  collatingElement  := single character that is not a metacharacter
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWBoolean
RWTRegularExpressionImp<charT>::collatingElement()
{
  // ^ is a meta char when found first in a bracket expression
  // ] is a meta char when found anywhere but first(after initail ^ if any)
  //    in a bracket expression .  see ISO/IEC 9945-2: 1993(E)
  
  // The initial ^ has been processed in bracketExpression, so all ^'s found
  // here are normal chars.

  if (token_.isEmpty()) return FALSE;

  if (token_.position() == bracketListStartPos_ || 
      token_() != closeBracket()  )
  {
    // Add character to symbol set to be matched.
    symbolSetString_.append(token_());

    token_++;

    return TRUE;
  }
  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
//  collatingSymbol   := dot collatingElement dot | dot metacharacter dot
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWBoolean
RWTRegularExpressionImp<charT>::collatingSymbol ()
{
  if (token_() == dot())
  {
    while (token_() != dot() && !token_.isEmpty())
      token_++;                 // Not supported so consume tokens.
    
    if (token_() == dot())
      throw RWRECompileError(RWTRegularExpression<charT>::NOT_SUPPORTED);
  }
  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
//  endRange          := collatingElement | collatingSymbol
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWBoolean
RWTRegularExpressionImp<charT>::endRange()
{
  if (collatingElement())
    return TRUE;

  if (collatingSymbol())
  {
    // Special handling will be put here sometime in future.
    // A collating symbol does NOT set the symbol set
    // directly but refers to the locale to see if and as what the collating
    // is defined.  For now, collating symbols are not supported.
    return TRUE;
  }
  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
//  equivalenceClass  := equal collatingElement equal
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWBoolean
RWTRegularExpressionImp<charT>::equivalenceClass()
{
  if (token_() == equal())
  {
    while
    (
      token_() != equal() && token_() != closeBracket() &&
      !token_.isEmpty()
    )
      token_++;     // Not supported so consume tokens.

    if (token_() == equal())
      throw RWRECompileError(RWTRegularExpression<charT>::NOT_SUPPORTED);

    else if (token_() == closeBracket())
      return FALSE;

    throw RWRECompileError(REG_BADPAT);
  }
  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
//      ere     := ereBranch ereRest
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWRERange
RWTRegularExpressionImp<charT>::ere(const RWRERange &range)
{
  if (token_() == bar())
    throw(RWRECompileError((int)REG_BADPAT));
  
  RWRERange  state2(ereBranch(range));          // See ereRest() for details.
  if (state2.isValid())
    return ereRest(state2);
  return state2;
}

/////////////////////////////////////////////////////////////////////////////
//
//      ereBranch       := ereExpression ereBranchRest
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWRERange
RWTRegularExpressionImp<charT>::ereBranch(const RWRERange &range)
{
  RWRERange        result;
  RWRERange        result1(ereExpression(range));
  RWRERange        result2(ereBranchRest());
  
  result.start_ = result1.start_;
  if (result2.isValid()) 
    result.end_ = result2.end_;
  else
    result.end_ = result1.end_;

  return result;
}

/////////////////////////////////////////////////////////////////////////////
//
//      ereBranchRest           := ereExpression ereBranchRest | epsilon
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWRERange
RWTRegularExpressionImp<charT>::ereBranchRest()
{
  RWRERange     result;
  RWRERange     r(ereExpression(result));
  
  while (r.isValid())     // Use a while() to eliminate tail-recursion.
  {
    result = r;
    r = ereExpression(result);
  }
  return result;
}

/////////////////////////////////////////////////////////////////////////////
//
//      ereExpression           := ereExpressionToken ereExpressionRest
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWRERange
RWTRegularExpressionImp<charT>::ereExpression(const RWRERange &range)
{
  RWRERange             result(range);

  if (token_.isEmpty())
    return result;
  
  result = ereExpressionToken(result);
  
  if (!result.isValid())
    return result;
  
  result = ereExpressionRest(result);
  return result;
}

/////////////////////////////////////////////////////////////////////////////
//
//  ereExpressionRest := ereRepeat ereExpressionRest
//                      | ereExpressionToken ereExpressionRest | epsilon
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWRERange
RWTRegularExpressionImp<charT>::ereExpressionRest(const RWRERange &range)
{
  RWRERange     result(range);
  RWRERange     r;

  do                  // do-while eliminates tail-recursion.
  {
    r = ereRepeat(result);
    if (!r.isValid())
    {
      r = ereExpressionToken(result);
      
      if(r.isValid())
        result = r;
    }
    else
      result = r;
    
    result = (r.isValid() ? r : result);
  } while (r.isValid() && !token_.isEmpty());
  
  return result;
}

/////////////////////////////////////////////////////////////////////////////
//
// ereExpressionToken:= oneCharEre | hat | dollar | openParen ere closeParen
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWRERange
RWTRegularExpressionImp<charT>::ereExpressionToken(const RWRERange &range)
{
  RWRERange             result(range.start_);

  if
    (
      token_() == star() || token_() == plus() ||
      token_() == questionMark() || token_() == openBrace()
    )
      throw RWRECompileError(REG_BADRPT); // Repeat metacharacter in wrong place.

  size_t  endResult = oneCharEre();

  if (endResult)
  {
    result.end_ = endResult;
    return result;
  }

  if (token_() == hat())            // Anchor match to front of string or line.
  {
    result.end_ = nextState_;
    build(nullCharT(), nextState_ + 1, nextState_ + 1,0, FALSE, anchorFront);
    token_++;
    
    if
      (
       token_() == star() || token_() == plus() ||
       token_() == questionMark() || token_() == openBrace()
       )
        throw RWRECompileError(REG_BADRPT);
    
    return result;
  }
  
  if (token_() == dollar())       // Anchor match to end of string or line.
  {
    result.end_ = nextState_;
    build(nullCharT(), nextState_ + 1, nextState_ + 1,0, FALSE, anchorBack);
    token_++;
    return result;
  }
  
  if (token_() == openParen())    // Parenthesized subexpression: (ere).
  {
    size_t            startState = nextState_;
    build(nullCharT(), startState + 1, startState + 1);
    token_++;
    
    unmatchedOpenParen_++;   // number of unmatched open parentheses

    if
      (
       token_() == star() || token_() == plus() ||
       token_() == questionMark() || token_() == openBrace()
       )
        throw RWRECompileError(REG_BADRPT);
    
    if (token_() == closeParen())
      throw RWRECompileError(REG_BADPAT);
    
    RWRERange result1(startState, startState); 
    result1 = ere(result1);
    
    if (token_() == closeParen())
    {
      size_t            endState(nextState_);
      build(nullCharT(), endState + 1, endState + 1);
      addSubexpression(startState, endState);
      token_++;

      unmatchedOpenParen_--;

      result.end_ = endState;
      return result;
    }
    
    throw RWRECompileError(REG_EPAREN);
  } // if (token_() == openParen)
  
  return result;
}

/////////////////////////////////////////////////////////////////////////////
//
//  ereRepeat         := star | plus | questionMark
//                      | openBrace repeatCount closeBrace
//                      | openBrace repeatCount comma closeBrace
//                      | openBrace repeatCount comma repeatCout closeBrace
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWRERange
RWTRegularExpressionImp<charT>::ereRepeat(const RWRERange &range)
{
  RWRERange result(range);

  if (token_() == plus())  {
    // "a+" == "aa*".  (Sub)Expression is repeated 1 or more times.
    // Parenthesized subexpressions are treated as one unit.
    size_t            lastSub(lastSubexpression());
    RWRESubexpression sub(getSubexpression(lastSub));
    RWRERange         previousState(nextState_ - 1, nextState_ - 1);
    size_t            plusStateStart(previousState.start_);
    RWRENode<charT>   node;


    if (lastSub && previousState.start_ == sub.end_)    {
      // If RWRESubexpression immediately before repeat,
      // then clone entire subexpression as "a".
      size_t          offset(sub.end_ - sub.start_ + 1);
      plusStateStart = sub.start_;

      for (size_t i(sub.start_); i <= sub.end_; i++)
      {
        node = getNode(i);
        build
          (
           node.input_, node.next1_ + offset, node.next2_ + offset,
           node.symbolSetIndex_, node.matchAny_, node.anchor_
           );
      }

      previousState.start_ = sub.end_ + 1;
      previousState.end_ = nextState_ - 1;
    }
    else
    {
      //Clone one character ERE "a".
      node = getNode(previousState.start_);
      build
        (
         node.input_, nextState_ + 1, nextState_ + 1,
         node.symbolSetIndex_, node.matchAny_, node.anchor_
         );
      previousState.start_ = previousState.end_ = nextState_ - 1;
    }
    
    makeStar(previousState);  // "a*"
    
    if (lastSub)
    {
      // Update subexpression so that last state points to _this_ last state.
      sub.end_ = nextState_ - 1;
      subexpression(lastSub, sub);
    }
    
    if          // Duplicate repeat metacharacter.
      (
       token_() == plus() || token_() == star() || token_() == questionMark() ||
       token_() == openBrace() || token_() == closeBrace()
       )
        throw RWRECompileError(REG_BADRPT);
    
    // A nonaccepting state is appended to an RE that has a repeat in it.
    nonacceptingState_ = 1;
    
    return plusStateStart;
  }
  
  if (token_() == star())
  { // (Sub)Expression is repeated 0 or more times.
    
    // Parenthesized subexpressions are treated as one unit.
    size_t            lastSub(lastSubexpression());
    RWRESubexpression sub(getSubexpression(lastSub));
    RWRERange         previousState(nextState_ - 1, nextState_ - 1);
    
    if (lastSub && previousState.start_ == sub.end_)
    {
      // Entire parenthesized subexpression is to be repeated.
      previousState.start_ = sub.start_;
      previousState.end_   = sub.end_;
    }
    
    makeStar(previousState);              // "a*"
    
    if (lastSub)
    {
      // Update subexpression so that last state points to _this_ last state.
      sub.end_ = nextState_ - 1;
      subexpression(lastSub, sub);
    }
    
    if         // Duplicate repeat metacharacter
      (
       token_() == plus() || token_() == star() || token_() == questionMark() ||
       token_() == openBrace() || token_() == closeBrace()
       )
        throw RWRECompileError(REG_BADRPT);
    
    // A nonaccepting state is appended to an RE that has a repeat in it.
    nonacceptingState_ = 1;
    
    result.end_ = nextState_ - 1;
    return result;
    //return previousState;
  }
  
  if (token_() == questionMark())
  {
    // Construction for a?:
    //           a
    // 0-->1-->2-->3
    //     |       |
    //     +-->-->-+
    //         e
    // (Sub)Expression is repeated 0 or 1 times.

    // Parenthesized subexpressions are treated as one unit.
    size_t            lastSub(lastSubexpression());
    RWRESubexpression sub(getSubexpression(lastSub));
    RWRERange         state2(nextState_ - 1, nextState_ - 1);
    
    if (lastSub && state2.start_ == sub.end_)
    {
      // Entire subexpression is to be repeated.
      state2.start_  = sub.start_;
      state2.end_    = sub.end_;
    }
    
    // Make a new state 2.
    // At this point we have state 2 and "a".
    
    size_t                newState2(nextState_);
    RWRENode<charT>   node(getNode(state2.start_));
    
    if (state2.start_ == state2.end_)   // One char ERE is repeated.
      build
        (
         node.input_, newState2 + 1, newState2 + 1,
         node.symbolSetIndex_, node.matchAny_, node.anchor_
         );
    else                              // Subexpression is repeated.
      build
        (
         node.input_, node.next1_, node.next2_,
         node.symbolSetIndex_, node.matchAny_, node.anchor_
         );

    size_t  state3(nextState_);
    build(nullCharT(), state3 + 1, state3 + 1);
    
    // Move old state 2 (or old state 2 first state) to state 1.
    set(state2.start_, nullCharT(), newState2, state3);
    
    if (state2.start_ != state2.end_)
      set(state2.end_, nullCharT(), state3, state3);

    token_++;

    if (lastSub)
    {
      // Update subexpression so that last state points to _this_ last state.
      sub.end_ = state3;
      subexpression(lastSub, sub);
    }

    if    // Duplicate repeat metacharacter(s).
      (
       token_() == plus() || token_() == star() || token_() == questionMark() ||
       token_() == openBrace() || token_() == closeBrace()
       )
        throw RWRECompileError(REG_BADRPT);

    return state2.start_;     // State 1.
  }

  // Braces not supported.
  if (token_() == openBrace())
  {
    while (token_() != closeBrace() && !token_.isEmpty())
      token_++;             // Not supported so consume tokens.

    if (token_() == closeBrace())
      throw RWRECompileError(RWTRegularExpression<charT>::NOT_SUPPORTED);
    else
      throw RWRECompileError(REG_BADBR);
  }

  return RWRERange();
}

/////////////////////////////////////////////////////////////////////////////
//
// makeStar
//
//  Makes "a*" subexpression in RE finite state machine.
//  Called by ereRepeat().
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWRERange
RWTRegularExpressionImp<charT>::makeStar
(
  const RWRERange    &state2
)
{
  // Thompson construction for a* (e == no input symbol):
  //         e
  //       +-<-+
  //       | a |
  //    0-->1-->2-->3-->4
  //    |           |
  //    +--->--->---+
  //                e
  // (Sub)Expression repeats 0 or more times.

  // Make a new state 2.
  // At this point we have state 2 and "a".

  size_t        newState2(nextState_);
  RWRENode<charT> node(getNode(state2.start_));

  if (state2.start_ == state2.end_)          // One character ERE.
    build
      (
       node.input_, nextState_ + 1, nextState_ + 1,
       node.symbolSetIndex_, node.matchAny_, node.anchor_
       );
  else                                      // Parenthesized subexpression.
        build
          (
           node.input_, node.next1_, node.next2_,
           node.symbolSetIndex_, node.matchAny_, node.anchor_
          );
  
  node = getNode(state2.end_);
  set
    (
     state2.end_, node.input_, newState2, nextState_ + 1,
     node.symbolSetIndex_, node.matchAny_, node.anchor_
    );
  build(nullCharT(), newState2, nextState_ + 1);      // State 3

  size_t                state4(nextState_);
  build(nullCharT(), state4 + 2, state4 + 2);         // State 4
  build(nullCharT(), newState2, state4);              // state 1
  
  // Make old state 2 into state 0.
  set(state2.start_, nullCharT(), nextState_ - 1, nextState_ - 1);
  
  token_++;
  return state2;      // State 0.
}

/////////////////////////////////////////////////////////////////////////////
//
//      ereRest := bar ereBranch ereRest | epsilon
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWRERange
RWTRegularExpressionImp<charT>::ereRest( const RWRERange &state)
     // state 2 in Thompson construction for a|b.
{

  RWRERange result(state.start_);
  RWRERange state2(state);

  while (token_() == bar())
  {
    // Thompson construction for a | b (e == no input symbol):
    //  e   a   e
    //  +-->2-->3--+
    //  |          |
    //   -->1      +-->6
    //  |          |
    //  +-->4-->5--+
    //  e   b   e

    // At this point we only have state 2 and "a|".
    // Concatenation has a higher precedence than alternation.

    size_t        state2Start(state2.start_);

    build(nullCharT(), nextState_ + 1, nextState_ + 1); // State 3
    RWRERange state6(nextState_, nextState_);
    build(nullCharT(), 0, 0);                           // State 6 start.
    token_++;
    
    if
      (
       token_() == star() || token_() == plus() ||
       token_() == questionMark() || token_() == openBrace()
       )
        throw RWRECompileError(REG_BADRPT);
    
    // Get "b".
    RWRERange  state4(nextState_);
    state4 = ereBranch(state4);
    
    if (!state4.isValid())
    {
      // Look at previous token and see if can get a different token.
      token_--;
      state4.start_ = nextState_;
      state4.end_ = nextState_;
      state4 = ereExpressionToken(state4);

      if (!state4.isValid())
        throw RWRECompileError(REG_BADPAT);
      
      // Get "b*" if any.
      if (!token_.isEmpty())
        ereRepeat(result);
    }

    build(nullCharT(), state6.start_, state6.start_);     // State 5.
    
    // Insert new state 2 and make old state 2 into state 1 so don't have
    // to update pointers to state 1.
    size_t              newState2(nextState_);
    RWRENode<charT> node(getNode(state2Start));
    build
      (
      node.input_, node.next1_, node.next2_,
       node.symbolSetIndex_, node.matchAny_, node.anchor_
       );
    set(state2Start, nullCharT(), newState2, state4.start_);
    // Old state 2 now state 1.

    set(state6.end_, nullCharT(), nextState_, nextState_);

    // set new state2
    state2.end_ = nextState_ - 1; // state2.begin_ not changed
  }

  result.end_ = nextState_ - 1;
  return result;
}

/////////////////////////////////////////////////////////////////////////////
//
//  expressionTerm    := rangeExpression | singleExpression
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWBoolean
RWTRegularExpressionImp<charT>::expressionTerm()
{
  // Prepare for backtracking.
  size_t  oldTokenPosition(token_.position());
  size_t  oldSymbolSetStringPosition(symbolSetString_.length());
  
  if (rangeExpression())
    return TRUE;

  // Backtrack
  token_.position(oldTokenPosition);
  symbolSetString_.remove(oldSymbolSetStringPosition);

  if (singleExpression())
    return TRUE;
  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
//      followList                      := expressionTerm followListRest
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWBoolean
RWTRegularExpressionImp<charT>::followList()
{
  if(expressionTerm())
  {
    followListRest();
    return TRUE;
  }
  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
//      followListRest          := expressionTerm followListRest | epsilon
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> void
RWTRegularExpressionImp<charT>::followListRest()
{
  if (token_.isEmpty())
    return;
  while (expressionTerm())
    ;
}

/////////////////////////////////////////////////////////////////////////////
//
//  oneCharEre        := ordinaryChar | quotedChar | dot | bracketExpression
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> size_t
RWTRegularExpressionImp<charT>::oneCharEre()
{
  size_t        result;
  
  if ((result = ordinaryChar()) == 0) {  /* regular char? */
    if (token_() == dot())               /* no. dot?      */
    {
      result = nextState_;
      build(nullCharT(), nextState_ + 1, nextState_ + 1, 0, TRUE);
      token_++;
    } else if((result = quotedChar()) == 0) { /* no. Quoted? */
      result = bracketExpression();           /* no. Only bracket left */
    }
  }
  return result;
}


/////////////////////////////////////////////////////////////////////////////
//
//  ordinaryChar      := not a specialChar
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> size_t
RWTRegularExpressionImp<charT>::ordinaryChar()
{
  size_t result = 0;
  if(token_() != hat() && token_() != dot() && 
     token_() != openBracket() &&  
     token_() != openBrace() && 
     token_() != openParen() && 
     (token_() != closeParen() || unmatchedOpenParen_ == 0) &&
     token_() != dollar() && token_() != bar() && token_() != star() &&
     token_() != plus() && token_() != questionMark() &&
     token_() != backslash() && !token_.isEmpty()
     )
  {
    result = nextState_;
    build(token_(), nextState_ + 1, nextState_ + 1);
    token_++;
  }
  return result;
}


/////////////////////////////////////////////////////////////////////////////
//
//  quotedChar        := one of: \^ \. \[  \$ \( \) \| \* \+ \? \{  \\
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> size_t
RWTRegularExpressionImp<charT>::quotedChar()
{
  size_t result = 0;
  
  if (token_()== backslash())
  {
    token_++;
    if (token_.isEmpty())
      throw(RWRECompileError((int)REG_EESCAPE));
    if(token_() == hat()         || token_() == dot()          ||
       token_() == openBracket() || token_() == dollar()       ||
       token_() == openParen()   || token_() == closeParen()   ||
       token_() == bar()         || token_() == star()         ||
       token_() == plus()        || token_() == questionMark() ||
       token_() == openBrace()   || token_() == backslash() 
       )
    {
      result = nextState_;
      build(token_(), nextState_ + 1, nextState_ + 1);
      token_++;
      return result;
    }
    else {
      throw(RWRECompileError((int)REG_BADPAT));
    }
  }
  return result;
}

/////////////////////////////////////////////////////////////////////////////
//
//  rangeExpression   := startRange endRange | startRange dash
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWBoolean
RWTRegularExpressionImp<charT>::rangeExpression()
{
  if (startRange())
  {
    if (endRange())   // Test to make sure that startRange < endRange.
    {                 // symbolSetString_ holds string representation of range.
      size_t  lastIndex(symbolSetString_.length() - 1);
      
      if ((unsigned char)symbolSetString_[lastIndex] < 
	       (unsigned char)symbolSetString_[lastIndex - 2])

        throw RWRECompileError(REG_ERANGE);
      
      return TRUE;
    }
    
    else if(token_() == dash())
    {
      symbolSetString_.append(token_());
      return TRUE;
    }
  }
  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
//  singleExpression  := endRange | characterClass | equivalenceClass
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWBoolean
RWTRegularExpressionImp<charT>::singleExpression()
{
  if (endRange())
    return TRUE;
  
  if (characterClass())
  {
    // Special handling will be put here.
    // A collating symbol does NOT set the symbol set
    // directly but refers to the locale to see if and as what the character
    // class is defined.  For now, this feature is not supported.
    return TRUE;
  }

  if (equivalenceClass())
  {
    // Special handling will be put here.
    // A collating symbol does NOT set the symbol set
    // directly but refers to the locale to see if and as what the equivalence
    // class is defined.  For now, this feature is not supported.

    return TRUE;
  }
  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
//  specialChar       := one of: ^ . [ $ ( ) | * + ? { \    
//                      outside of bracketExpression.
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> size_t
RWTRegularExpressionImp<charT>::specialChar()
{
  size_t        result(0);
  if
    (
     (token_() == hat() || token_() == dot() || token_() == openBracket()) ||
     (token_() == dollar() || token_() == openParen()) ||
     (token_() == closeParen() || token_() == bar() || token_() == star()) ||
     (token_() == plus() || token_() == questionMark()) ||
     (token_() == openBrace() || token_() == backslash())
    )
    {
      result = nextState_;
      build(token_(), nextState_ + 1, nextState_ + 1);
      token_++;
      return result;
    }
  
  return result;
}

/////////////////////////////////////////////////////////////////////////////
//
//  startRange        := endRange dash
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> RWBoolean
RWTRegularExpressionImp<charT>::startRange()
{
  if (endRange() && token_() == dash())
  {
    symbolSetString_.append(dash());
    token_++;
    return TRUE;
  }

  return FALSE;
}
#endif  // defined(RW_NO_POSIX_RE)
#endif  // !defined(RW_NO_STL)
