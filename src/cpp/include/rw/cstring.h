#ifndef __RWCSTRING_H__
#define __RWCSTRING_H__
/*
 * Declarations for RWCString --- C strings
 *
 * $Id: cstring.h 18623 2000-03-03 08:28:13Z sih $
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

#include "rw/tooldefs.h"
#include "rw/ref.h"
#include "rw/rstream.h"

//#2437, 2896,2899, 4812: C++ Standard templated operators <= >= >, !=
#if !defined(RW_NO_STL) && defined(RW_NO_RELOPS_NAMESPACE)
# if !defined(__SUNPRO_CC) || __SUNPRO_CC < 0x420
// SunPro 4.20 confuses the global operator=='s with template specializations.
# if  !defined(__TURBOC__) 
// bcc V4.52 and under does not cope with <utility>
#   define RW_USES_STD_OPERATORS /* for later use in this file only */
#   ifndef RW_HEADER_WITH_EXTENSION
#     include <utility>
#   else
#     include <utility.hpp>
#   endif  /* RW_HEADER_WITH_EXTENSION */
# endif  /* !TURBOC */
# endif  /* !SUNPRO */
#endif   /* !RW_NO_STL && RW_NO_RELOPS_NAMESPACE */


STARTWRAP
#include <string.h>
ENDWRAP

class RWExport RWCRegexp;

#if !defined(RW_NO_STL)
template<class charT> class RWTExport RWTRegularExpression;
typedef RWTRegularExpression<char> RWCRExpr;
#endif

class RWSExport RWCString;
class RWExport RWCSubString;

#ifdef RW_DECLARE_GLOBAL_FRIENDS_FIRST
inline RWBoolean rwexport operator==(const RWCString &s1, const RWCString &s2);
#endif

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             RWCStringRef                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

