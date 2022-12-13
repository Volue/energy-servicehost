#ifndef __RWCOMPILER_H__
#define __RWCOMPILER_H__ 1

#if 0
/*
 * Compiler and system related foibles and directives
 *
 * $Id: compiler.h 313034 2019-05-06 06:34:42Z siv $
 * $Revision: 313034 $
 *
 * (c) Copyright 1989-1998 Rogue Wave Software, Inc.
 * ALL RIGHTS RESERVED
 *
 * The software and information contained herein are proprietary to, and
 * comprise valuable trade secrets of, Rogue Wave Software, Inc., which
 * intends to preserve as trade secrets such software and information.
 * This software is furnished pursuant to a written license agreement and
 * may be used, copied, transmitted, and stored only in accordance with
 * the terms of such license and with the inclusion of the above copyright
 * notice.  This software and information or any other copies thereof may
 * not be provided or otherwise made available to any other person.
 *
 * Notwithstanding any other lease or license that may pertain to, or
 * accompany the delivery of, this computer software and information, the
 * rights of the Government regarding its use, reproduction and disclosure
 * are as set forth in Section 52.227-19 of the FARS Computer
 * Software-Restricted Rights clause.
 * 
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 * Contractor/Manufacturer is Rogue Wave Software, Inc.,
 * 5500 Flatiron Parkway, Boulder, CO 80301.
 *
 * This computer software and information is distributed with "restricted
 * rights."  Use, duplication or disclosure is subject to restrictions as
 * set forth in NASA FAR SUP 18-52.227-79 (April 1985) "Commercial
 * Computer Software-Restricted Rights (April 1985)."  If the Clause at
 * 18-52.227-74 "Rights in Data General" is specified in the contract,
 * then the "Alternate III" clause applies.
 *
 * The file compiler.h is automatically generated.
 *
 * Following are brief descriptions of each preprocessor macro which
 * is defined or not in each platform's code block.

 * Although you may edit this file if you wish, we encourage you in
 * most cases to use the configuration script which shipped with
 * your Rogue Wave library unless it is making a wrong decision for
 * your compiler.  If that is the case, please contact Rogue Wave
 * Technical Support so that we may correct the problem for you and
 * other future users of our libraries.

 *** Some macros will be "default" configured, either because we cannot
 *** determine the situation on your machine, or because you may have
 *** more than one reasonable choice. Such macros will be marked in
 *** the descriptions below by ">>>" rather than the "***" which marks
 *** macros which we believe we can automatically configure for you.
 *** If you wish to change those macros, please search for them in the
 *** section of this header devoted to your platform, and make the
 *** change there.


 Availability and version of the standard library
 ************************************************
 >>> RW_NO_BOOL: Defined in either of: Your compiler has no bool type;
     or we cannot use the compiler's bool type for link compatibility
     reasons. You might undef this if you don't mind recompiling all
     your object code, and your compiler does support bool, but
     RWBoolean is still typedef'd to int.
 *** RW_NO_STL: Defined if your compiler doesn't support
     the C++ Standard Library containers, iterators, or algorithms.
 *** RW_3ARG_DISTANCE: Defined if utility function distance() takes
     a third, 'reference return' argument.
 *** RW_ALLOCATOR_NO_REBIND: Defined if the allocator interface
     does not support the static member function rebind().
 *** RW_ALLOCATOR_PLAIN: Defined if the allocator interface is not
     templatized.
 *** RW_ALLOC_INTERFACE_HACK: Defined if the allocator class in 
     your standard library has a parallel allocator_interface class
     to get around compiler limitations.
 *** RW_ITER_NO_TAGS: Defined if the base iterator class is not
     templatized on iterator tags.
 *** RW_NO_PTR_PAIR_CTOR: Defined if the compiler doesn't support
     constructors of classes templatized on T with the parameters:
     (T*const*, T*const*,...)

 The iostreams library you have
 ******************************
 *** RW_IOS_XALLOC_BROKEN: Defined if the iostream functions
        ios::xalloc() and ios::pword() fail to initialize the
        xalloc'd storage to zero. Prevents RWLocale::imbue(ios&) from
        being available.
 *** RW_NO_IOSTD: Defined if you have the "original flavor" iostreams
     package, as opposed to the emerging draft ANSI C++ standard.
 *** RW_NO_STREAMBUF_STOSSC: Defined if your streambuffers do not
     have the member function stossc(). 
 *** RW_NO_IO_SENTRY: Defined if your streams use opfx and osfx calls
     to set up and tear down, rather than the newer sentry class.
 *** RW_NO_IOSSTREAM:  Adds certain functionality needed for iostream
        support.
 *** RW_NO_PUBSEEKOFF: Older iostream libraries had a public method
        seekoff().  The current standard names the equivalent method
        pubseekoff(). 
        and unusuable which requires this work around.
 *** RW_NO_STRM_GCOUNT: certain libraries don't return the number
        of chars extracted in the last unformatted extraction operation
        through gcount(), as the standard says they should.
 *** RW_NO_UNBUFFERED:  prevents user from allocating new buffer which
        causes memory leak with certain compilers.

 The sizes of things
 *******************
 *** RW_BYTES_PER_WORD:    ...
 *** RW_BYTES_PER_PTR: Defined to the appropriate numbers for your
        platform.
 *** RW_DEFAULT_PRECISION: Defined to the number of decimal digits of
     precision for your platform's type double.
 >>> RW_FIX_XSIZE_T: protects link incompatible code which allows
        persistence with estreams to work between certain platforms
        with incompatible size_t definitions. See toolread.doc.

 Template support
 ****************
 *** RW_NO_TEMPLATES: Defined if your compiler doesn't understand
        template syntax at all.
 *** RW_BROKEN_TEMPLATES: Defined if your compiler understands only
        very simple template code. (Like AT&T CFront V3.0).
 >>> RW_COMPILE_INSTANTIATE: Defined if your compiler requires both
        declarations and definitions of template code to be in the
        same file.  (Some compilers pass our test, for link time
        instantiation, but fail on more complex code)
 *** RW_NO_TEMPLINST_ON_BASE: Defined if your compiler will not
        instantiate a function template when an actual parameter
        type is derived from the corresponding formal parameter
        type (i.e. the compiler demands too exact of a match)
 *** RW_BROKEN_TEMPLATE_TYPEDEFS: Defined if your compiler doesn't
        handle a template class out-of-line member function argument
        list that is based on a typedef scoped in a different class.
 *** RW_NO_EXPRESSION_TEMPLATES: Defined if your compiler cannot
        handle templates that compile into inline code to evaluate
 *** RW_GLOBAL_BEFORE_CLASS_SCOPE: Under certain circumstances,
        some compilers will instantiate global templates rather than
        use overloaded class member functions.
        This type of problem also manifests itself in RW_GLBL_SCP_BUG
        and RW_GLBL_SCP_BUG2 and RW_GLBL_SCP_BUG3 and RW_GLBL_SCP_BUG4.
 *** RW_NO_RESOLVE_TEMPL_AMBIGUITY:  Certain compilers do not want
        the assistance in resolving certain template ambiguities.
        arithmetic expressions.
 *** RW_NO_INHERIT_TEMPL_ACCESS:  Defined if your compiler is unable to
        comprehend the idea of member functions being accessed from
        complicated template parent classes.

 *** RW_NO_DEFAULT_TEMPLATE_ARGS:  Defined if your compiler does not
        support using a template argument as a default parameter.

 Thread Support
 **************
 >>> RW_SOLARIS_THREADS: Defined if you have a Solaris platform that
        supports threads.  (We choose this if you have it, in
        preference to other thread packages. You may prefer a
        different package.)
 *** RW_DCE_THREADS: Defined if you have a threads package which
        meets the Posix 1003.1c draft 4 (DCE) standard
 *** RW_POSIX_D10_THREADS: As above, if you have threads which meet
        the Posix Draft 10 threads description. If you define this,
        then you must also define RW_POSIX_THREADS
 *** RW_NO_THREADS: If you have no threads, or your package doesn't
        meet our expectations for header and function names.

 Exception handling
 ******************
 >>> RW_NO_EXCEPTIONS: Defined if your compiler doesn't adequately
        support exceptions. (You might prefer to compile with no
        exception support, even though your compiler provides it, if
        you have efficiency considerations, and can live without
        "real" exceptions.)
 *** RW_NO_THROW_WITH_SHARED: Defined if your compiler cannot handle
        exceptions thrown from a shared library.
 *** RW_NO_XMSG: We will always define this, since the standard has
        not yet settled... Meanwhile, we provide our own error
        message types.

 Internationalization issues
 ***************************
 *** RW_NO_WSTR: Defined if your compiler doesn't support wide
        character strings such as wslen().
 *** RW_USE_WCHEADERS: Defined if we have to #include <wctype.h> and
        <widec.h> to find wide character string prototypes.
 *** RW_WSTR_C_HEADERS: Defined if we have to #include <ctype.h> and
        <string.h> to find wide character string prototypes.
 *** RW_SUPPLY_WSTR: Defined if we are able to provide the wide string
        support by using the 32-bit Windows API, and you have no other
        wide string support.
 *** RW_NO_OVERLOAD_WCHAR: Defined if your compiler provides wchar_t
        as a typedef rather than a distinct type.
 *** RW_NO_WCSXFRM: Defined if your compiler doesn't provide a
        prototype for function wcsxfrm(). This is an optimization
        that we pass through to you, and don't otherwise use.
 *** RW_NO_LOCALE: Defined if your compiler doesn't fully support the
        ANSI C locale facility. We require setlocale(), strxform()
        strftime() and strcoll().
 >>> RW_NOT_FULL_LOCALE: Defined if your compiler doesn't pass all
        the tests in the Test Suite for Tools.  For use only by
        that product.
 *** RW_NO_STRFTIME_CAPC: Defined if your compiler doesn't support
        the %C directive to strftime(). This is an optimization
        opportunity.
 >>> RW_MESSAGE: We will always define this to RW_NOMSG. You should
        set it otherwise if you have, and wish to use, a messaging
        facility. The choices are:
        RW_NOMSG   : we will use imbedded "english" messages.
        RW_CATGETS : we will use catgets() for messages.
        RW_GETTEXT : we will use gettext() for messages.
        RW_DGETTEXT: we will use dgettext() for messages.
 >>> RW_MESSAGE_SET_NUMBER: Defined to 1 whenever you choose the
        RW_CATGETS messaging facility. You may defined it to another
        number if you need to.

 Issues concerning time and zone
 *******************************
 *** RW_NO_CLOCK: Defined if a prototype for the ANSI C function
        clock() cannot be found in <time.h> or <stdlib.h>
 *** RW_NO_GLOBAL_TZ: Defined if we cannot find global variables
        daylight, timezone, and tzname; or the same names with
        leading underscore.
 *** RW_NO_LEADING_UNDERSCORE: Defined if we find variable daylight,
        and not _daylight.
 *** RW_NO_GETTIMEOFDAY: Defined if we cannot find the time variables
        and can also not find the function gettimeofday().
 *** RW_STRUCT_TM_TZ: Defined if your <time.h> has a struct tm with
        extra member data "tm_zone" and "tm_gmtoff." This is an
        optimization opportunity.
 >>> RW_CENTURY_REQD: If you define this, then several things happen:
	-- RWDate::asString('x') which is the default for printing an
           RWDate always provides 4 digits in the year part of the
           date output.
        -- An RWDate constructed from a string requires 4 digits in
           the year field, and an RWDate constructed from unsigned
           year, month, day requires that the year be greater than 99. 
        -- RWLocale::stringToDate(const RWCString, struct tm*) will
           have the same behavior as the equivalent RWDate
           constructor. 
        -- RWLocale::dateToString will have the same behavior as
           RWDate::asString().
	-- If RWDEBUG is defined, the program will stop during the
           creation of RWDates earlier than 0100. If RWDEBUG is not
           defined, then an invalid RWDate will be constructed, or the
           struct tm* will not be changed when such a date is parsed.

        This macro is undefined by default to remain as for prior
        versions of Tools.h++: RWDates constructed with years between
        1 and 99 inclusive are considered to be in the 20th century.

Operating System issues
 ***********************
 *** RW_CRLF_CONVENTION: Defined if your OS uses both carriage-return
        and line-feed characters at the end of textfile lines.
 *** RW_REVERSED_CR_AND_LF: values for '\n' and '\r' reversed from ASCII.

 Compilers by name
 *****************
 >>> __ATT2__: Defined if your compiler is based on CFront version 2.x
 >>> __ATT3__: Defined if your compiler is based on CFront version 3.x
        These are set based on whether you tell us your compiler is
        "CFront" during configuration.  If you do not tell us your
        compiler is "CFront" and you get errors in bstream.h (and
	other places) try setting the appropriate one.
 *** __GLOCK__: Defined if your compiler is Glockenspiel.

 Miscellaneous quirks and limitations
 ************************************
 *** RW_ACCESS_NON_CONST:  For compilers that forget the const
        modifier for access()
 *** RW_BAD_STATIC_ACCESS: Defined if your compiler prevents derived
        classes from accessing static member functions.
 *** RW_BAD_STATIC_DESTRUCTORS: Defined if your compiler dumps core
        while destroying static objects.  If so, we use this macro to
        prevent such cleanup, relying instead on the fact that the
        application heap is restored to the OS after the app exits.
 *** RW_BROKEN_ACCESS_ADJUST:  typedef to base_class issues
 *** RW_DIRECTORY_WO_SLASHES: Defined if, for example, #include
        <sys/types.h> fails, but #include <types.h> succeeds.
 >>> RW_DONT_USE_MEMPOOL: Defined if you should not use RWMemoryPool
        to cache small heap objects.        
 *** RW_FRIEND_THEN_INLINE: Defined if inline friend functions must be
        declared as "friend inline ..." in that order.
 *** RW_GLOBAL_ENUMS: Some older compilers make enums declared inside
        classes part of the global scope.
 >>> RW_INLINE86_ASSEMBLY: We will never define this. It is here for
        historical reasons. (Used by older Math.h++ versions)
 >>> RW_KR_ONLY: We will never define this. It refers to the style
        of C code which a C (not C++) compiler on your platform can
        handle, and is here for historical reasons. (Used by older
        Math.h++ versions.)
 *** RW_NO_ACCESS_ADJUSTMENT: Defined if your compiler cannot handle
        adjusting the access rights of a base method in a derived
        class using the "::baseMethodName" construct
 *** RW_NO_ANSI_SPRINTF: Defined if your sprintf doesn't return an
        int describing the amount of buffer used.
 *** RW_NO_CPP_RECURSION: Defined if your C preprocessor cannot
        handle recursive macros.
 *** RW_NO_MEMMOVE: Defined if your compiler doesn't provide ANSI C's
        memmove().
 *** RW_NO_NATIVE_GENERIC_H: Defined if your compiler ships without
     a (useful) generic.h header file.
 *** RW_NO_NESTED_QUOTES: Defined if your C preprocessor cannot
        handle macros with nested quotes.
 *** RW_NO_OSTR_REF_CAST: Defined if your compiler cannot cast
        ostream to ostream&.
 *** RW_NO_OVERLOAD_SCHAR: Defined if your compiler doesn't
        distinguish between formal arg "char" and "signed char."
 *** RW_NO_OVERLOAD_UCHAR: As above, but for "unsigned char."
 >>> RW_NO_POSIX_RE: We always define this because POSIX.2 C bindings
        don't support wchar_t or embedded nulls. If you have and can
        use these bindings, you may comment this out.
 *** RW_NO_POSTFIX: If your compiler doesn't distinguish overloading
        postfix operator++ from overloading prefix ++.
 >>> RW_NO_SCHAR: We will always define this to be the same as
        RW_NO_OVERLOAD_SCHAR. Here for historical reasons. (Used by
        Math.h++)
 *** RW_NO_STRICMP: Defined if your compiler doesn't provide
        stricmp() (case independent comparison of char*)
 *** RW_NO_STRNICMP: Defined if your compiler doesn't provide
        strnicmp() (case independent comparison of char*, nchars).
 *** RW_NO_STRSTR: Defined if your compiler doesn't provide ANSI C's
        strstr() (string search).
 *** RW_NO_TYPENAME: Defined if your compiler doesn't support the new
        C++ keyword "typename".
 *** RW_NO_XDR: Defined if your compiler doesn't support XDR streams.
 *** RW_NON_ANSI_HEADERS: Defined if the prototype for memcpy is in
        <memory.h> rather than <string.h> as ANSI requires.
 *** RW_NOT_POSIX_FSTAT: Defined if you don't have POSIX file status
        functions fstat() and access().
 *** RW_SPECIFIC_BASEBASE_ACCESS: Microsoft V4.1 has trouble adjusting
        access to methods defined in a base of base class. Used to
        force an inline call instead.
 *** RW_UNLINK_NON_CONST: defined if the system unlink() function
        requires a non-const char* argument.
 *** RW_WRAP_C: Defined if your header files must be wrapped in the
        'extern "C" { ... }' construct.
 ***/

