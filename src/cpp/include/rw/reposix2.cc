/*
 * Definitions for POSIX services for the parameterized regular expression
 * class, Part 2: regexec() and regfree().
 *
 * NOTE:  Do not change the signature of any of the member functions in this
 * file except in response to changes in the POSIX standard for RE's.  If you
 * do, then you will be unable to use POSIX RE services in the future.
 *
 * $Id: reposix2.cc 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.42  1996/02/19 12:34:12  jims
 * Port to Stdlib 1.2
 *
 * Revision 7.41  1996/02/18 01:45:52  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.40  1996/02/16 00:02:33  jims
 * Now provides RWREState::operator<() for all compilations
 *
 * Revision 7.39  1996/01/30 22:23:56  kevinj
 * Bug #2154: Made regfree() inline.
 *
 * Revision 7.38  1995/12/15 06:58:21  jims
 * Remove use of RW_DONT_PERIST_MACRO
 *
 * Revision 7.37  1995/12/13 00:27:23  pearson
 * Add RW_DONT_PERSIST invocations for RWTBitVec, RWRESubexpression,
 * and RWRENode
 *
 * Revision 7.36  1995/12/12 18:25:54  jims
 * Remove once-only inclusion guard for .cc file
 *
 * Revision 7.35  1995/12/12 01:10:09  pearson
 * Changed RWDequeVal with RWTValDeque
 *
 * Revision 7.34  1995/12/12 00:08:08  pearson
 * Change evdeque.h to tvdeque.h
 *
 * Revision 7.33  1995/11/16 16:01:29  jims
 * Switch use of DONT_PERSIST macro from rec.cpp to reposix2.cc for xlC
 *
 * Revision 7.32  1995/11/15 06:36:09  jims
 * Suppress use of RW_DONT_PERSIST macro for xlC
 *
 * Revision 7.31  1995/10/31 00:28:03  jims
 * include compiler.h before testing for RW_NO_STL
 *
 * Revision 7.30  1995/10/27  16:06:39  jims
 * Port to cset
 *
 * Revision 7.29  1995/10/24  00:15:48  jims
 * Move RW_DONT_PERSIST(RWREState) to rec.cpp
 * add operator<() to RWREState to appease overachievers
 *
 * Revision 7.28  1995/09/22  20:14:06  kevinj
 * Clean up loose ends.
 *
 * Revision 7.27  1995/09/22  16:22:08  jims
 * Comment out code causing 'value never used' warnings
 *
 * Revision 7.26  1995/09/20  18:55:59  kevinj
 * Removed inadvertently introduced tab characters.
 *
 * Revision 7.25  1995/09/19  18:23:24  kevinj
 * Bug #2051.
 *
 * Revision 7.24  1995/09/05  19:22:04  jims
 * Use new copyright macro
 *
 * Revision 7.23  1995/08/31  17:42:16  kevinj
 * #1186: Replaced RWTDeque with RWDequeVal.
 *
 * Revision 7.22  1995/08/30  00:19:35  kevinj
 * Added RW_NO_EXCEPTIONS.
 *
 * Revision 7.21  1995/06/27  23:40:49  kevinj
 * #1876: Added RW_NO_STL to guard against futile compilation of classes
 * that depend on the existence of C++ Standard Library containers
 * and iterators.
 *
 * Revision 7.20  1995/01/26  17:39:40  kevinj
 * Bug #1038: Infinite loop on some pattern matching.
 *
 * Revision 7.19  1995/01/21  01:20:02  kevinj
 * Completed port to Solaris Cafe 3.0.
 *
 * Revision 7.19  1995/01/21  01:20:02  kevinj
 * Completed port to Solaris Cafe 3.0.
 *
 * Revision 7.18  1995/01/18  00:19:05  kevinj
 * rwRegexec speed optimization.
 *
 * Revision 7.17  1995/01/10  21:12:06  kevinj
 * Ported to Solaris Cafe.
 *
 * Revision 7.17  1995/01/10  21:12:06  kevinj
 * Ported to Solaris Cafe.
 *
 * Revision 7.16  1994/12/30  23:56:44  kevinj
 * Added RW_NO_POSIX_RE.  Made classes MT safe.  Commented and cleaned up code.
 * Combined NFA arrays in regex_t into one array called "node".
 * Removed RWREMatch.
 *
 * Revision 7.16  1994/12/30  23:56:44  kevinj
 * Added RW_NO_POSIX_RE.  Made classes MT safe.  Commented and cleaned up code.
 * Combined NFA arrays in regex_t into one array called "node".
 * Removed RWREMatch.
 */