/*
 * This is the dynamically allocated part of a RWCString.
 * It maintains a reference count.
 * There are no public member functions.
 */

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWCStringRef : public RWReference
{

  static RWCStringRef*  getRep(size_t capac, size_t nchar, void *where);
  static RWCStringRef*  getRep(size_t capac, size_t nchar);

  void          unLink(); // disconnect from a stringref, maybe delete it

  size_t        length   () const {return nchars_;}
  size_t        capacity () const {return capacity_;}
  char*         data     () const {return (char*)(this+1);}

  char&         operator[](size_t i)       {return ((char*)(this+1))[i];}
  char          operator[](size_t i) const {return ((char*)(this+1))[i];}

  size_t        first    (char       ) const;
  size_t        first    (char,size_t) const;
  size_t        first    (const char*) const;
  size_t        first    (const char*, size_t) const;
  unsigned      hash     (           ) const;
  unsigned      hashFoldCase (       ) const;
  size_t        last     (char       ) const;
  size_t        last     (char,size_t) const;

#ifndef RW_NO_LOCALE
  int           collate(const char*) const;
#endif

  size_t        capacity_;      // Max string length (excluding null)
  size_t        nchars_;        // String length (excluding terminating null)

friend class RWSExport RWCString;
friend class RWExport RWCSubString;
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             RWCSubString                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

/*
 * The RWCSubString class allows selected elements to be addressed.
 * There are no public constructors.
 */

class RWExport RWCSubString
{
public:
  RWCSubString(const RWCSubString& sp)
         : str_(sp.str_), begin_(sp.begin_), extent_(sp.extent_) {;}

  RWCSubString& operator=(const char*);         // Assignment to char*
  RWCSubString& operator=(const RWCString&);    // Assignment to RWCString
  RWCSubString& operator=(const RWCSubString&); // Assignment to RWCSubString
  char&         operator()(size_t i);           // Index with optional bounds checking
  char&         operator[](size_t i);           // Index with bounds checking
#ifndef RW_NO_CONST_OVERLOAD
  char          operator()(size_t i) const;     // Index with optional bounds checking
  char          operator[](size_t i) const;     // Index with bounds checking
#endif
  const char*   startData() const;      // Replaces data(). See definition below.
  // startData() will remain undocumented.  Please don't even ask.
  // Use at your own risk. It may be deprecated in the future.

  // DON'T USE THE FUNCTION BELOW!
  const char*   data() const;
  // This member is deprecated and will be removed in a future version.
  // It remains public only to maintain source compatibility.
  // Since RWCSubString works by referencing the RWCString's data,
  // if you attempt to directly use the data() member of the RWCString,
  // you will very likely be surprised by the result, which will be null
  // terminated not at the extent of the substring,
  // but at the end of the RWCString.

  size_t        length() const          {return extent_;}
  size_t        start() const           {return begin_;}
  void          toLower();              // Convert self to lower-case
  void          toUpper();              // Convert self to upper-case

  // For detecting null substrings:
  RWBoolean     isNull() const          {return begin_==RW_NPOS;}
  int           operator!() const       {return begin_==RW_NPOS;}

protected:

  void          subStringError(size_t, size_t, size_t) const;
  void          assertElement(size_t i) const;  // Verifies i is valid index

private:

  // NB: the only constructor is private:
  RWCSubString(const RWCString & s, size_t start, size_t len);

  RWCString*    str_;           // Referenced string
  size_t              begin_;   // Index of starting character
  size_t              extent_;// Length of RWCSubString



#ifdef RW_TRAILING_RWEXPORT

friend 
RWBoolean rwexport operator==(const RWCSubString& s1, const RWCSubString& s2);
friend 
RWBoolean rwexport operator==(const RWCSubString& s1, const RWCString& s2);
friend 
RWBoolean rwexport operator==(const RWCSubString& s1, const char* s2);

#else

friend rwexport
RWBoolean operator==(const RWCSubString& s1, const RWCSubString& s2);
friend rwexport
RWBoolean operator==(const RWCSubString& s1, const RWCString& s2);
friend rwexport
RWBoolean operator==(const RWCSubString& s1, const char* s2);

#endif  /* RW_TRAILING_RWEXPORT */

friend class RWSExport RWCString;
  
  RW_TYPESPEC  /* This macro usually expands to nothing */
};

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                              RWCString                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class RWSExport RWCString
{
public:

  enum stripType    {leading = 0x1, trailing = 0x2, both = 0x3};
  enum caseCompare  {exact, ignoreCase};
  enum scopeType    {one, all};

  RWCString();                   // Null string
  RWCString(RWSize_T ic);        // Suggested capacity
  RWCString(const RWCString& S); // Copy constructor
  RWCString(const char * a);            // Copy to embedded null
  RWCString(const char * a, size_t N);  // Copy past any embedded nulls
  RWCString(char c) {initChar(c);} 

#ifndef RW_NO_OVERLOAD_UCHAR
  RWCString(unsigned char c) {initChar(char(c));}
#endif
#ifndef RW_NO_OVERLOAD_SCHAR
  RWCString(signed char c) {initChar(char(c));}
#endif

  RWCString(char, size_t N);
  
  RWCString(const RWCSubString& SS);

  ~RWCString();

  // Type conversion:
#ifndef RW_ZTC_TYPE_CONVERSION_BUG
                operator const char*() const {return data_;}
#endif

  // Assignment:
  RWCString&    operator=(const char*);         // Replace string
  RWCString&    operator=(const RWCString&);    // Replace string
  RWCString&    operator+=(const char*);        // Append string.
  RWCString&    operator+=(const RWCString& s);


  // Indexing operators:
  char&         operator[](size_t);             // Indexing with bounds checking
  char&         operator()(size_t);             // Indexing with optional bounds checking
  RWCSubString  operator()(size_t start, size_t len);           // Sub-string operator
  RWCSubString  operator()(const RWCRegexp& re);                            // Match the RE
  RWCSubString  operator()(const RWCRegexp& re, size_t start);  // Match the RE
# if !defined(RW_NO_STL)
  RWCSubString  match(const RWCRExpr& re);                                // Match the RE
  RWCSubString  match(const RWCRExpr& re, size_t start);              // Match the RE
# endif
  RWCSubString  subString(const char* pat, size_t start=0, caseCompare=exact);
#ifndef RW_NO_CONST_OVERLOAD
  char          operator[](size_t) const;
  char          operator()(size_t) const;
  RWCSubString    operator()(size_t start, size_t len) const;
  RWCSubString    operator()(const RWCRegexp& pat) const;   // Match the RE
  RWCSubString    operator()(const RWCRegexp& pat, size_t start) const;   // Match the RE
# if !defined(RW_NO_STL)
  RWCSubString    match(const RWCRExpr& pat) const; // Match the RE
  RWCSubString    match(const RWCRExpr& pat, size_t start) const; // Match the RE
# endif
  RWCSubString    subString(const char* pat, size_t start=0, caseCompare=exact) const;
  RWCSubString    strip(stripType s=trailing, char c=' ') const;
#endif

                // Non-static member functions:
  RWCString&    append(const char* cs);
  RWCString&    append(const char* cs, size_t N);
  RWCString&    append(const RWCString& s);
  RWCString&    append(const RWCString& s, size_t N);
  RWCString&    append(char c, size_t rep=1);   // Append c rep times
  RWspace       binaryStoreSize() const         {return length()+sizeof(size_t);}
  size_t        capacity() const                {return pref()->capacity();}
  size_t        capacity(size_t N);
#ifndef RW_NO_LOCALE
  int           collate(const char* cs) const   {return pref()->collate(cs);}
  int           collate(const RWCString& st) const;
#endif
  int           compareTo(const char* cs,      caseCompare cmp = exact) const;
  int           compareTo(const RWCString& st, caseCompare cmp = exact) const;
  RWBoolean     contains(const char* pat,      caseCompare cmp = exact) const;
  RWBoolean     contains(const RWCString& pat, caseCompare cmp = exact) const;
  RWCString     copy() const;
  const char*   data() const {return data_;}
  size_t        first(char c) const             {return pref()->first(c);}
  size_t        first(char c, size_t i) const
  { return pref()->first(c, i); }
  size_t        first(const char* cs) const     {return pref()->first(cs);}
  size_t        first(const char* cs, size_t N) const
  { return pref()->first(cs,N); }
  static unsigned   hash(const RWCString&);
  unsigned          hash(caseCompare cmp = exact) const;
  size_t        index(const char* pat, size_t i=0, caseCompare cmp = exact)
                      const;
  size_t        index(const RWCString& s, size_t i=0, caseCompare cmp = exact)
                      const;
  size_t        index(const char* pat, size_t patlen, size_t i,
                      caseCompare cmp) const;
  size_t        index(const RWCString& s, size_t patlen, size_t i,
                      caseCompare cmp) const;
# if !defined(RW_NO_STL)
  size_t        index(const RWCRExpr& pat, size_t i=0) const;
  size_t        index(const RWCRExpr& pat, size_t* ext, size_t i=0) const;

  // returns: index(RWCRExpr(pat), ext, i):
  size_t        index(const char* pat, size_t* ext, size_t i=0) const;
# endif

  size_t        index(const RWCRegexp& pat, size_t i=0) const;
  size_t        index(const RWCRegexp& pat, size_t* ext, size_t i=0) const;
  RWCString&    insert(size_t pos, const char*);
  RWCString&    insert(size_t pos, const char*, size_t extent);
  RWCString&    insert(size_t pos, const RWCString&);
  RWCString&    insert(size_t pos, const RWCString&, size_t extent);
  RWBoolean     isAscii() const;
  RWBoolean     isNull() const                  {return pref()->nchars_ == 0;}
  size_t        last(char c) const              {return pref()->last(c);}
  size_t        last(char c,size_t i) const     {return pref()->last(c,i);}
  size_t        length() const                  {return pref()->nchars_;}
#ifndef RW_NO_LOCALE
  size_t        mbLength() const;       // multibyte length, or RW_NPOS on error
#endif
  RWCString&    prepend(const char*);                   // Prepend a character string
  RWCString&    prepend(const char* cs, size_t N);
  RWCString&    prepend(const RWCString& s);
  RWCString&    prepend(const RWCString& s, size_t N);
  RWCString&    prepend(char c, size_t rep=1);  // Prepend c rep times
  RW_SL_IO_STD(istream)&      readFile(RW_SL_IO_STD(istream)&);                     // Read to EOF or null character.
  RW_SL_IO_STD(istream)&      readLine(RW_SL_IO_STD(istream)&,
                         RWBoolean skipWhite = TRUE);   // Read to EOF or newline.
  RW_SL_IO_STD(istream)&      readString(RW_SL_IO_STD(istream)&);                   // Read to EOF or null character.
  RW_SL_IO_STD(istream)&      readToDelim(RW_SL_IO_STD(istream)&, char delim='\n'); // Read to EOF or delimitor.
  RW_SL_IO_STD(istream)&      readToken(RW_SL_IO_STD(istream)&);                    // Read separated by white space.
  RWCString&    remove(size_t pos);                     // Remove pos to end of string
  RWCString&    remove(size_t pos, size_t n);           // Remove n chars starting at pos
  RWCString&    replace(size_t pos, size_t n, const char*);
  RWCString&    replace(size_t pos, size_t n, const char*, size_t);
  RWCString&    replace(size_t pos, size_t n, const RWCString&);
  RWCString&    replace(size_t pos, size_t n, const RWCString&, size_t);
# if !defined(RW_NO_STL)
  RWCString&  replace
              (
                                         const RWCRExpr& pattern, const char *replacement,
                scopeType scope = one
              );
  RWCString&  replace
              (
                const RWCRExpr& pattern, const RWCString &replacement,
                scopeType scope = one
              );
# endif
  void          resize(size_t N);                       // Truncate or add blanks as necessary.
  void          restoreFrom(RWvistream&);               // Restore from ASCII store
  void          restoreFrom(RWFile&);                   // Restore string
  void          saveOn(RWvostream& s) const;
  void          saveOn(RWFile& f) const;
  RWCSubString  strip(stripType s=trailing, char c=' ');
  void          toLower();                              // Change self to lower-case
  void          toUpper();                              // Change self to upper-case

  // Static member functions:
  static size_t         initialCapacity(size_t ic = 15);        // Initial allocation Capacity
  static size_t         maxWaste(size_t mw = 15);               // Max empty space before reclaim
  static size_t         resizeIncrement(size_t ri = 16);        // Resizing increment
#if defined(_RWTOOLSDLL) && defined(__WIN16__)
  // Just declarations --- static data must be retrieved from the instance manager.
  static size_t         getInitialCapacity();
  static size_t         getResizeIncrement();
  static size_t         getMaxWaste();
#else
  static size_t         getInitialCapacity()    {return initialCapac;}
  static size_t         getResizeIncrement()    {return resizeInc;}
  static size_t         getMaxWaste()           {return freeboard;}
#endif

  // useful for supplying hash functions to template hash collection ctors:  
protected:

  // Special concatenation constructor:
  RWCString(const char* a1, size_t N1, const char* a2, size_t N2);
  void                  assertElement(size_t) const;    // Index in range
  void                  clobber(size_t nc);             // Remove old contents
  void                  cow();                          // Do copy on write as needed
  void                  cow(size_t nc);                 // Do copy on write as needed
  static size_t         adjustCapacity(size_t nc);
  void                  initChar(char);                 // Initialize from char
private:

  void                  clone();          // Make self a distinct copy
  void                  clone(size_t nc); // Make self a distinct copy w. capacity nc

#if !defined(_RWTOOLSDLL) || !defined(__WIN16__)
  /* If not compiling for a shared address space, then use static data */
  static size_t initialCapac;           // Initial allocation Capacity
  static size_t resizeInc;              // Resizing increment
  static size_t freeboard;              // Max empty space before reclaim
#endif

  RWCStringRef* pref() const { return (((RWCStringRef*) data_) - 1); }
  char*         data_;          // ref. counted data (RWCStringRef is in front)

#ifdef RW_TRAILING_RWEXPORT

friend RWCString rwexport operator+(const RWCString& s1, const RWCString& s2);
friend RWCString rwexport operator+(const RWCString& s,  const char* cs);
friend RWCString rwexport operator+(const char* cs, const RWCString& s);
RW_INLINE_FRIEND 
RWBoolean rwexport operator==(const RWCString& s1, const RWCString& s2);
friend RWBoolean rwexport operator==(const RWCString& s1, const char* s2);

# else

friend rwexport RWCString operator+(const RWCString& s1, const RWCString& s2);
friend rwexport RWCString operator+(const RWCString& s,  const char* cs);
friend rwexport RWCString operator+(const char* cs, const RWCString& s);
RW_INLINE_FRIEND  
RWBoolean operator==(const RWCString& s1, const RWCString& s2);
friend rwexport RWBoolean operator==(const RWCString& s1, const char* s2);

# endif /* RW_TRAILING_RWEXPORT */

friend class RWExport RWCSubString;
friend class RWExport RWCStringRef;

  RW_TYPESPEC  /* This macro usually expands to nothing */
};

// Related global functions:
#ifndef RW_TRAILING_RWEXPORT
rwexport RW_SL_IO_STD(istream)&  operator>>(RW_SL_IO_STD(istream)& str   ,       RWCString& cstr);
rwexport RW_SL_IO_STD(ostream)&  operator<<(RW_SL_IO_STD(ostream)& str   , const RWCString& cstr);
#else
RW_SL_IO_STD(istream)& rwexport  operator>>(RW_SL_IO_STD(istream)& str   ,       RWCString& cstr);
RW_SL_IO_STD(ostream)& rwexport  operator<<(RW_SL_IO_STD(ostream)& str   , const RWCString& cstr);
#endif

inline RWvistream& operator>>(RWvistream& str,       RWCString& cstr)
                { cstr.restoreFrom(str);  return str; }
inline RWFile&     operator>>(RWFile& file,          RWCString& cstr)
                { cstr.restoreFrom(file); return file; }
inline RWvistream& operator>>(RWvistream& str,       RWCString*& cstr)
                { cstr = new RWCString; cstr->restoreFrom(str);  return str; }
inline RWFile&     operator>>(RWFile& file,          RWCString*& cstr)
                { cstr = new RWCString; cstr->restoreFrom(file); return file; }
inline RWvostream& operator<<(RWvostream& str, const RWCString& cstr)
                { cstr.saveOn(str);       return str; }
inline RWFile&     operator<<(RWFile& file,    const RWCString& cstr)
                { cstr.saveOn(file);      return file; }

RWCString rwexport toLower(const RWCString&);   // Return lower-case version of argument.
RWCString rwexport toUpper(const RWCString&);   // Return upper-case version of argument.
inline    unsigned rwhash(const RWCString& s) { return s.hash(); }
inline    unsigned rwhash(const RWCString* s) { return s->hash(); }
#ifndef RW_NO_LOCALE
RWCString rwexport strXForm(const RWCString&);  // strxfrm() interface
#endif


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               Inlines                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

inline void RWCString::cow()
{ if (pref()->references() > 1) clone(); }

inline void RWCString::cow(size_t nc)
{ if (pref()->references() > 1  || capacity() < nc) clone(nc); }

inline RWCString& RWCString::append(const char* cs)
{ return replace(length(), 0, cs, strlen(cs)); }

inline RWCString& RWCString::append(const char* cs, size_t N)
{ return replace(length(), 0, cs, N); }

inline RWCString& RWCString::append(const RWCString& s)
{ return replace(length(), 0, s.data(), s.length()); }

inline RWCString& RWCString::append(const RWCString& s, size_t N)
{ return replace(length(), 0, s.data(), rwmin(N, s.length())); }

inline RWCString& RWCString::operator+=(const char* cs)
{ return append(cs, strlen(cs)); }

inline RWCString& RWCString::operator+=(const RWCString& s)
{ return append(s.data(),s.length()); }

#ifndef RW_NO_LOCALE
inline int RWCString::collate(const RWCString& st) const
{ return pref()->collate(st.data()); }
#endif

inline RWBoolean RWCString::contains(const RWCString& pat, caseCompare cmp) const
{ return index(pat.data(), pat.length(), (size_t)0, cmp) != RW_NPOS; }

inline RWBoolean RWCString::contains(const char* s, caseCompare cmp) const
{ return index(s, strlen(s), (size_t)0, cmp) != RW_NPOS; }

inline size_t RWCString::index(const char* s, size_t i, caseCompare cmp) const
{ return index(s, strlen(s), i, cmp); }

inline size_t RWCString::index(const RWCString& s, size_t i, caseCompare cmp) const
{ return index(s.data(), s.length(), i, cmp); }

inline size_t RWCString::index(const RWCString& pat, size_t patlen, size_t i, caseCompare cmp) const
{ return index(pat.data(), patlen, i, cmp); }

inline RWCString& RWCString::insert(size_t pos, const char* cs)
{ return replace(pos, 0, cs, strlen(cs)); }

inline RWCString& RWCString::insert(size_t pos, const char* cs, size_t N)
{ return replace(pos, 0, cs, N); }

inline RWCString& RWCString::insert(size_t pos, const RWCString& cstr)
{ return replace(pos, 0, cstr.data(), cstr.length()); }

inline RWCString& RWCString::insert(size_t pos, const RWCString& cstr, size_t N)
{ return replace(pos, 0, cstr.data(), rwmin(N, cstr.length())); }

inline RWCString& RWCString::prepend(const char* cs)
{ return replace(0, 0, cs, strlen(cs)); }

inline RWCString& RWCString::prepend(const char* cs, size_t N)
{ return replace(0, 0, cs, N); }

inline RWCString& RWCString::prepend(const RWCString& s)
{ return replace(0, 0, s.data(), s.length()); }

inline RWCString& RWCString::prepend(const RWCString& s, size_t N)
{ return replace(0, 0, s.data(), rwmin(N, s.length())); }

inline RWCString& RWCString::remove(size_t pos)
{ return replace(pos, length()-pos, rwnil, 0); }

inline RWCString& RWCString::remove(size_t pos, size_t n)
{ return replace(pos, n, rwnil, 0); }

inline RWCString& RWCString::replace(size_t pos, size_t n, const char* cs)
{ return replace(pos, n, cs, strlen(cs)); }

inline RWCString& RWCString::replace(size_t pos, size_t n, const RWCString& cstr)
{ return replace(pos, n, cstr.data(), cstr.length()); }

inline RWCString& RWCString::replace(size_t pos, size_t n1, const RWCString& cstr, size_t n2)
{ return replace(pos, n1, cstr.data(), rwmin(cstr.length(),n2)); }

inline char& RWCString::operator()(size_t i)
{ 
#ifdef RWBOUNDS_CHECK
  assertElement(i); 
#endif
  cow();
  return data_[i];
}

#ifndef RW_NO_CONST_OVERLOAD
inline char RWCString::operator[](size_t i) const
{ assertElement(i); return data_[i]; }

inline char RWCString::operator()(size_t i) const
{
#ifdef RWBOUNDS_CHECK    
  assertElement(i); 
#endif
  return data_[i];
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
// RWCSubString::startData()
//
// This member replaces data().
// startData() will remain undocumented.  Please don't even ask.
// Use at your own risk. It may be deprecated in the future.
//
// Since RWCSubString works by referencing the RWCString's data,
// if you attempt to directly use the data() member of the RWCString,
// you will very likely be surprised by the result, which will be null
// terminated not at the extent of the substring,
// but at the end of the RWCString.
//
///////////////////////////////////////////////////////////////////////////////

inline const char* RWCSubString::startData() const
{ return str_->data() + begin_; }

// DON'T USE THE FUNCTION BELOW!
// This member is deprecated and will be removed in a future version.
// It remains public only to maintain source compatibility.
inline const char* RWCSubString::data() const
{ return str_->data() + begin_; }

//------------------------------------------------------------------------------

// Access to elements of sub-string with bounds checking
#ifndef RW_NO_CONST_OVERLOAD
inline char RWCSubString::operator[](size_t i) const
{ assertElement(i); return str_->data_[begin_+i]; }

inline char RWCSubString::operator()(size_t i) const
{
#ifdef RWBOUNDS_CHECK    
   assertElement(i);
#endif
   return str_->data_[begin_+i];
}
#endif

// String Logical operators:
inline RWBoolean        operator==(const RWCString& s1, const RWCString& s2)
                                  { return ((s1.length() == s2.length()) &&
                                    !memcmp(s1.data(), s2.data(), s1.length())); }
inline RWBoolean        operator< (const RWCString& s1, const RWCString& s2)
                                  { return s1.compareTo(s2)< 0;}


#ifdef RW_USES_STD_OPERATORS /* defined near top of this file */
// nothing goes here: <utility> provides
#undef RW_USES_STD_OPERATORS /* prevent macro escape from header */
#else
// we must provide our own
inline RWBoolean        operator!=(const RWCString& s1, const RWCString& s2)
                                  { return !(s1 == s2); }
inline RWBoolean        operator> (const RWCString& s1, const RWCString& s2)
                                  { return s1.compareTo(s2)> 0;}
inline RWBoolean        operator<=(const RWCString& s1, const RWCString& s2)
                                  { return s1.compareTo(s2)<=0;}
inline RWBoolean        operator>=(const RWCString& s1, const RWCString& s2)
                                  { return s1.compareTo(s2)>=0;}
#endif


//     RWBoolean        operator==(const RWCString& s1, const char* s2);
inline RWBoolean        operator!=(const RWCString& s1, const char* s2)
                                  { return !(s1 == s2); }
inline RWBoolean        operator< (const RWCString& s1, const char* s2)
                                  { return s1.compareTo(s2)< 0; }
inline RWBoolean        operator> (const RWCString& s1, const char* s2)
                                  { return s1.compareTo(s2)> 0; }
inline RWBoolean        operator<=(const RWCString& s1, const char* s2)
                                  { return s1.compareTo(s2)<=0; }
inline RWBoolean        operator>=(const RWCString& s1, const char* s2)
                                  { return s1.compareTo(s2)>=0; }

inline RWBoolean        operator==(const char* s1, const RWCString& s2)
                                  { return (s2 == s1); }
inline RWBoolean        operator!=(const char* s1, const RWCString& s2)
                                  { return !(s2 == s1); }
inline RWBoolean        operator< (const char* s1, const RWCString& s2)
                                  { return s2.compareTo(s1)> 0; }
inline RWBoolean        operator> (const char* s1, const RWCString& s2)
                                  { return s2.compareTo(s1)< 0; }
inline RWBoolean        operator<=(const char* s1, const RWCString& s2)
                                  { return s2.compareTo(s1)>=0; }
inline RWBoolean        operator>=(const char* s1, const RWCString& s2)
                                  { return s2.compareTo(s1)<=0; }

// SubString Logical operators:
//     RWBoolean operator==(const RWCSubString& s1, const RWCSubString& s2);
//     RWBoolean operator==(const RWCSubString& s1, const char* s2);
//     RWBoolean operator==(const RWCSubString& s1, const RWCString& s2);
inline RWBoolean operator==(const RWCString& s1,    const RWCSubString& s2)
                           { return (s2 == s1); }
inline RWBoolean operator==(const char* s1,         const RWCSubString& s2)
                           { return (s2 == s1); }
inline RWBoolean operator!=(const RWCSubString& s1, const char* s2)
                           { return !(s1 == s2); }
inline RWBoolean operator!=(const RWCSubString& s1, const RWCString& s2)
                           { return !(s1 == s2); }

#if defined(RW_NO_STL) || (defined __TURBOC__ && (__TURBOC__ < 0x500))
inline RWBoolean operator!=(const RWCSubString& s1, const RWCSubString& s2)
                           { return !(s1 == s2); }
#endif

inline RWBoolean operator!=(const RWCString& s1,    const RWCSubString& s2)
                           { return !(s2 == s1); }
inline RWBoolean operator!=(const char* s1,         const RWCSubString& s2)
                           { return !(s2 == s1); }

#ifndef RW_NO_RESOLVE_TEMPL_AMBIGUITY
#include "rw/strmshft.h"
class RWExport RWpostream;
class RWExport RWbostream;
class RWExport RWeostream;

RW_PROVIDE_DVSTREAM_INSERTER(RWpostream,RWCString)
RW_PROVIDE_DVSTREAM_INSERTER(RWbostream,RWCString)
RW_PROVIDE_DVSTREAM_INSERTER(RWeostream,RWCString)

#ifndef RW_NO_XDR
   class RWExport RWXDRostream;
   RW_PROVIDE_DVSTREAM_INSERTER(RWXDRostream,RWCString)
#endif /*  RW_NO_XDR  */

#endif /* RW_NO_RESOLVE_TEMPL_AMBIGUITY */

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /* __RWCSTRING_H__ */

