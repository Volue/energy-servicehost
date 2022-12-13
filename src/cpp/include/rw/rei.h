#ifndef __RWREI_H__
#define __RWREI_H__ 1

/*
 * Declarations for class RWRegularExpressionImp and related classes.
 *
 * $Id: rei.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/retrait.h"

#if !defined(RW_NO_STL) && !defined(RW_NO_EXCEPTIONS)

#ifdef RW_TOOLSPRO
# include "rw/rewtrait.h"
#endif
#include "rw/tvordvec.h"

#undef RWC
#define RWC(name, dummy) \
    static charT name() \
      {return RWTRegularExpressionTraits<charT>::name();}

#if !defined(RW_NO_POSIX_RE)
  // If your compiler supports POSIX.2-compliant regular expressions
  // then you can comment out #define RW_NO_POSIX_RE
  // in rw/compiler.h to use the compiler's RE functions in this class.
  
#ifndef RW_DIRECTORY_WO_SLASHES                                         // jvd
# include <sys/types.h>
#else
# include <types.h>
#endif  /* RW_DIRECTORY_WO_SLASHES */

# include <regex.h>
#define RW_REGCOMP(preg, pattern, cflags) \
  ::regcomp(preg, pattern, cflags);
#define RW_REGEXEC(preg, string, stringLength, nmatch, pmatch, eflags) \
  ::regexec(preg, string, nmatch, pmatch, eflags);

#else
  // The following #defines were done to minimize the passing of unused
  // arguments on the stack.  These will have to be changed if the
  // member functions are changed.
#define RW_REGCOMP(preg, pattern, cflags) \
  rwRegcomp(pattern);
# define RW_REGEXEC(preg, string, stringLength, nmatch, pmatch, eflags) \
  rwRegexec(string, stringLength, pmatch);
  // stringLength is added to take care of embedded nulls.

#include "rw/regexp.h"              // Fast RE class, RWCRegexp.

/////////////////////////////////////////////////////////////////////////////
//
// RWRERange
//
//  Class used to store range info.  This class isn't nested in
//  RWTRegularExpression because some compilers don't understand a nested
//  class name qualified with a templatized class name.
//
//  Used in RWTRegularExpressionImp during regular expression compilation and
//  during regular expression pattern matching.
//
//  RWRERange(const RWRERange& other) and operator=(const RWRERange& other)
//  are omitted because automatically-defined memberwise copy can safely and
//  perhaps more quickly copy present data members.
//
/////////////////////////////////////////////////////////////////////////////

class RWRERange
{
public:
  size_t              start_;    // First state in range.
  size_t              end_;      // Last state in range.

  RWRERange(size_t first = RW_NPOS, size_t last = RW_NPOS)
    : start_(first), end_(last) {}

  RWBoolean   operator==(const RWRERange& other) const
    {return (start_ == other.start_ && end_ == other.end_);}

  RWBoolean   operator<(const RWRERange& other) const
    {return (start_ < other.start_ ||
             (start_ == other.start_ && end_ < other.end_));}

  RWBoolean   isValid() {return (start_ != RW_NPOS && end_ != RW_NPOS);}

private:
  RWRERange&  operator=(size_t){return *this;}
                // Dummy to prevent assignment of size_t to RWRERange.
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};

/////////////////////////////////////////////////////////////////////////////
//
// RWRESubexpression
//
//  Class stores both the starting and ending states of parenthesized
//  subexpressions in a given regular expression.
//
//  Used by RWTValSortedVector, which requires well-defined copy, assignment
//  and equality semantics as well as a default constructor.
//
//  Used in RWTRegularExpressionImp during regular expression compilation and
//  during regular expression pattern matching.
//
/////////////////////////////////////////////////////////////////////////////

class RWRESubexpression : public RWRERange
{

public:
  RWRESubexpression(size_t first = 0, size_t last = 0)
    : RWRERange(first, last) {}

  ~RWRESubexpression() {}

  RWBoolean           operator==(const RWRESubexpression& other) const
    {return RWRERange::operator==(other);}

  RW_TYPESPEC  /* This macro usually expands to nothing */
};