#include "rw/compiler.h"
#if !defined(RW_NO_STL) && !defined(RW_NO_EXCEPTIONS)

#include "rw/rei.h"

// Below enables users to use regular expression bindings
// included with compiler.
#if defined(RW_NO_POSIX_RE)

/////////////////////////////////////////////////////////////////////////////
// rwRegexec
//
//  Finds matches in a string with a compiled regular expression.
//
//  Returns:
//    0             if OK
//    REG_NOMATCH   if not OK.
//
//  This function can be replaced by POSIX.2 system service call regexec().
//  See top of rw/rei.h for details.
//
//  Unlike the POSIX.2 regexec(), rwRegexec processes wchar_t strings
//  and embedded NULLs.
//
//  Matching function uses a deque to store states in regular expresssion
//  nondeterministic finite automaton (NFA).  Each state stored on the deque is
//  composed of its index of the state node in the NFA together with the
//  index of the next input symbol that will fire the transition in the state
//  to the next state.
//
//  States with epsilon transitions are pushed onto front of deque and states
//  with input transitions pushed onto the back of the deque.  The next state
//  to process is always popped off the front of the deque.
//
//  Epsilon states are processed on the present symbol; input states are
//  processed on the next symbol.  Epsilon and input states are separated
//  by the scan state which triggers a read of the next input symbol.
//
//  PRECONDITIONS:
//    1.  There is one and only one input transition out of any state.
//    2.  _OR_ there are either one or two epsilon transitions out of a state.
//    3.  States with two transitions aren't adjacent to each other.
//
/////////////////////////////////////////////////////////////////////////////
/*
 * class RWREState:  
 *   Used to store states of NFA on deque. This class
 *   declaration is external because some compilers
 *   don't allow a local class to be used as a
 *   template argument.
 */

  class RWREState {
  public:
    size_t          number;       // Number of NFA node (stateIndex).
    size_t          nextToMatch;  // Position of next symbol to be matched.

    RWREState (size_t inNumber = 0, size_t inNextToMatch = 0)
      : number(inNumber), nextToMatch(inNextToMatch) { }
    RWBoolean operator==(const RWREState& other)const
      {return(number == other.number);}
    RWBoolean operator!=(const RWREState& other) const
      {return(!operator==(other));}
    // some unused methods in stdlib need less-than:
    RWBoolean operator<(const RWREState& other) const
      { return number < other.number; }                 
    ~RWREState() { }
  };

#include "rw/tvdeque.h"
#include "rw/edefs.h"
#include "rw/bitvec.h"

inline void __destroy(RWREState**) { }  // needed for Borland 4.5 