#endif /* 0 for comments */

/* The next several lines #define constants for later use */
/*** For RW_MESSAGE ***/
#define RW_NOMSG    0x00
#define RW_CATGETS  0x01
#define RW_GETTEXT  0x02
#define RW_DGETTEXT 0x03

/********************************************************************
 *** The following block encodes the knowledge about your compiler and
 *** operating system that we gained by running config. You _should_
 *** never need to change it. However, if you must, please also
 *** contact Rogue Wave Technical support to report the difficulty.
 ********************************************************************/
#ifndef RW_NEVER_ENTER_DEFAULT_HEADER_BLOCK
#define RW_MESSAGE_SET_NUMBER 1

/* #define RW_INLINE86_ASSEMBLY 1 */
/* #define RW_KR_ONLY 1 */
/* #define RW_MESSAGE 1 */
/* #define RW_NOT_POSIX_FSTAT 1 */
/* #define RW_NO_OSTR_REF_CAST 1 */

#define RW_NO_POSIX_RE 1

/* #define RW_NO_SCHAR 1 */
/* #define RW_NO_STRFTIME_CAPC 1 */

#define RW_NO_XMSG 1

/* #define __ATT2__ 1 */
/* #define __ATT3__ 1 */
/* #define __GLOCK__ 1 */