/////////////////////////////////////////////////////////////////////////////
//
// RWTREToken
//
//  Class supplies regular expression tokens for parsing.
//
//  Used in RWTRegularExpressionImp during regular expression compilation.
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> class RWTREToken
{
  typedef typename RWTRegularExpressionTraits<charT>::tStringT    stringT;
  
public:
  RWTREToken(const stringT& s) : tIndex_(0), tString_(s){Set();}
  RWTREToken()
    :
  tIndex_(0),
  tToken_(RWTRegularExpressionTraits<charT>::nullCharT())
  {}
  charT         operator()()
  {
    return
      (
       tIsEmpty_ ?
       (tToken_ = RWTRegularExpressionTraits<charT>::nullCharT()) :
       tToken_
       );
  }
  charT               operator*()
  {
    return
      (
       tIsEmpty_ ?
       (tToken_ = RWTRegularExpressionTraits<charT>::nullCharT()) :
       tToken_
       );
  }
  RWTREToken<charT>&   operator=(const stringT& s)
  {
    tString_ = s;
    tIndex_ = 0;
    Set();
    return *this;
  }
  RWTREToken<charT>&   operator=(const charT* cp)
  {
    tString_ = cp;
    tIndex_ = 0;
    Set();
    return *this;
  }
  void          operator++(int)             {Set(); tIndex_++;}
  void          operator--(int)             {tIndex_--; Set();}
  RWBoolean     isEmpty()                   {return tIsEmpty_;}
  size_t  position()                  {return tIndex_ - 1;}
  size_t  position(const size_t i)
  {
    tIndex_ = i;
    Set();
    tIndex_++;
    return tIndex_ - 1;
  }
  
private:
  charT       tToken_;  // Used instead of tString_[tIndex_] to reduce
  // time overhead of operator().
  size_t      tIndex_;
  stringT     tString_;
  RWBoolean   tIsEmpty_;
  void        Set()
  {
    // tIndex now points to next token to get.
    
    if (tIndex_ < tString_.length())
    {
      tToken_ = tString_[tIndex_];
      tIsEmpty_ = FALSE;
    }
    else
    {
      tToken_ = RWTRegularExpressionTraits<charT>::nullCharT();
      tIsEmpty_ = TRUE;
    }
  }

  RW_T_TYPESPEC  /* This macro usually expands to nothing */

}; // RWTREToken

/////////////////////////////////////////////////////////////////////////////
//
// RWTRENode
//
//  Class is node of finite state machine contained in regex_t.
//
//  Used in RWTRegularExpressionImp during regular expression compilation
//  and during pattern matching.
//
//  RWRENode(const RWRERange& other) and operator=(const RWRENode& other)
//  are omitted because automatically-defined memberwise copy can safely and
//  perhaps more quickly copy present data members.
//
/////////////////////////////////////////////////////////////////////////////

template<class charT> class RWRENode
{
public:

  charT     input_;           // Symbol that fires transition.
  size_t    next1_;           // First transition to next state.
  size_t    next2_;           // 2nd transition to next state.
  size_t    symbolSetIndex_;  // Pointer to character class.
  RWBoolean matchAny_;        // Match any character.
  int       anchor_;          // Flags anchoring match.


  RWRENode
  (
    charT     input,
    size_t    next1             = 0,
    size_t    next2             = 0,
    size_t    symbolSetIndex    = 0,
    RWBoolean matchAny          = FALSE,
    int       anchor            = 0
  )
    : input_ (input), next1_(next1), next2_(next2),
      symbolSetIndex_(symbolSetIndex), matchAny_(matchAny), anchor_(anchor)
    {}

  RWRENode()
    : input_ (RWTRegularExpressionTraits<charT>::nullCharT()),
      next1_(0), next2_(0), symbolSetIndex_(0), matchAny_(FALSE), anchor_(0)
    {}

  ~RWRENode() {}

  RWBoolean           operator==(const RWRENode<charT>& other) const
    {
      return
        input_ == other.input_                    &&
        next1_ == other.next1_                    &&
        next2_ == other.next2_                    &&
        symbolSetIndex_ == other.symbolSetIndex_  &&
        matchAny_ == other.matchAny_              &&
        anchor_ == other.anchor_
        ;
    }

  // Do not use. Given only to appease compilers that instantiate everything:
  RWBoolean           operator<(const RWRENode<charT>& other) const
    { return FALSE; }

  RW_T_TYPESPEC  /* This macro usually expands to nothing */

};