template<class charT> int
RWTRegularExpressionImp<charT>::rwRegexec
   (
    const charT*  searchStringPtr,    // String to be searched.
    size_t        searchStringLength, // Length of string to be searched,
                                      //  including embedded nulls.
     regmatch_t    pmatch[]            // Array of matched strings.
   )
{

  if (!compiled_)
    return REG_NOMATCH; // regexec() can only return 0 or REG_NOMATCH (POSIX.2)

  const RWREState   scanState(getNextSymbol_, 0); // Get new input.
  const size_t      firstStateIndex(next1(0));
  const RWRENode<charT>
                    firstNode(getNode(firstStateIndex));
  size_t            stateIndex(firstStateIndex);  // First state is state 1.
  RWRENode<charT>   node(getNode(stateIndex));    // State machine node.
  size_t            stringIndex(0);               // Position in search string.
  RWREState         theState(stateIndex, 0);      // Set to state 1.

   RWTValDeque<RWREState> state;  // Deque of states to check.  This needs to
                               // be replaced with new templatized deque.
                               // Replacement deque should be set to initial
                               // size and resize increment of 8 or 16 entries.
                               // Deque popping and pushing are the most
                               // time-critical parts of this algorithm.

   RWBitVec          stateUsed(nextState_);  // Prevents one state from being
                                            // added twice to deque; prevents
                                            // infinite loops in NFA from
                                            // being executed.
  size_t            firstMatch(0);                // 1st poss. symbol of match.
  RWBoolean         found(FALSE);                 // Flags a match.
  size_t            leftmost(RW_NPOS);            //Leftmost position in match.
  size_t            longest(RW_NPOS);             //Rightmost position in match

  // Prime the pump.
  // When scanState is popped from the deque, then a new symbol is input.
  state.pushBack(scanState);

  while (stateIndex)    // Try to find leftmost longest match in search string.
  {
    //======================= SCAN NEXT SYMBOL ================================
    
    if (theState == scanState)
    {
      if (state.isEmpty())
      {
        // Scan with empty deque;
        // exhausted all possible matches at this place in string.
        stateIndex = firstStateIndex;  // Reset to beginning of RE.
        theState.number = stateIndex;
        theState.nextToMatch = firstMatch;
        firstMatch++;

        if
        (
         firstMatch > searchStringLength ||
         (
          leftmost != RW_NPOS &&
          (leftmost != longest || leftmost != firstMatch)
          )
         )
          break;          // Out of input or match already found.
        
        
        // Reset to beginning of RE.
        stateIndex = firstStateIndex;
        theState.number = stateIndex;
        theState.nextToMatch = firstMatch;
        stringIndex = firstMatch;

        // Push marker back on for the next time we run out of candidate
        // epsilon states on the front of the deque:
        state.pushBack(scanState);  // Push on scan marker.
        node = firstNode;           // Go back to start of pattern.
        continue;
      } // if (state.isEmpty())

      // Push marker back on for the next time we run out of candidate epsilon
      // states on the front of the deque.
      state.pushBack(scanState);
    } // if (theState == scanState)

    //===================== NONACCEPTING STATE ==============================

    else if (stateIndex == nonacceptingState_)
    {
      // Nonaccepting state: determine if match is leftmost longest.
      // A nonaccepting state is a state that is appended to a regular
      // expression to help determine if a matched string is the longest
      // possible string to be found at that position.

      found = TRUE;

      if (leftmost == RW_NPOS)
        leftmost = firstMatch;

      if
        (
         (
          longest < stringIndex && stringIndex <= searchStringLength &&
          leftmost == firstMatch
          )  ||
         longest == RW_NPOS
         )
          longest = stringIndex;         // Found a longer match.
    }   //  if (stateIndex == nonacceptingState_)

    //============== STATE WITH EPSILON TRANSITIONS =========================

    else if
      (
       node.input_ == nullCharT()        &&
       !node.matchAny_                   &&
       !node.symbolSetIndex_             &&
       !stateUsed.testBit(node.next1_)   &&
       (
        node.anchor_ == notAnchored ||
        (node.anchor_ == anchorFront && stringIndex == 0)      ||
        (node.anchor_ == anchorBack && stringIndex == searchStringLength)
        )
       ) // Guess which epsilon transition to choose.
      {

        // Guess: Push next1 onto front of deque.
        theState.number = node.next1_;
        theState.nextToMatch = stringIndex;
        state.pushFront(theState);
        stateUsed.setBit(theState.number);

        // Don't make any changes to theState before if () below.

        if (!stateUsed.testBit(node.next2_))
        {
          // Guess: Push next2 onto front of deque.
          theState.number = node.next2_;
          theState.nextToMatch = stringIndex;
          state.pushFront(theState);
          stateUsed.setBit(theState.number);
        }
      } // else guess on epsilon.

    // ===================== MATCH FOUND ==================================

    else if // Exact match or match from character class expression or "."
      (
       searchStringPtr[stringIndex] == node.input_               ||
       node.matchAny_                                            ||
       (
        node.input_ == nullCharT()                              &&
        isMatchedInSymbolSet
        (node.symbolSetIndex_, searchStringPtr[stringIndex])
        )
       )
      {
        // Start finding a match.

        theState.number = node.next1_;      // Push next state onto back of deque.

        if (!stateUsed.testBit(theState.number))
        {
          // Push theState onto back of deque.
          theState.nextToMatch = stringIndex + 1;
          state.pushBack(theState);
          stateUsed.setBit(theState.number);
        }
      }       // else started finding a match.

    //=================== POP ANOTHER STATE OFF FRONT ========================

    // Pop from front of deque:
    theState = state.popFront();
    stateIndex = theState.number;

    if (stateIndex != RW_NPOS)
    {
      stateUsed.clearBit(stateIndex);
      node = getNode(stateIndex);
    }
    stringIndex = theState.nextToMatch;

    if (stringIndex > searchStringLength && stringIndex != RW_NPOS)
      break;     // Out of input
  } // while

  //=================== CLEANUP AND RETURN ===================================

  if (found)
  {
    pmatch[0].rm_so = leftmost;
    pmatch[0].rm_eo = longest;          // rm_eo points to char past last char.
  }
  else
  {
    // No match found at end of while.
    pmatch[0].rm_so = RW_NPOS;
    pmatch[0].rm_eo = RW_NPOS;          // rm_eo points to char past last char.
    return REG_NOMATCH;
  }

  // Found a match.
  return 0;                             // Found a match.
}

#endif // defined(RW_NO_POSIX_RE)
#endif //!defined(RW_NO_STL)