#define RWOSVERSION 400
#define RWCOMPVERSION 4B0
#define RW_BYTES_PER_PTR 4
#define RW_BYTES_PER_WORD 4
#define RW_DEFAULT_PRECISION 16

/* #define RW_POSIX_D10_THREADS 1 */
/* #define RW_DCE_THREADS 1 */
/* #define RW_SOLARIS_THREADS 1 */
/* #define RW_NO_THREADS 1 */
/* #define RW_NO_DCE_PTHREAD_H 1 */
/* #define RW_CMA_WRAPPERS 1 */
/* #define RW_WRAP_C 1 */
/* #define RW_GLOBAL_ENUMS 1 */
/* #define RW_NO_BOOL 1 */
/* #define RW_NO_ACCESS_ADJUSTMENT 1 */
/* #define RW_BROKEN_ACCESS_ADJUST 1 */
/* #define RW_NO_CONST_OVERLOAD 1 */
/* #define RW_NO_EXCEPTIONS 1 */
/* #define RW_NO_THROW_WITH_SHARED 1 */
/* #define RW_NO_OVERLOAD_UCHAR 1 */
/* #define RW_NO_OVERLOAD_SCHAR 1 */
/* #define RW_NO_POSTFIX 1 */
/* #define RW_NO_CPP_RECURSION 1 */
/* #define RW_NO_NESTED_QUOTES 1 */
/* #define RW_BROKEN_TEMPLATES 1 */
/* #define RW_NO_TEMPLATES 1 */
/* #define RW_NO_TEMPLINST_ON_BASE 1 */
/* #define RW_NO_TYPENAME 1 */

#define RW_COMPILE_INSTANTIATE 1

/* #define RW_HEADER_WITH_EXTENSION 1 */
/* #define RW_BROKEN_TEMPLATE_TYPEDEFS 1 */
/* #define RW_NO_REF_TO_REF 1 */
/* #define RW_NO_EXPRESSION_TEMPLATES 1 */
/* #define RW_NO_ANSI_SPRINTF 1 */
/* #define RW_NO_STRICMP 1 */
/* #define RW_NO_STRNICMP 1 */
/* #define RW_NO_STRSTR 1 */
/* #define RW_NO_MEMMOVE 1 */

#define RW_MAXINT_CONFLICT 1

/* #define RW_INHERIT_ACCESS_PROBLEM 1 */

#if (_MSC_VER < 1400)
#	define RW_NO_OVERLOAD_WCHAR 1
#endif

