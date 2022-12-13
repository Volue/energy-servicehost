#ifndef __RWRETRAIT_H__
#define __RWRETRAIT_H__

/*
 * Template trait declarations for class RWTRegularExpression<char>
 *
 * $Id: retrait.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/cstring.h"

#if !defined(RW_NO_STL) && !defined(RW_NO_EXCEPTIONS)

#include "rw/tvordvec.h"
#include "rw/tbitvec.h"


#if defined( macintosh )  //jvd,djk
# define BITSPERBYTE RWBITSPERBYTE
#else
# if defined(_MSC_VER) || (defined(__IBMCPP__) && defined(__OS2__))
#   define BITSPERBYTE 8
# else
#   ifdef __TURBOC__    //Suppress duplicate definition warning in Borland 4.52
#     undef MAXSHORT
#     undef MAXLONG
#   endif
#   include <values.h>
# endif  /* _MSC_VER */
#endif /* (macintosh) */

/////////////////////////////////////////////////////////////////////////////
//
// RWTwoToThePowerOf
//
//  Calculates powers of two at compile time.
//  Used to initialize RWTRegularExpressionTraits::symbolSetT.
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(RW_NO_EXPRESSION_TEMPLATES)
template<int N>
class RWTwoToThePowerOf
{
public:
#if !defined(__SUNPRO_CC) || __SUNPRO_CC<0x420 
    enum {value = 2 * RWTwoToThePowerOf<N - 1>::value};
#else
    int p =  RWTwoToThePowerOf<N - 1>::value;
    enum {value = 2*p};
#endif /* !SunPro 420 */
};

#ifndef RW_NO_NEW_TEMPLATE_SYNTAX
  template<>
#endif
class RWTwoToThePowerOf<0>
{
public:
  enum {value = 1};
};
#endif


/////////////////////////////////////////////////////////////////////////////
//
// RWTRegularExpressionTraits
//
//  Trait classes that enables RWTRegularExpression to specialize on
//  different types.
//
/////////////////////////////////////////////////////////////////////////////

#if defined(RW_BROKEN_TEMPLATE_TYPEDEFS)
  template<class charT> class RWTExport RWTRegularExpressionTraits
    {
    public:
    typedef     int   tStringT;
#   if defined(RW_NO_POSIX_RE)
      typedef   int   symbolSetT;
      typedef   int   symbolSetTVector;
#   endif
    };  // Dummy typedefs.
#else
  template<class charT> class RWTRegularExpressionTraits{};
#endif

#undef RWC
#define RWC(name, value) static char name() {return(value);}

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

#ifndef RW_NO_NEW_TEMPLATE_SYNTAX
  template<>
#endif
class RWExport RWTRegularExpressionTraits<char>
{
public:

  // Used in RWTRegularExpression<charT> to specialize on charT
  typedef RWCString                                     tStringT;

  // Extended regular expression metacharacters.
  RWC(a,              'a');
  RWC(b,              'b');
  RWC(bar,            '|');
  RWC(backslash,      '\\');
  RWC(backspace,      '\b');
  RWC(c,              'c');
  RWC(colon,          ':');
  RWC(closeBrace,     '}');
  RWC(closeBracket,   ']');
  RWC(closeParen,     ')');
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
#if defined(RW_NO_EXPRESSION_TEMPLATES) || defined(__SUNPRO_CC)
  typedef RWTBitVec<256>                                    symbolSetT;
#else
  typedef RWTBitVec<(int)RWTwoToThePowerOf<BITSPERBYTE>::value>  symbolSetT;
#endif
  typedef RWTValOrderedVector<symbolSetT>                   symbolSetTVector;
    // Used in RWTRegularExpression<charT> to match charT input symbols
    // in range expressions and character class expressions.

  static RWBoolean  isChar(){return TRUE;}  // Used to see if can use "fast" RE.

  // nullCharT returns a null character value for any symbol type.
  static char     nullCharT() {return 0;}

  // Adds character class expression match maps, and returns
  //   index of new symbol set entry:
  static size_t   addSymbolSet
  (
    symbolSetTVector&         symbolSetTVector, // Ref to symbol set array.
    const tStringT&           setString     // Character class string in RE.
  );

  // Returns TRUE if "symbol" has a match in symbol set 
  //   pointed to by "setIndex":
  static RWBoolean isMatchedInSymbolSet
  (
    symbolSetT&       symbolSet,  // Symbol set of character class expression.
                                  // Can't be const because
                                  // RWTBitVec::operator[] isn't const.
    const unsigned char        symbol
  )
  {
    size_t  i(symbol);

    if (symbolSet.testBit(i))
      return TRUE;

    return FALSE;
  }
#endif  // RW_NO_POSIX_RE
};  // RWTRegularExpressionTraits<char>

#undef RWC

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* !defined(RW_NO_STL) */

#endif /* __RWRETRAIT_H__ */