/////////////////////////////////////////////////////////////////////////////
//
// RWRECompileError
//
// Exception for use within RWTRegularExpressionImp<charT>::rwRegcomp() only.
// Used to keep recursive descent parser clean.
//
/////////////////////////////////////////////////////////////////////////////

class RWRECompileError
{
public:
  int no;
  RWRECompileError(int n) : no(n) {}
};

#endif  // defined(RW_NO_POSIX_RE)


/////////////////////////////////////////////////////////////////////////////
//
// RWTRegularExpressionImp<charT>
//
//  Templatized Regular Expression implementation.
//
/////////////////////////////////////////////////////////////////////////////

#include "rw/rebase.h"

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

template<class charT> class RWTExport RWTRegularExpressionImp
  : public RWREBaseClass
{
friend class RWTRegularExpression<charT>;

protected:  // All members accessed by RWTRegularExpression here are protected.

  typedef typename RWTRegularExpressionTraits<charT>::tStringT         stringT;
  typedef typename RWTRegularExpressionTraits<charT>::symbolSetT       symbolSetT;
  typedef typename RWTRegularExpressionTraits<charT>::symbolSetTVector symbolSetTVector;

  void    compile();    // Compiles RE. This declaration needs to be here
  // rather than in Compilation section because otherwise the Solaris Cafe
  // compilation of
  // RWTRegularExpressionImp(const RWTRegularExpressionImp<charT>&   other);
  // won't be able to find compile().
  
#if defined(RW_NO_POSIX_RE)
  // These declarations need to be here rather than in Compilation section
  // because otherwise the Solaris Cafe compilation of this class can't find the
  // dependent member functions' source.  Essentially, for this to compile
  // under Cafe, Cafe must see declarations of first member function before the
  // declaration of a second member function that calls the first
  // member function.  #ifdef's aren't used to show the alternate positions
  // of the member function declarations because duplication of code can lead
  // to subtle errors if all duplicates aren't kept up to date.

private:
  struct  regex_t       // POSIX.2 RE finite automaton structure.
    // Frees space used by compiled RE. This declaration needs to be here
    // rather than in Compilation and Matching Patterns section because
    // otherwise the Solaris Cafe
    // compilation of ~RWTRegularExpressionImp()won't be able to find regfree().
  {                     // Only re_nsub is required by POSIX.2.
    size_t                re_nsub;          // # of parenthesized subexpressions.
    RWTValOrderedVector<RWRESubexpression>
                          subexpression;    // ()'d subexpressions.
    RWTValOrderedVector<RWRENode<charT> >
                          node;             // Node of RE finite automaton.
    regex_t()
      : re_nsub(0), node(16),
    subexpression(MAX_NO_OF_SUBEXPRESSIONS) {}
  };

  void              regfree(regex_t* preg){delete preg;}  // POSIX.2 C-binding.
    // Frees space used by compiled RE. This definition needs to be here
    // rather than in Compilation section because otherwise the Solaris Cafe
    // compilation of ~RWTRegularExpressionImp()won't be able to find regfree().
  enum anchorType {notAnchored = 0, anchorFront, anchorBack}; // Used in RWRENode.
  void      build   // Builds automaton nodes.  Used by rwRegcomp().
    (
     const charT                symbol,       // Input that fires transition(s).
     const size_t               transition1     = 0,
     const size_t               transition2     = 0,
     const size_t    symbolSetIndex  = 0,            // Character range.
     const RWBoolean matchAny        =FALSE,         // "."
     const int       anchor          = notAnchored   // "^" or "$"
     );
  
  void      set     // Sets values of automaton nodes.  Used by rwRegcomp().
    (
     const size_t               stateIndex,   // Node of automaton.
     const charT                symbol,       // Input that fires transition(s).
     const size_t               transition1,
     const size_t               transition2,
     const size_t    symbolSetIndex  = 0,            // Character range
     const RWBoolean matchAny        = FALSE,        // "."
     const int       anchor          = notAnchored   // "^" or "$".
     );
  
  // rwRegcomp() recursive descent compiler members.
  size_t          bracketExpression();
  RWBoolean       bracketList();
  RWBoolean       characterClass();
  RWBoolean       collatingElement();
  RWBoolean       collatingSymbol();
  RWBoolean       endRange();
  RWBoolean       equivalenceClass();
  RWRERange       ere(const RWRERange&);
  RWRERange       ereBranch(const RWRERange&);
  RWRERange       ereBranchRest();
  RWRERange       ereExpression(const RWRERange&);
  RWRERange       ereExpressionRest(const RWRERange&);
  RWRERange       ereExpressionToken(const RWRERange&);
  RWRERange       ereRepeat(const RWRERange&);
  RWRERange       ereRest(const RWRERange&);
  RWBoolean       expressionTerm();
  RWBoolean       followList();
  void            followListRest();
  RWRERange       makeStar(const RWRERange&);
  size_t          oneCharEre();
  size_t          ordinaryChar();
  size_t          quotedChar();
  RWBoolean       rangeExpression();
  RWBoolean       singleExpression();
  size_t          specialChar();
  RWBoolean       startRange();
  
  
  int       rwRegcomp(const stringT& pattern);  // RE compiler. Replaces regcomp()
#endif

  void              clear();    // Clears internal state of regular expression.
  // This declaration needs to be here rather than in Compilation section
  // because otherwise the Solaris Cafe compilation of constructors
  // won't be able to find hexStringToChar().

  RWTRegularExpressionImp();
  // If you use the default constructor, then you must use operator=
  // before you call the execute() function.

#ifdef RW_BROKEN_NAME_RESOLUTION
/* 
 *   The following system of macros allows a portion of a given ".cc" file
 *   to be used within a class body. This portion can alternatively be used
 *   to define templated function bodies in the normal fashion when needed.
 *   (See the second include of this same ".cc" file at the end of this 
 *   file).
 *   The remaining portion of the ".cc" file templates can only be used in the
 *   standard fashion. They are read during the include at the end of this 
 *   file.
 *   For more information see the comments at the beginning of the ".cc" file.
 *   Also, the methodology is commented in greatest detail in files slist.h and
 *   slist.cc. Study those files if further information is needed
 */
#define RW_GET_INLINES_FOR_CLASS_BODY
#include "rw/rei.cc"
#undef RW_GET_INLINES_FOR_CLASS_BODY
#define RW_GOT_INLINES_FOR_CLASS_BODY_REI_CC      

#else
  
  RWTRegularExpressionImp(const charT* reStringPtr);

#if defined(RW_BROKEN_TEMPLATE_TYPEDEFS)
  RWTRegularExpressionImp(const RWTRegularExpressionTraits<charT>::tStringT&);
#else
  RWTRegularExpressionImp(const stringT&);
#endif

  RWTRegularExpressionImp(const RWTRegularExpressionImp<charT>&);

#endif /* RW_BROKEN_NAME_RESOLUTION */


  virtual ~RWTRegularExpressionImp();

  RWTRegularExpressionImp<charT>& operator=(const charT*  other);
  RWTRegularExpressionImp<charT>& operator=
    (const RWTRegularExpressionImp<charT>&  other);
  RWTRegularExpressionImp<charT>& operator=(const stringT& string)
  {
    reString_ = string;
    compile();
    return *this;
  }

  statusType    status() const {return status_;}
  
private:  // All members used only by this class are private.
  
//----- Both Regular Expression Compilation and Pattern Matching Members -----

# if  defined(RW_NO_POSIX_RE)
  // Below are POSIX.2 C bindings for regular expressions.
  // All of these must be kept as members in case user wishes to use
  // Rogue Wave implementations of POSIX.2 C services rather than
  // operating system services.  For example, present POSIX.2 services do not
  // support wchar_t or embedded nulls.

public:  // errorType is public so that RWRECompileError can use it.
  enum errorType      /* POSIX.2 error codes found in <regex.h> */
  {
    REG_NOMATCH = 1, REG_BADPAT, REG_ECOLLATE, REG_ECTYPE, REG_EESCAPE,
    REG_ESUBREG, REG_EBRACK, REG_EPAREN, REG_EBRACE, REG_BADBR,
    REG_ERANGE, REG_ESPACE, REG_BADRPT
   };

private:
  // Members used by RWTRegularExpressionImp's POSIX.2 C-bindings but
  // not POSIX.2 C-bindings.

  const size_t      maxState_;          // The largest possible state in NFA.
  const size_t      getNextSymbol_;     // fake state that tells rwRegexec()
                                        //  to scan in another symbol from the
                                        //  search string.

  size_t            nonacceptingState_; // fake state that tells rwRegexec()
                                        //  to search for a longer leftmost
                                        //  substring.
  symbolSetTVector  symbolSetVector_;   // Used to match symbols in range and
                                        //  character class expressions.
  RWCRegexp*        fastRePtr_;         // RWCRegexp pointer to "fast" RE class

# endif  // defined(RW_NO_POSIX_RE)

  size_t            unmatchedOpenParen_;   // number of unmatched open 
                                           // parenthesis
  size_t            bracketListStartPos_;  
                                // the first char position in the 
                                // bracket list of a bracket expression
  RWBoolean         compiled_;  // TRUE if re_ is compiled_
  statusType        status_;
  regex_t*          rePtr_;     // POSIX.2-compatible finite automaton.
  stringT           reString_;  // Copy of RE in text form. Need to keep this
                                // after compile() is called in order to deep
                                // copy this class when defined(RW_NO_POSIX_RE)

#if defined(RW_NO_POSIX_RE)

  // Regular expression Nondeterministic Finite Automaton (regex_t) accessors.
  
  int               anchor(const size_t stateIndex)
        {return rePtr_->node[stateIndex].anchor_;}
  RWRENode<charT>   getNode(const size_t stateIndex)
        {return rePtr_->node[stateIndex];}
  charT             input(const size_t stateIndex)
        {return rePtr_->node[stateIndex].input_;}
  RWBoolean         matchAny(const size_t stateIndex)
        {return rePtr_->node[stateIndex].matchAny_;}
  size_t            next1(const size_t stateIndex)
        {return rePtr_->node[stateIndex].next1_;}
  size_t           next2(const size_t stateIndex)
        {return rePtr_->node[stateIndex].next2_;}
  size_t           symbolSetIndex(const size_t stateIndex)
  {
    if
      (
       rePtr_->node[stateIndex].symbolSetIndex_
       < symbolSetVector_.entries()
       )
        return rePtr_->node[stateIndex].symbolSetIndex_;
    else
      return 0;
  }
#endif

//------------------ Regular Expression Compilation Members -----------------

protected:

# if  defined(RW_NO_POSIX_RE)
  // Below are POSIX.2 C bindings for regular expressions.
  // All of these must be kept as members in case user wishes to use
  // Rogue Wave implementations of POSIX.2 C services rather than
  // operating system services.  For example, present POSIX.2 services do not
  // support wchar_t or embedded nulls.

private:
  enum compileFlags       // POSIX.2 compile flags.
  {
    REG_EXTENDED  = 0x0001,
    REG_ICASE     = 0x0002,
    REG_NOSUB     = 0x0004,
    REG_NEWLINE   = 0x0008
  };

  // Members used by RWTRegularExpressionImp's POSIX.2 C-bindings but
  // not POSIX.2 C-bindings.

  size_t           nextState_;         // The next state to build in the NFA.
  RWTREToken<charT> token_;            // Regex token for rwRegcomp
  stringT           symbolSetString_;  // Used by rwRegcomp() to construct
                                       // symbol sets that are stored in
                                       // symbolSetVector_ above.

# endif  // defined(RW_NO_POSIX_RE)

  // RWC macros here and in retrait.h are used to retrieve either char
  // or wchar_t versions of metacharacters.
  RWC(a,              'a');
  RWC(b,              'b');
  RWC(bar,            '|');
        RWC(backslash,      '\\');
  RWC(backspace,      '\b');
  RWC(c,              'c');
  RWC(closeBrace,     '}');
  RWC(closeBracket,   ']');
  RWC(closeParen,     ')');
  RWC(colon,          ':');
  RWC(d,              'd');
  RWC(dash,           '-');
  RWC(dollar,         '$');
  RWC(dot,            '.');
  RWC(e,              'e');
  RWC(eight,          '8');
  RWC(equal,          '=');
  RWC(f,              'f');
  RWC(five,           '5');
  RWC(formfeed,       '\f');
  RWC(four,           '4');
  RWC(hat,            '^');
  RWC(n,              'n');
  RWC(newlineChar,    '\n');
  RWC(nine,           '9');
  RWC(one,            '1');
  RWC(openBrace,      '{');
  RWC(openBracket,    '[');
  RWC(openParen,      '(');
  RWC(plus,           '+');
  RWC(questionMark,   '?');
  RWC(r,              'r');
  RWC(seven,          '7');
  RWC(six,            '6');
  RWC(star,           '*');
  RWC(t,              't');
  RWC(tab,            '\t');
  RWC(three,          '3');
  RWC(two,            '2');
  RWC(x,              'x');
  RWC(zero,           '0');

#if defined(RW_NO_POSIX_RE)

  // nullCharT returns a null character value for any symbol type.
  static charT  nullCharT()
        {return RWTRegularExpressionTraits<charT>::nullCharT();}

  size_t    addSymbolSet()      // Adds a character class expression match map.
                                //  Returns index of new symbol set.
                                //  Used by rwRegcomp().
  {
    return RWTRegularExpressionTraits<charT>::addSymbolSet
      (symbolSetVector_, symbolSetString_);
  }


  // Parenthesized subexpression manipulation functions.

  void    addSubexpression(const size_t start, const size_t end)
  {
    rePtr_->re_nsub++;
    rePtr_->subexpression.insert(RWRESubexpression
                                 (start, end));
  }
  
  RWRESubexpression getSubexpression(const size_t i)
        {return rePtr_->subexpression[i];}

  void              subexpression (
                                   const size_t i,
                                   const size_t start,
                                   const size_t end
                                  )
  {
    rePtr_->subexpression[i] =
      RWRESubexpression(start, end);
  }
  void              subexpression (
                                   const size_t i,
                                   const RWRESubexpression sub
                                  )
        { rePtr_->subexpression[i] = sub; }

    size_t         lastSubexpression(){return numberOfSubexpressions();}
    size_t         numberOfSubexpressions(){return rePtr_->re_nsub - 1;}
  // RWRESubexpression 0 is whole string.


//------------------ Pattern Matching Members -------------------------------

  // Below are POSIX.2 C bindings for regular expressions.
  // All of these must be kept as members in case user wishes to use
  // Rogue Wave implementations of POSIX.2 C services rather than
  // operating system services.  For example, present POSIX.2 services do not
  // support wchar_t or embedded nulls.

  enum executeFlags       //  POSIX.2 execute flags.
  {
    REG_NOTBOL  = 0x0010,
    REG_NOTEOL  = 0x0020
  };

public:
  typedef size_t regoff_t;    // POSIX.2 RE byte offset.  public so can be
                              // accessed by regmatch_t.
private:

  struct regmatch_t           // POSIX.2 RE match structure.
  {
    regoff_t  rm_so;    // Byte offset from start of string
                        //  to start of substring.
    regoff_t  rm_eo;    // Byte offset from start of string of first char
  };                    //  after the end of substring.

  // Returns TRUE if "symbol" has a match in symbol set pointed to by 
  // "setIndex". Used by rwRegexec().
  RWBoolean isMatchedInSymbolSet (
                                  const size_t      setIndex,
                                  const unsigned char        symbol
                                 )
  {
    if (setIndex < symbolSetVector_.entries())
      return RWTRegularExpressionTraits<charT>::isMatchedInSymbolSet
        (symbolSetVector_[setIndex], symbol);
    
    return FALSE;
  }

  // Pattern matching function. Replaces POSIX.2 regexec()
  int       rwRegexec    (
                  const charT*  searchStringPtr,    // String to be searched.
                  size_t        searchStringLength, // Length to search
                                                    // including embedded nulls.
                  regmatch_t    pmatch[]            // Array of matches found.
                         );
#endif  // RW_NO_POSIX_RE

public: // This member function is down here because some compilers can't
        // find it otherwise.
  size_t          index   (
#if defined(RW_BROKEN_TEMPLATE_TYPEDEFS)
                           const RWTRegularExpressionTraits<charT>::tStringT&
                           searchString,
#else
                           const stringT&  searchString,
#endif
                           size_t*         len       = NULL,
                           size_t          start     = 0
                          );
  RW_T_TYPESPEC  /* This macro usually expands to nothing */
};

#undef RWC

/*
 *   This is (possibly) the second include of the same ".cc" file. It 
 *   may have been included fron inside the class body above. Please see the
 *   comments in the class body above and at the beginning of the ".cc" file
 *   if you need to further understand what is accomplished with these macros.
 */

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("rei.cc")
#endif

#if defined(RW_COMPILE_INSTANTIATE)
# include "rw/rei.cc"
# include "rw/reposix1.cc"
# include "rw/reposix2.cc"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif


#endif /* !defined(RW_NO_STL) */

#endif /* __RWREI_H__ */