/* #define RW_NO_LOCALE 1 */
/* #define RW_NO_CLOCK 1 */
/* #define RW_NO_LEADING_UNDERSCORE 1 */
/* #define RW_NO_GLOBAL_TZ 1 */

#define RW_NO_GETTIMEOFDAY 1

/* #define RW_STRUCT_TM_TZ 1 */

#define RW_NO_XDR 1

/* #define RW_RPC_TYPES_BUG 1 */
/* #define RW_NO_XDR_OP_PROTOTYPES 1 */
/* #define RW_NO_XDR_OP_SCOPING 1 */
/* #define RW_SUPPLY_WSTR 1 */
/* #define RW_NO_WSTR 1 */

#define RW_NO_WCSXFRM 1

/* #define RW_WSTR_C_HEADERS 1 */
/* #define RW_USE_WCHEADERS 1 */

#define RW_NO_STD_NAMESPACE 1
#define RW_NO_IO_SENTRY 1
/*sih #define RW_NO_IOSTD 1 */

/* #define RW_RWV12_STDLIB 1 */
/* #define RW_RWV2X_STDLIB 1 */
#define RW_NO_STL 1
#if (_MSC_VER >= 1200)
#define RW_BCB_NEW_IOSTREAMS 1 /*define this to get new iostreams SIH */
#else
#define RW_NO_IOSTD 1
#endif

/* #define RWSTDLIBTYPE 1 */
/* #define RW_NAMESPACE_REQUIRED 1 */
/* #define RW_NAMESPACE_RELOPS_DEFINED 1 */
/* #define RW_STD_RELOPS_IN_NAMESPACE 1 */

/* #define RW_BOOL_NEEDS_IO 1 */

/* #define RW_INCL_IOSTREAM 1 */
/* #define RW_SOCKET_SYSCALLS_USE_SIZE_T 1 */

/* #define RW_IOS_XALLOC_BROKEN 1 */

/* #define RW_NON_ANSI_HEADERS 1 */
/* #define RW_NO_NEW_HEADER 1 */
#endif /* RW_NEVER_ENTER_DEFAULT_HEADER_BLOCK */
/* The next part of this file, using pre-defined macros provided by
 * your compiler, defines some macros for compiler limitations or
 * vagaries that are hard or impossible to test. You may decide that
 * your version of a given compiler no longer suffers from a
 * particular limitation. If so, feel free to change the macro. Be
 * aware, however, that leaving hese macros defined will usually not
 * reduce the general utility of our libraries, although they may
 * cause us to generate code that is less than perfectly optimal.
 */

/*
** hand coded macros for compilers that define a useful macro "name"
*/
/*************************** Apple's MrCpp  *******************************/
#if defined( __MRC__ )
/* MPW hosted, MPW PowerPC MrCpp C/C++ compiler, Apple's standard libraries */
#  define RW_NO_BOOL                 1
#  define RW_NO_POSIX_RE             1
#  define RW_NO_GLOBAL_TZ            1
#  define RW_NO_GETTIMEOFDAY         1
#  define RW_NO_XDR                  1
#  define RW_FRIEND_THEN_INLINE      1
#  define RW_NO_OVERLOAD_WCHAR       1
#  define RW_NO_IO_SENTRY            1
#  define RW_NO_PUBSEEKOFF           1
#  define RW_NO_WSTR                 1
#  define RW_NO_XMSG                 1
#  define RW_NOT_POSIX_FSTAT         1
#  define RW_CRLF_CONVENTION         1
#  define RW_DIRECTORY_WO_SLASHES    1
#  define RW_COMPILE_INSTANTIATE     1
#  define RW_NOT_TIME_SINCE_1_1_70   1
#  define RW_NO_LOCALE               1
#  define RW_NO_POSIX_RE             1
#  define RW_NO_STL                  1
#  define RW_NO_EXCEPTIONS           1
#  define RW_UNLINK_NON_CONST        1
#  define RW_NO_STRM_GCOUNT          1
#  define RW_REVERSED_CR_AND_LF      1
#  define RW_IOS_XALLOC_BROKEN       1
#  include "rw/fseek.h"       /* bring in patch code for fseek() */
#  define RW_BAD_STATIC_ACCESS       1
#endif  /* MrCpp */

/*************************** MetroWerks *******************************/
#if defined( __MWERKS__ )
#  define RW_NO_POSIX_RE                        1
#  define RW_NO_BOOL                            1
#  if defined( macintosh )
#    define RW_NO_GLOBAL_TZ                     1
#    define RW_NO_GETTIMEOFDAY                  1
#    define RW_NO_XDR                           1
#    define RW_FRIEND_THEN_INLINE               1
#    define RW_NO_OVERLOAD_WCHAR                1
#    define RW_NO_WSTR                          1
#    define RW_NO_XMSG                          1
#    define RW_NO_IO_SENTRY                     1
#    define NO_STREAMBUF_STOSSC                 1
#    define RW_NO_NATIVE_GENERIC_H              1
#    define RW_NOT_POSIX_FSTAT                  1
#    define RW_CRLF_CONVENTION                  1
#    define RW_DIRECTORY_WO_SLASHES             1
#    define RW_COMPILE_INSTANTIATE              1
#    define RW_NOT_TIME_SINCE_1_1_70            1
#    define RW_BROKEN_TEMPLATES                 1
#    define RW_NO_IOSSTREAM                     1
#    define RW_NO_LOCALE                        1
#    define RW_NO_UNBUFFERED                    1
#    define RW_NO_STL                           1
#    define RW_NO_POSIX_RE                      1
#    define RW_BROKEN_ACCESS_ADJUST             1
#    define RW_NO_CONST_PARAM_OVERLOAD          1
#  endif /* macintosh */
#endif  /* __MWERKS__ */

/*************************** Symantec *******************************/

#if defined(__SC__)
# define RW_NO_POSIX_RE                  1
# define RW_NOT_FULL_LOCALE              1
# define RW_NO_IO_SENTRY                 1
# define RW_NO_PUBSEEKOFF                1
# define RW_NO_BOOL                      1

/* For Macs ************/
#  if defined(macintosh)
#    if defined(__SCpp__)     /* NOTE: this must be defined on MPW cmdLine! */
     /* MPW hosted, Symantec's 68k SCpp compiler, Apple's standard libraries */
#      define RW_REVERSED_CR_AND_LF      1
#      define RW_NO_XDR                  1
#      define RW_NOT_TIME_SINCE_1_1_70   1
#      define RW_DIRECTORY_WO_SLASHES    1
#      define RW_CRLF_CONVENTION         1
#      define RW_NO_GLOBAL_TZ            1
#      define RW_NO_GETTIMEOFDAY         1
#      define RW_FRIEND_THEN_INLINE      1
#      define RW_NO_OVERLOAD_WCHAR       1
#      define RW_NO_WSTR                 1
#      define RW_NO_XMSG                 1
#      define RW_ACCESS_NON_CONST        1
#      define RW_UNLINK_NON_CONST        1
#      define RW_NO_STL                  1
#      define RW_NOT_POSIX_FSTAT         1
#      define RW_NO_EXCEPTIONS           1
#      define RW_COMPILE_INSTANTIATE     1
#      define RW_NO_LOCALE               1
#      define RW_NO_POSIX_RE             1
#      define RW_DIRECTORY_WO_SLASHES    1
#      define RW_HEADER_WITH_EXTENSION   1
#      define RW_NO_STRM_GCOUNT          1
#      include "rw/fseek.h"  /* (Apple Libs) bring in patch code for fseek() */
#      define RW_IOS_XALLOC_BROKEN	      1
#      define RW_USE_NO_AMPERSAND             1

#    else /* __SCpp__ */
    /* SPM hosted, Symantec PowerPC <notag> C++ compiler, Symantec standard libraries */
#      define __PowerCpp__                        1
#      define RW_NO_GLOBAL_TZ                     1
#      define RW_NO_GETTIMEOFDAY                  1
#      define RW_NO_XDR                           1
#      define RW_FRIEND_THEN_INLINE               1
#      define RW_NO_OVERLOAD_WCHAR                1
#      define RW_NO_WSTR                          1
#      define RW_NO_XMSG                          1
#      define RW_ACCESS_NON_CONST                 1
#      define RW_UNLINK_NON_CONST                 1
#      define RW_NO_STL                           1
#      define RW_NOT_POSIX_FSTAT                  1
#      define RW_CRLF_CONVENTION                  1
#      define RW_NO_EXCEPTIONS                    1
#      define RW_COMPILE_INSTANTIATE              1
#      define RW_NO_LOCALE                        1
#      define RW_NO_POSIX_RE                      1
#      define RW_NOT_TIME_SINCE_1_1_70            1
#      define RW_DIRECTORY_WO_SLASHES             1
#      define RW_HEADER_WITH_EXTENSION            1
#      define RW_GLOBAL_BEFORE_CLASS_SCOPE        1
#      define RW_NO_STRM_GCOUNT                   1
#      include "rw/fseek.h"  /* (Apple Libs) bring in patch code for fseek() */
#      define RW_IOS_XALLOC_BROKEN	          1
#      define RW_BAD_STATIC_DESTRUCTORS           1
#      ifndef RW_NO_STL
#        define RW_GLBL_SCP_BUG                   1
#        define RW_GLBL_SCP_BUG2                  1
#        define RW_GLBL_SCP_BUG3                  1
#      endif /* RW_NO_STL */
#      define RW_GLOBAL_BEFORE_CLASS_SCOPE        1
#      define RW_NO_RESOLVE_TEMPL_AMBIGUITY       1
#      define RW_USE_NO_AMPERSAND                 1
#    endif    /* __SCpp__ */

/* For PCs ************/
#  else /* macintosh else PC */
#    define RW_NO_POSIX_RE           1
#    if defined(__NT__)
#     if !defined(__WIN32__)
#      define __WIN32__              1
#     endif             
#    endif
#    define __MSDOS__                1
#    define RW_NO_STL                1
#    define RW_NO_OVERLOAD_WCHAR     1
#    define RW_NO_FRIEND_INLINE_DECL 1
#    define RW_COMPILE_INSTANTIATE   1
#    define RW_NO_WSTR               1
#    define RW_NO_XDR                1
#    define RW_NO_XMSG               1
#    define RW_NOT_TIME_SINCE_1_1_70 1
#    define RW_TIME_SINCE_1_1_68     1
#    define RW_NO_TYPENAME           1
#    if defined(_RWTOOLSDLL)
#      define RW_DONT_USE_MEMPOOL    1
#    endif
#  endif /*mac else PC */
#endif /*Symantec*/

/********************** Borland **************************/
#if defined(__TURBOC__)
#  define RW_TURN_OFF_BORLAND_WARNINGS  1
#  define RW_NO_XDR                     1
#  define RW_NO_POSIX_RE                1
#  define RW_NO_XMSG                    1
#  define RW_COMPILE_INSTANTIATE        1
#  define RW_NO_IO_SENTRY               1
#  define RW_NO_PUBSEEKOFF              1
#  define RW_NO_TYPENAME                1
#  define RW_NOT_FULL_LOCALE            1
#  define RW_NO_BOOL                    1
#  if defined(__MSDOS__) && defined(_Windows)
#    define __WIN16__                   1
#  endif
#  if defined(__MSDOS__)
#    define RW_NO_STL                   1
#  endif
#  if defined(__OS2__)
#     define RW_NO_TEMPLATE_REF_INSTANTIATED  1
#     define RW_NO_STL                    1
#     define RW_WCTYPEH_NEEDED            1
#  endif
/* __export in a function declaration must follow the return type */
#  define RW_TRAILING_RWEXPORT            1
/* Borland won't inline code that contains "while" or "for" loops or
 * that generates temporaries requiring destructors
 */
#  define RW_NO_INLINED_WHILES            1
#  define RW_NO_INLINED_FORS              1
#  define RW_NO_INLINED_TEMP_DESTRUCTORS  1

#  ifndef name2
#    define name2 _Paste2
#  endif

#  if __TURBOC__ <= 0x0295                      /* Turbo C++ V 1.00 */
#    define RW_UNDEFINED_REFERENCE_BUG     1
#    define RW_NO_STL 1
#  endif
#  if __TURBOC__ >=0x200        /* Turbo C++ V1.01 and above */
#    if __TURBOC__ < 0x300 /* Borland V2.0 */
#      define RW_BCC_INLINE_DESTRUCTOR_BUG 1
#      define RW_BCC_STRUCT_POINTER_BUG    1
#      define RW_TCC_DELETE_SIZE_BUG       1
#    endif      /* end Borland C++ V2.0 */
#    if __TURBOC__ < 0x400
#      define RW_NO_TEMPLATES              1
#    endif
#    if __TURBOC__ < 0x451                      /* prior to V 4.0 */
#     define RW_NO_EXCEPTIONS              1
#     define RW_NO_FRIEND_INLINE_DECL      1
#     define RW_NO_OVERLOAD_WCHAR          1
#     define RW_NO_OVERLOAD_SCHAR          1
#     define RW_NO_STL                     1
#    endif
#    if __BORLANDC__ < 0x453                    /* Borland 4.0 */
     /* The following are provided, but are not locale-correct */
#      undef RW_NO_STRICMP
#      undef RW_NO_STRNICMP
#      define RW_NO_STRICMP                1
#      define RW_NO_STRNICMP               1
#    endif
#    if __TURBOC__ > 0x459
#      if defined(__WIN32__)
#        define RW_DOUBLE_FLUSH_ON_CLOSE   1
#      endif
#      define RW_DOUBLE_UNDERBAR_POINTER_QUALIFIERS 1
#    endif
#    if __TURBOC__ < 0x461
#      define RW_NO_LEADING_UNDERSCORE      1
#      define RW_NO_FRIEND_INLINE_TMPL_DECL 1
#    endif
#    ifdef __MT__
#      ifndef RW_MULTI_THREAD
#        define RW_MULTI_THREAD            1
#      endif
#    endif
#    ifdef __WIN32__
#       define RW_SUPPLY_WSTR              1
#       define RW_NO_WCSXFRM               1
#    else
#       define RW_NO_WSTR                  1
#       undef RW_NO_STL
#       define RW_NO_STL                   1
#    endif
#    if __TURBOC__ >= 0x500
#      define RW_RWV12_STDLIB              1
#      define RW_NO_EXPRESSION_TEMPLATES   1
#      define RW_NO_TZSET_UNDERSCORE       1
#      define RW_GLOBAL_BEFORE_CLASS_SCOPE 1
#    endif
#    if __TURBOC__ >= 0x502
#      define RW_NO_INLINE_CLASS_BY_VALUE  1
#      define RW_ASSIGN_EXCEPTION_BUG      1
#      define RW_UNSIGNED_CHAR_TO_INT_BUG  1
#      undef RW_NO_BOOL
#      ifndef RW_NO_STL         
#        define RW_GLBL_SCP_BUG2           1
#        define RW_GLBL_SCP_BUG4           1
#      endif
#    endif
#    if __TURBOC__ >= 0x530
#      define RW_NO_NATIVE_GENERIC_H       1
#      define RW_RWSTLV201B_NAME2_BUG      1
#      define RW_RWV2X_STDLIB              1
#      define RW_3ARG_DISTANCE             1
#      ifndef RW_NO_STL                        
#        define RW_NAMESPACE_REQUIRED      1 
#        define RW_ALLOC_INTERFACE_STLV2X_HACK    1
#        define RW_ITER_NO_TAGS            1
#        define RWSTD_NO_CONST_INST        1
#      endif                                   
#      undef RW_RWV12_STDLIB                
#      undef RW_NO_IO_SENTRY
#      undef RW_SUPPLY_WSTR
#    endif
#  endif
#endif  /* __TURBOC__ */
/************************ Microsoft C/C++ *****************************/
#if defined(_MSC_VER)
#  if !defined(RW_RWV12_STDLIB) && !defined(RW_NO_STL)
#    define RW_MSVC_STDLIB	0x0620 /* version 4.2 from fall 1996 */
#  endif
#  define RW_NO_XDR                        1
#  define RW_NO_POSIX_RE                   1
#  define RW_NO_REF_TO_REF                 1
#  define RW_NO_XMSG                       1
#  define RW_MSC_BACKEND                   1
#  define RW_NO_IO_SENTRY                  1
/*#  define RW_NO_PUBSEEKOFF                 1 */
#  define RW_NOT_FULL_LOCALE               1
#  if (_MSC_VER < 1100) || defined(RW_NO_STL)
/*#    define RW_NO_BOOL                     1*/
#  endif
#  if defined(_MSDOS) && !defined(WIN32) && !defined(_WIN32)
#    define __MSDOS__                      1
#    if defined(_WINDOWS)
#      define __WIN16__                    1
#    endif
#  endif
#  if defined(WIN32) || defined(_WIN32)
#   if !defined(__WIN32__)
#    define __WIN32__
#   endif 
#    define RW_TOLOWER_SIGN_EXTENDS_RESULT_BUG 1
#  else
#    define RW_NO_WSTR                     1
#  endif
#  if (_MSC_VER < 900)
#    define RW_NO_STL                      1
#    define RW_MSC_BACKEND                 1
#    define RW_NO_EXCEPTIONS               1
#    define RW_NO_OVERLOAD_WCHAR           1
#    define RW_NO_TEMPLATES                1
#    define RW_NO_TYPENAME                 1
#  else
#    if (_MSC_VER < 1000)
#      define RW_NO_STL                    1
#      define RW_MSC_BACKEND               1
#      define RW_COMPILE_INSTANTIATE       1
#      define RW_NO_OVERLOAD_WCHAR         1
#      define RW_NO_TYPENAME               1
#    else
#      define RW_MSC_BACKEND               1
#	   if (_MSC_VER <1400)
#        define RW_NO_OVERLOAD_WCHAR         1
#      endif
#      define RW_COMPILE_INSTANTIATE       1
#      if (_MSC_VER < 1100)
#        define RW_GLOBAL_BEFORE_CLASS_SCOPE 1
#      endif
#      define RW_SPECIFIC_BASEBASE_ACCESS  1
   /*  MSVC 4.1 generates *NO* code for some adjusted access methods */
#      define RW_NO_ACCESS_ADJUSTMENT      0
#      ifndef RW_NO_STL
#        undef RWSTD_NO_DESTROY_NONBUILTIN
#      endif
#    endif
#    if (_MSC_VER >= 1020)
#      define RW_NO_TYPEDEF_ITERATOR       1
#      define RW_REF_SEEMS_PTR             1
#      define RW_NO_PTR_PAIR_CTOR          1
#      define RW_NO_DEFAULT_TEMPLATE_ARGS  1
#      define RW_BROKEN_NAME_RESOLUTION    1
#      if defined(RW_RWV12_STDLIB) || defined(RW_NO_STL)
/*#        define RW_STREAMOK void* */
#        define RW_NO_UNBUFFERED           1
#      else
#        define RW_NO_UNBUFFERED           1
#      endif
#      if defined(RW_MSVC_STDLIB) && (RW_MSVC_STDLIB == 0x0420)
#        define RW_SORT_LIST_BACKWARD      1
#        define RW_SPUTN_BROKEN_HEXFF      1
#        if defined(_RWTOOLSDLL)
#          define RW_MSVC_DLL_LOCALE_FACET 1
#        endif
#      endif
#    endif
#    if (_MSC_VER >= 1100)
#	   define RW_NAMESPACE_REQUIRED        1
#      define RW_BROKEN_INITIALIZER        1
#      define RW_BROKEN_NAME_RESOLUTION    1
#      define RW_NO_INHERIT_TEMPL_ACCESS   1
#      define RW_NO_INHERIT_TEMPL_TYPEDEF  1 
#      define RW_NO_DEFAULT_TEMPLATE_ARGS  1
#	   define RW_NO_LOCALTIME_R 1
#      undef  RW_SORT_LIST_BACKWARD
#      if (_MSC_VER < 1200) && defined(RW_NO_IOSTD)
# 	    define RW_NO_PUBSEEKOFF 1
#        define RW_NO_TYPENAME             1
#      endif
#    endif
#    if (_MSC_VER >= 1200)
#      undef  RW_NO_INHERIT_TEMPL_ACCESS
#      undef  RW_NO_DEFAULT_TEMPLATE_ARGS
#      undef  RW_NO_REF_TO_REF
#      define RW_MEMBER_OPERATOR_SYNTAX_PROBLEM 1
#	   define RW_MSVC_DLL_LOCALE_FACET_X 1
#    endif
#  endif
#  ifdef _RWBUILDDLL
#    define __DLL__         1
#  endif
#  ifdef _M_I86SM
#    define __SMALL__       1
#  endif
#  ifdef _M_I86CM
#    define __COMPACT__     1
#  endif
#  ifdef _M_I86MM
#    define __MEDIUM__      1
#  endif
#  ifdef _M_I86LM
#    define __LARGE__       1
#  endif
#  ifdef _MT
#    define RW_MULTI_THREAD 1
#  endif
#endif
/************************** WATCOM C/C++ ******************************/
#ifdef __WATCOMC__
#  define RW_NO_XDR                1
#  define RW_NO_IO_SENTRY          1
#  define RW_NO_PUBSEEKOFF         1
#  define RW_NO_POSIX_RE           1
#  define RW_NO_STL                1
#  define RW_NO_XMSG               1
#  define RW_NO_TEMPLINST_ON_BASE  1
#  define RW_NO_WSTR               1
#  define RW_NO_LEADING_UNDERSCORE 1
#  define RW_NO_NESTED_QUOTES      1
#  define RW_COMPILE_INSTANTIATE   1
#  define RW_NO_TYPENAME           1
#  define RW_NOT_FULL_LOCALE       1
#  define RW_NO_BOOL               1
/* Watcom cannot always handle high precision (RW_DEFAULT_PRECISION) */
#  define RW_DEFAULT_PRECISION 10
#  if defined(__DOS__) && !defined(__MSDOS__)
#    define __MSDOS__        1
#  endif
#  if defined(__WINDOWS__) || defined(__NT__)
#    if defined(__WINDOWS_386__) || defined(__NT__)
#     if !defined(__WIN32__)
#      define __WIN32__      1
#     endif
#    else
#      define __WIN16__      1
#    endif
#  endif
#endif
/********************** Metaware High C/C++ ***************************/
#if defined(__HIGHC__)
#  define RW_NO_XDR              1
#  define RW_NO_POSIX_RE         1
#  define RW_NO_STL              1
#  define RW_NO_OVERLOAD_WCHAR   1
#  define RW_NO_IO_SENTRY        1
#  define RW_NO_PUBSEEKOFF       1
#  define RW_NO_XMSG             1
#  define RW_NO_TYPENAME         1
#  define RW_NOT_FULL_LOCALE     1
#  define RW_NO_BOOL             1
#  if defined(_MSDOS)
#    define __MSDOS__            1
#    define RW_HIGHC_INLINE_BUG  1
#    define RW_NO_EXCEPTIONS     1
#    define RW_NO_WSTR           1
#  endif
#  if defined(_OS2)
#    undef __OS2__
#    define __OS2__              1
#    define RW_SUPPLY_WSTR       1
#    define RW_NO_WCSXFRM        1
#    ifdef _REENTRANT
#      define RW_MULTI_THREAD    1
#    endif
#  endif
#  if defined(_MSNT)
#    undef __WIN32__
#    define __WIN32__            1
#    if defined(_REENTRANT) || defined(_MT)
#      define RW_MULTI_THREAD    1
#    endif
#  endif
#endif  /* __HIGHC__ */

/********************** IBM C++ *********************************/
#if defined(__IBMCPP__) && !defined(_AIX)
#  define RW_IOS_XALLOC_BROKEN   1
#  define RW_NO_POSIX_RE         1
#  define RW_NO_XMSG             1
#  define RW_NO_XDR              1
#  define RW_NO_IO_SENTRY        1
#  define RW_NO_PUBSEEKOFF       1
#  define RW_NO_TYPENAME         1
#  define RW_COMPILE_INSTANTIATE 1
#  define RW_NOT_FULL_LOCALE     1
#  define RW_WCTYPEH_NEEDED      1
#  define RW_NO_BOOL             1
#  ifdef __MULTI__
#    define RW_MULTI_THREAD      1
#  endif
#  ifdef _WIN32
#   if !defined(__WIN32__)
#    define __WIN32__            1
#   endif
#    define RW_NO_STL            1
#  else
#    define RW_RWV12_STDLIB      1
#    define RW_BROKEN_TEMPLATE_TYPEDEFS 1
#  endif
#endif

#if defined(__IBMCPP__) && defined(_AIX)
#  define RW_NO_DEFAULT_TEMPLATE_ARGS  1
#endif

/********************** ObjectStore ***************************/
#ifdef RW_USE_OBJECTSTORE_PSE
#  ifndef RW_USE_OBJECTSTORE
#    define RW_USE_OBJECTSTORE   1
#  endif
#endif

#if defined(_ODI_OSSG_)
#  ifdef RW_COMPILE_INSTANTIATE
#    undef RW_COMPILE_INSTANTIATE
#  endif
#endif

/******** Turn of C++ exceptions by default with Objectstore ******/
#if defined(RW_USE_OBJECTSTORE) && !defined(_OS_CPP_EXCEPTIONS)
#  ifndef RW_NO_EXCEPTIONS
#    define RW_NO_EXCEPTIONS     1
#  endif
#endif

#ifdef RW_USE_OBJECTSTORE
#  ifndef RW_NO_STL
#    define RW_NO_STL            1
#  endif
#endif

#if defined(__SUNPRO_CC)
#define RW_NO_DEFAULT_TEMPLATE_ARGS 1
#endif 

#if defined(__sgi)

#   define RW_NO_DEFAULT_TEMPLATE_ARGS 1

#   if !defined(RW_NO_STL) && defined(RW_RWV12_STDLIB) 
#include <stdcomp.h>
#     if !defined(RWSTD_NO_NAMESPACE)
#       define RW_NAMESPACE_REQUIRED	1
#     endif
#   endif

#endif 

#if defined(_AIX)
#define RW_NO_DEFAULT_TEMPLATE_ARGS 1
#endif 

#ifndef HP_ANSI
#  if defined(__hpux) && defined(__cplusplus) && __cplusplus >= 199707
#    define HP_ANSI
#  endif
#endif

#ifdef HP_ANSI
#    define RW_AWFUL_OCAST(x) *(RWvostream *)(void*) & x
#    define RW_AWFUL_ICAST(x) *(RWvistream *)(void*) & x
#    define RW_DONT_USE_MEMPOOL 1
#    define RW_NO_DEFAULT_TEMPLATE_ARGS 1
#else
#    define RW_AWFUL_OCAST(x) (RWvostream &) x
#    define RW_AWFUL_ICAST(x) (RWvistream &) x
#endif

/********************* misc adjustments, extras *********************/
/**** YOU SHOULD NOT NEED TO EVER TOUCH ANYTHING BELOW THIS LINE ****/
/********************************************************************/
/* No way to test Glockenspiel for this bug */
#if defined(__GLOCK__)
#  define RW_CONST_EMIT_BUG 1
#endif

/* some IBM xlC don't #define unix */
#if defined(_AIX) && !defined(unix)
#define unix 1
#endif

/* No Pi for these compilers: */
#if defined(RW_MSC_BACKEND) || defined(__OREGON__) || defined(__HIGHC__) || defined(applec) || defined(CII) || defined(__WATCOMC__)
#  ifndef M_PI
#    define M_PI 3.14159265358979323846
#  endif
#endif

#ifdef RW_GLOBAL_ENUMS
#  define RWSCOPE(a)
#  define RWVECTOR_DELETE(i) delete[i]
#else
#  define RWSCOPE(a) a::
#  define RWVECTOR_DELETE(i) delete[]
#endif

#ifdef RW_WRAP_C
#  define STARTWRAP     extern "C" {
#  define ENDWRAP       }
#else
#  define STARTWRAP
#  define ENDWRAP
#endif

#if defined(RW_NO_TYPENAME)
#  define typename
#endif

#if defined(HP_ANSI) && !defined(RW_NO_STL)
#  define RW_RWV12_STDLIB     1
#endif

#if defined(RW_NO_BOOL) && !defined(RW_NO_STL)
#  undef  RWSTD_NO_RWBOOLEAN
#  define RWSTD_NO_RWBOOLEAN
#endif

/* allocator interfaces. So the same code in rw/stdex works (always :) */
/* RW_ALLOC_TYPE(Type2,Allocator,Type2Allocator,Count,pointerHint)     */
/*   Because these are macros, always pass the hint (use 0 by default) */
/* RW_DEALLOC_TYPE(Allocator,Type2Allocator,Pointer)                   */
/* otherAllocator is a RWV12 hack. _ALLOCATE_X is a MSVC hack.         */
/* Other macros may go here as we port to other "standard" libs        */
#ifdef RW_MSVC_STDLIB
#  define RW_ALLOCATOR_NO_REBIND
#  if defined(_MSC_VER) && (_MSC_VER < 1100)
#     define RW_ALLOC_TYPE(T,A,oA,N,H) _ALLOCATE_X(T,oA,A,N)
#     define RW_DEALLOC_TYPE(A,oA,P,N)   _DEALLOCATE_X(oA,A,P)
#  else
#     define RW_ALLOC_TYPE(T,A,oA,N,H)   oA.allocate(N,H)
#     define RW_DEALLOC_TYPE(A,oA,P,N)     oA.deallocate(P,N)
#  endif
#endif

#ifdef RW_RWV12_STDLIB
#  define RW_ALLOC_TYPE(T,A,oA,N,H)   oA.allocate(N,H)
#  define RW_DEALLOC_TYPE(A,oA,P,N)     oA.deallocate(P)
#endif

#ifdef RW_RWV2X_STDLIB                                    
#  define RW_ALLOC_TYPE(T,A,oA,N,H)   oA.allocate(N,H)    
#  define RW_DEALLOC_TYPE(A,oA,P,N)     oA.deallocate(P,N)
#endif                                                    

/* miscellaneous standard library adaptations */
#ifdef RW_RWV12_STDLIB
#  define RW_ALLOCATOR_PLAIN      1 
#  define RW_ALLOC_INTERFACE_HACK 1
#  define RW_STLCTOR_PTR_HACK     1
#  define RW_3ARG_DISTANCE        1
#  define RW_ITER_NO_TAGS         1
#endif

#ifdef RW_MSVC_STDLIB
#  undef  RW_NO_PUBSEEKOFF
#  define RW_STREAMOK bool
#endif

#if defined(RW_RWV12_STDLIB) || defined(RW_NO_STL)
/*#  define RW_NO_PUBSEEKOFF        1*/
/*#  define RW_NO_IOSTD             1*/
#endif

#if defined(RW_RWV2X_STDLIB)
#  undef RW_NO_PUBSEEKOFF
#  undef RW_NO_IOSTD
#  define RW_NO_STREAMBUF_STOSSC  1
#  define RW_NO_UNBUFFERED        1
#endif

//SIV. 2019-03-27. It looks like the most recent STD does not implement basic_streambuf::stossc(). See 
//https://docs.microsoft.com/en-us/cpp/standard-library/basic-streambuf-class?f1url=https%3A%2F%2Fmsdn.microsoft.com%2Fquery%2Fdev15.query%3FappId%3DDev15IDEF1%26l%3DEN-US%26k%3Dk(STREAMBUF%2Fstd%3A%3Abasic_streambuf%3A%3Astossc)%3Bk(std%3A%3Abasic_streambuf%3A%3Astossc)%3Bk(stossc)%3Bk(SolutionItemsProject)%3Bk(DevLang-C%2B%2B)%3Bk(TargetOS-Windows)%26rd%3Dtrue&view=vs-2017#stossc
//We use basic_streambuf::sbumpc() instead:
#define RW_NO_STREAMBUF_STOSSC  1

// the following takes care of a name collision between MSVC's
//  iostreams' iomanipulators and vstreams' flush
#if defined(_MSC_VER) && defined(RW_NAMESPACE_REQUIRED)
#   if !defined(RW_NO_IOSTD)
//#		define RW_NAMESPACE_COLLISION_MSVC_BUG 1
#   endif
#endif

#if !defined(RW_STREAMOK)
#  define RW_STREAMOK void* 
#endif

#if defined(__DECCXX) 
#  if !defined(RW_NO_PUBSEEKOFF)
#    define RW_NO_PUBSEEKOFF        1
#  endif
#  define RW_NO_DEFAULT_TEMPLATE_ARGS 1
#  if !defined(RW_NO_STL) && !defined(RW_RWV12_STDLIB)
#    define RW_ALLOC_TYPE(T,A,oA,N,H)   oA.allocate(N,H)    
#    define RW_DEALLOC_TYPE(A,oA,P,N)     oA.deallocate(P,N)
#  endif
#endif

#if defined(_ATT1__) || defined(__ATT2__) || defined(__ATT3__)
#  define __ATT__ 1
#endif

#if defined(__MSDOS__) || defined(__OS2__) || defined(__WIN32__) || defined(__NT__) || defined(__WINDOWS__)
#  define RW_CRLF_CONVENTION 1
#endif

#ifndef RW_BYTES_PER_PTR
# if defined(__SMALL__) || defined(__MEDIUM__)
#   define RW_BYTES_PER_PTR 2
# else
#   define RW_BYTES_PER_PTR 4   /* Assume 32 bit pointers */
# endif
#endif
#ifndef RW_BYTES_PER_WORD
# if defined(__SMALL__) || defined(__COMPACT__) || defined(__MEDIUM__) || defined(__LARGE__)
#   define RW_BYTES_PER_WORD 2
# else
#   define RW_BYTES_PER_WORD 4  /* Assume 32 bit words */
# endif
#endif

#ifndef RW_DEFAULT_PRECISION
#   define RW_DEFAULT_PRECISION 16      /* Assume standard IEEE format */
#endif

/** safety check **/
#if defined(RW_NO_THREADS) && defined(RW_MULTI_THREAD)
#  error cannot have RW_MULTI_THREAD with RW_NO_THREADS defined
#endif

/** Must use MT-safe version of stdlib with MT-safe Tools.h++ **/
#if defined(RW_MULTI_THREAD) && !defined(RWSTD_MULTI_THREAD)
#define RWSTD_MULTI_THREAD 1
#endif

#endif /* __RWCOMPILER_H__ */
