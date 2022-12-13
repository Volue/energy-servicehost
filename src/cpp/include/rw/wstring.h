#ifndef __RWWSTRING_H__
#define __RWWSTRING_H__
/*
 * Declarations for RWWString --- wide character strings.
 *
 * $Id: wstring.h 18623 2000-03-03 08:28:13Z sih $
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
 */

#ifndef __RWTOOLDEFS_H__
# include "rw/tooldefs.h"
#endif
#ifndef __RWREF_H__
# include "rw/ref.h"
#endif
#include "rw/rstream.h"

STARTWRAP
#include <string.h>
ENDWRAP

#if !defined(RW_NO_STL) && defined(RW_TOOLSPRO)
template<class charT> class RWExport RWTRegularExpression;
typedef RWTRegularExpression<wchar_t> RWWRExpr;
#endif

//#2437, 2896,2899, 4812: C++ Standard templated operators <= >= >, !=
#if !defined(RW_NO_STL) && defined(RW_NO_RELOPS_NAMESPACE)
# if !defined(__SUNPRO_CC) || __SUNPRO_CC < 0x420
// SunPro 4.20 confuses the global operator=='s with template specializations.
# if !defined(__TURBOC__)
// bcc V4.52 and under does not cope with <utility>
#   define RW_USES_STD_OPERATORS /* for later use in this file only */
#   ifndef RW_HEADER_WITH_EXTENSION
#     include <utility>
#   else
#     include <utility.hpp>
#   endif  /* RW_HEADER_WITH_EXTENSION */
# endif  /* !TURBOC */
# endif  /* !SUNPRO */
#endif   /* RW_NO_STL */


class RWSExport RWCString;
class RWSExport RWWString;
class RWExport  RWWSubString;

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             RWWStringRef                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

/*
 * This is the dynamically allocated part of a RWWString.
 * It maintains a reference count.
 * There are no public member functions.
 */

class RWExport RWWStringRef : public RWReference
{

  static RWWStringRef*  getRep(size_t capac, size_t nchar);
  static RWWStringRef*  getRep(size_t capac, size_t nchar, void *where);

  void          unLink();       // Disconnect from a stringref, maybe delete it

  size_t        length   () const {return nchars_;}
  size_t        capacity () const {return capacity_;}
  wchar_t*      data     () const {return (wchar_t*)(this+1);}

  wchar_t&      operator[](size_t i)       {return ((wchar_t*)(this+1))[i];}
  wchar_t       operator[](size_t i) const {return ((wchar_t*)(this+1))[i];}

  int           collate(const wchar_t*) const;
  size_t        first    (wchar_t       ) const;
  size_t        first    (wchar_t,size_t) const;
  size_t        first    (const wchar_t*) const;
  size_t        first    (const wchar_t*,size_t) const;
  unsigned      hash     (              ) const;
  unsigned      hashFoldCase (          ) const;
  size_t        last     (wchar_t       ) const;
  size_t        last     (wchar_t,size_t) const;

  size_t        capacity_;      // Max string length (excluding null)
  size_t        nchars_;        // String length (excluding terminating null)

friend class RWSExport RWWString;
friend class RWExport  RWWSubString;
  
  RW_TYPESPEC  /* This macro usually expands to nothing */ 
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             RWWSubString                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

/*
 * The RWWSubString class allows selected elements to be addressed.
 * There are no public constructors.
 */

class RWExport RWWSubString
{
public:
  RWWSubString(const RWWSubString& sp)
    : str_(sp.str_), begin_(sp.begin_), extent_(sp.extent_) {;}

  RWWSubString& operator=(const wchar_t*);      // Assignment to wchar_t*
  RWWSubString& operator=(const RWWString&);    // Assignment to RWWString
  RWWSubString& operator=(const RWWSubString&);
  wchar_t&      operator()(size_t i);           // Index with optional bounds checking
  wchar_t&      operator[](size_t i);           // Index with bounds checking
#ifndef RW_NO_CONST_OVERLOAD
  wchar_t       operator()(size_t i) const;     // Index with optional bounds checking
  wchar_t       operator[](size_t i) const;     // Index with bounds checking
#endif

  const wchar_t*        startData() const;    // Replaces data(). See definition below.
  // startData() will remain undocumented.  Please don't even ask.
  // Use at your own risk. It may be deprecated in the future.

  // DON'T USE THE FUNCTION BELOW!
  const wchar_t*        data() const;         // Deprecated.
  // This member is deprecated and will be removed in a future version.
  // It remains public only to maintain source compatibility.
  // Since RWWSubString works by referencing the RWWString's data,
  // if you attempt to directly use the data() member of the RWWString,
  // you will very likely be surprised by the result, which will be null
  // terminated not at the extent of the substring,
  // but at the end of the RWWString.


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
  RWWSubString(const RWWString & s, size_t start, size_t len);

  RWWString*    str_;           // Referenced string
  size_t        begin_;         // Index of starting wchar_tacter
  size_t        extent_;        // Length of RWWSubString

friend rwexport 
RWBoolean operator==(const RWWSubString& s1, const RWWSubString& s2);
friend rwexport 
RWBoolean operator==(const RWWSubString& s1, const RWWString& s2);
friend rwexport
RWBoolean operator==(const RWWSubString& s1, const wchar_t* s2);

friend class RWSExport RWWString;

  RW_TYPESPEC  /* This macro usually expands to nothing */
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                              RWWString                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


class RWSExport RWWString
{

public:

  enum stripType   {leading = 0x1, trailing = 0x2, both = 0x3};
  enum caseCompare {exact, ignoreCase};
  enum scopeType   {one, all};
  enum multiByte_  {multiByte}; // Convert from multibyte
  enum ascii_      {ascii};     // Convert from ASCII

  RWWString();                  // Null string
  RWWString(RWSize_T ic);       // Suggested capacity
  RWWString(const RWWString& S);                // Copy constructor
  RWWString(const wchar_t * a);                 // Copy to embedded null
  RWWString(const wchar_t * a, size_t N);       // Copy past any embedded nulls
  RWWString(wchar_t);
  RWWString(char c) {initChar(c);}              // widen a char
#ifndef RW_NO_OVERLOAD_UCHAR
  RWWString(unsigned char c) {initChar(char(c));}     // widen a char
#endif
#ifndef RW_NO_OVERLOAD_SCHAR
  RWWString(signed char c) {initChar(char(c));}       // widen a char
#endif
  RWWString(wchar_t, size_t N);
  
  RWWString(const RWWSubString& SS);

  // Constructors used for MB to wide character conversions:
  RWWString(const char*, multiByte_);           // Convert from multibyte
  RWWString(const char*, ascii_    );           // Convert from ASCII
  RWWString(const char*, size_t N, multiByte_); // Convert N characters from MB
  RWWString(const char*, size_t N, ascii_    ); // Convert N characters from ASCII
  RWWString(const RWCString&, multiByte_);      // Convert from multibyte
  RWWString(const RWCString&, ascii_    );      // Convert from ASCII

  ~RWWString();

  // Type conversion:
#ifndef RW_ZTC_TYPE_CONVERSION_BUG  
                operator const wchar_t*() const {return data_;}
#endif

  // Assignment:
  RWWString&    operator=(const wchar_t*);      // Replace string
  RWWString&    operator=(const RWWString&);    // Replace string
  RWWString&    operator+=(const wchar_t*);     // Append string.
  RWWString&    operator+=(const RWWString& s);


  // Indexing operators:
  wchar_t&      operator[](size_t);             // Indexing with bounds checking
  wchar_t&      operator()(size_t);             // Indexing with optional bounds checking
  RWWSubString  operator()(size_t start, size_t len);   // Sub-string operator
# if !defined(RW_NO_STL) && defined(RW_TOOLSPRO)
  RWWSubString  operator()(const RWWRExpr& re);         // Match the RE
  RWWSubString  operator()(const RWWRExpr& re, size_t start);   // Match the RE
# endif
  RWWSubString  subString(const wchar_t* pat, size_t start=0, caseCompare=exact);
#ifndef RW_NO_CONST_OVERLOAD
  wchar_t       operator[](size_t) const;
  wchar_t       operator()(size_t) const;
  RWWSubString  operator()(size_t start, size_t len) const;
# if !defined(RW_NO_STL) && defined(RW_TOOLSPRO) /* Match the RE */
  RWWSubString  operator()(const RWWRExpr& pat) const; 
  RWWSubString  operator()(const RWWRExpr& pat, size_t start) const; 
#endif
  RWWSubString  subString(const wchar_t* pat, size_t start=0, caseCompare=exact) const;
  RWWSubString  strip(stripType s=trailing, wchar_t c=(wchar_t)' ') const;
#endif
  
      // Non-static member functions:
  RWWString&    append(const wchar_t* cs);
  RWWString&    append(const wchar_t* cs, size_t N);
  RWWString&    append(const RWWString& s);
  RWWString&    append(const RWWString& s, size_t N);
  RWWString&    append(wchar_t c, size_t rep=1);        // Append c rep times
  RWspace       binaryStoreSize() const         {return length()*sizeof(wchar_t)+sizeof(size_t);}
  size_t        capacity() const                {return pref()->capacity();}
  size_t        capacity(size_t N);
  int           collate(const wchar_t* cs) const {return pref()->collate(cs);}
  int           collate(const RWWString& st) const;
  int           compareTo(const wchar_t* cs,   caseCompare cmp = exact) const;
  int           compareTo(const RWWString& st, caseCompare cmp = exact) const;
  RWBoolean     contains(const wchar_t* pat,   caseCompare cmp = exact) const;
  RWBoolean     contains(const RWWString& pat, caseCompare cmp = exact) const;
  RWWString     copy() const;
  const wchar_t* data() const {return data_;}
  size_t        first(wchar_t c) const          {return pref()->first(c);}
  size_t        first(wchar_t c,size_t i) const
  { return pref()->first(c,i); }
  size_t        first(const wchar_t* cs) const  {return pref()->first(cs);}
  size_t        first(const wchar_t* cs,size_t N) const
  { return pref()->first(cs,N); }
  unsigned      hash(caseCompare cmp = exact) const;
  size_t        index(const wchar_t* pat, size_t i=0, caseCompare cmp = exact) const;
  size_t        index(const RWWString& s, size_t i=0, caseCompare cmp = exact) const;
  size_t        index(const wchar_t* pat, size_t patlen, size_t i,
                      caseCompare cmp) const;
  size_t        index(const RWWString& s, size_t patlen, size_t i,
                      caseCompare cmp) const;
# if !defined(RW_NO_STL) && defined(RW_TOOLSPRO)
  size_t        index(const RWWRExpr& pat, size_t i=0) const;
  size_t        index(const RWWRExpr& pat, size_t* ext, size_t i=0) const;
# endif
  RWWString&    insert(size_t pos, const wchar_t*);
  RWWString&    insert(size_t pos, const wchar_t*, size_t extent);
  RWWString&    insert(size_t pos, const RWWString&);
  RWWString&    insert(size_t pos, const RWWString&, size_t extent);
  RWBoolean     isAscii() const;
  RWBoolean     isNull() const                  {return pref()->nchars_ == 0;}
  size_t        last(wchar_t c) const           {return pref()->last(c);}
  size_t        last(wchar_t c,size_t i) const  {return pref()->last(c,i);}
  size_t        length() const                  {return pref()->nchars_;}

  RWWString&    prepend(const wchar_t*);                        // Prepend a wchar_tacter string
  RWWString&    prepend(const wchar_t* cs, size_t N);
  RWWString&    prepend(const RWWString& s);
  RWWString&    prepend(const RWWString& s, size_t N);
  RWWString&    prepend(wchar_t c, size_t rep=1);       // Prepend c rep times
  RW_SL_IO_STD(istream)&      readFile(RW_SL_IO_STD(istream)&);                     // Read to EOF or null wchar_tacter.
  RW_SL_IO_STD(istream)&      readLine(RW_SL_IO_STD(istream)&,
                         RWBoolean skipWhite = TRUE);   // Read to EOF or newline.
  RW_SL_IO_STD(istream)&      readString(RW_SL_IO_STD(istream)&);                   // Read to EOF or null wchar_tacter.
  RW_SL_IO_STD(istream)&      readToDelim(RW_SL_IO_STD(istream)&, wchar_t delim=(wchar_t)'\n');     // Read to EOF or delimiter.
  RW_SL_IO_STD(istream)&      readToken(RW_SL_IO_STD(istream)&);                    // Read separated by white space.
  RWWString&    remove(size_t pos);                     // Remove pos to end of string
  RWWString&    remove(size_t pos, size_t n);           // Remove n wchar_t's starting at pos
  RWWString&    replace(size_t pos, size_t n, const wchar_t*);
  RWWString&    replace(size_t pos, size_t n, const wchar_t*, size_t);
  RWWString&    replace(size_t pos, size_t n, const RWWString&);
  RWWString&    replace(size_t pos, size_t n, const RWWString&, size_t);
# if !defined(RW_NO_STL) && defined(RW_TOOLSPRO)
  RWWString&  replace
              (
                const RWWRExpr& pattern, const wchar_t *replacement,
                scopeType scope = one
              );
  RWWString&  replace
              (
                const RWWRExpr& pattern, const RWWString &replacement,
                scopeType scope = one
              );
# endif
  void          resize(size_t N);                       // Truncate or add blanks as necessary.
  void          restoreFrom(RWvistream&);               // Restore from ASCII store
  void          restoreFrom(RWFile&);                   // Restore string
  void          saveOn(RWvostream& s) const;
  void          saveOn(RWFile& f) const;
  RWWSubString  strip(stripType s=trailing, wchar_t c=(wchar_t)' ');
  RWCString     toAscii() const;                        // strip high bytes
  RWCString     toMultiByte() const;                    // use wcstombs()
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
  static unsigned       hash(const RWWString&);

protected:

  // Special concatenation constructor:
  RWWString(const wchar_t* a1, size_t N1, const wchar_t* a2, size_t N2);
  void                  assertElement(size_t) const;    // Index in range
  void                  clobber(size_t nc);             // Remove old contents
  void                  cow();                          // Do copy on write as needed
  void                  cow(size_t nc);                 // Do copy on write as needed
  RW_SL_IO_STD(istream)&              readToDelim(RW_SL_IO_STD(istream)&,
                                                                                wchar_t delim, RWBoolean skipWhite);
  static size_t         adjustCapacity(size_t nc);
  void                  initMB(const char*, size_t); // Initialize from multibyte
  void                  initMBReadNulls(const char*, size_t); // Initialize from multibyte to include embedded Nulls
  void                  initChar(char);              // Init from char

private:
  void                  clone();          // Make self a distinct copy
  void                  clone(size_t nc); // Make self a distinct copy w. capacity nc

#if !defined(_RWTOOLSDLL) || !defined(__WIN16__)
  /* If not compiling for shared address space, then use static data */
  static size_t initialCapac;           // Initial allocation Capacity
  static size_t resizeInc;              // Resizing increment
  static size_t freeboard;              // Max empty space before reclaim
#endif

  RWWStringRef* pref() const    { return ((RWWStringRef*)data_) - 1; }
  wchar_t*      data_;          // ref. counted data (RWWStringRef is in front)

friend rwexport RWWString operator+(const RWWString& s1, const RWWString& s2);
friend rwexport RWWString operator+(const RWWString& s,  const wchar_t* cs);
friend rwexport RWWString operator+(const wchar_t* cs, const RWWString& s);
RW_INLINE_FRIEND RWBoolean operator==(const RWWString& s1, const RWWString& s2);
friend rwexport RWBoolean operator==(const RWWString& s1, const wchar_t* s2);
friend class RWExport RWWSubString;
friend class RWExport RWWStringRef;

  RW_TYPESPEC  /* This macro usually expands to nothing */
};

// Related global functions:
#ifndef RW_TRAILING_RWEXPORT
rwexport RW_SL_IO_STD(istream)&  operator>>(RW_SL_IO_STD(istream)& str   , 
                                                                                 RWWString& wcstr);
rwexport RW_SL_IO_STD(ostream)&  operator<<(RW_SL_IO_STD(ostream)& str   ,
                                                                                 const RWWString& wcstr);
#else
RW_SL_IO_STD(istream)& rwexport  operator>>(RW_SL_IO_STD(istream)& str   ,
                                                                                 RWWString& wcstr);
RW_SL_IO_STD(ostream)& rwexport  operator<<(RW_SL_IO_STD(ostream)& str   ,
                                                                                 const RWWString& wcstr);
#endif

inline RWvistream& operator>>(RWvistream& str,       RWWString& wcstr)
                { wcstr.restoreFrom(str);  return str; }
inline RWFile&     operator>>(RWFile& file,          RWWString& wcstr)
                { wcstr.restoreFrom(file); return file; }
inline RWvistream& operator>>(RWvistream& str,       RWWString*& wcstr)
                { wcstr = new RWWString; wcstr->restoreFrom(str);  return str; }
inline RWFile&     operator>>(RWFile& file,          RWWString*& wcstr)
                { wcstr = new RWWString; wcstr->restoreFrom(file); return file; }
inline RWvostream& operator<<(RWvostream& str, const RWWString& wcstr)
                { wcstr.saveOn(str);       return str; }
inline RWFile&     operator<<(RWFile& file,    const RWWString& wcstr)
                { wcstr.saveOn(file);      return file; }

RWWString rwexport toLower(const RWWString&); // Ret lower-case version of arg
RWWString rwexport toUpper(const RWWString&); // Ret upper-case version of arg
inline    unsigned rwhash(const RWWString& s) { return s.hash(); }
inline    unsigned rwhash(const RWWString* s) { return s->hash(); }
#ifndef RW_NO_WCSXFRM
RWWString rwexport strXForm(const RWWString&);  // wsxfrm() interface
#endif /* RW_NO_WCSXFRM */

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               Inlines                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

inline void RWWString::cow()
{ if (pref()->references() > 1) clone(); }

inline void RWWString::cow(size_t nc)
{ if (pref()->references() > 1  || capacity() < nc) clone(nc); }

inline RWWString& RWWString::append(const wchar_t* cs, size_t N)
{ return replace(length(), 0, cs, N); }

inline RWWString& RWWString::append(const RWWString& s)
{ return replace(length(), 0, s.data(), s.length()); }

inline RWWString& RWWString::append(const RWWString& s, size_t N)
{ return replace(length(), 0, s.data(), rwmin(N, s.length())); }

inline RWWString& RWWString::operator+=(const RWWString& s)
{ return append(s.data(),s.length()); }

inline int RWWString::collate(const RWWString& st) const
{ return pref()->collate(st.data()); }

inline RWBoolean RWWString::contains(const RWWString& pat, caseCompare cmp) const
{ return index(pat.data(), pat.length(), (size_t)0, cmp) != RW_NPOS; }

inline size_t RWWString::index(const RWWString& s, size_t i, caseCompare cmp) const
{ return index(s.data(), s.length(), i, cmp); }

inline size_t RWWString::index(const RWWString& pat, size_t plen, size_t i, caseCompare cmp) const
{ return index(pat.data(), plen, i, cmp); }

inline RWWString& RWWString::insert(size_t pos, const wchar_t* cs, size_t N)
{ return replace(pos, 0, cs, N); }

inline RWWString& RWWString::insert(size_t pos, const RWWString& wcstr)
{ return replace(pos, 0, wcstr.data(), wcstr.length()); }

inline RWWString& RWWString::insert(size_t pos, const RWWString& wcstr, size_t N)
{ return replace(pos, 0, wcstr.data(), rwmin(N, wcstr.length())); }

inline RWWString& RWWString::prepend(const wchar_t* cs, size_t N)
{ return replace(0, 0, cs, N); }

inline RWWString& RWWString::prepend(const RWWString& s)
{ return replace(0, 0, s.data(), s.length()); }

inline RWWString& RWWString::prepend(const RWWString& s, size_t N)
{ return replace(0, 0, s.data(), rwmin(N, s.length())); }

inline RWWString& RWWString::remove(size_t pos)
{ return replace(pos, length()-pos, rwnil, 0); }

inline RWWString& RWWString::remove(size_t pos, size_t n)
{ return replace(pos, n, rwnil, 0); }

inline RWWString& RWWString::replace(size_t pos, size_t n, const RWWString& wcstr)
{ return replace(pos, n, wcstr.data(), wcstr.length()); }

inline RWWString& RWWString::replace(size_t pos, size_t n1, const RWWString& wcstr, size_t n2)
{ return replace(pos, n1, wcstr.data(), rwmin(wcstr.length(),n2)); }

inline wchar_t& RWWString::operator()(size_t i)
{ 
#ifdef RWBOUNDS_CHECK
  assertElement(i); 
#endif
  cow();
  return data_[i];
}

#ifndef RW_NO_CONST_OVERLOAD
inline wchar_t RWWString::operator[](size_t i) const
{ assertElement(i); return data_[i]; }

inline wchar_t RWWString::operator()(size_t i) const
{ 
#ifdef RWBOUNDS_CHECK    
  assertElement(i); 
#endif
  return data_[i];
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
// RWWSubString::startData()
//
// This member replaces data().
// startData() will remain undocumented.  Please don't even ask.
// Use at your own risk. It may be deprecated in the future.
//
// Since RWWSubString works by referencing the RWWString's data,
// if you attempt to directly use the data() member of the RWWString,
// you will very likely be surprised by the result, which will be null
// terminated not at the extent of the substring,
// but at the end of the RWWString.
//
///////////////////////////////////////////////////////////////////////////////

inline const wchar_t* RWWSubString::startData() const
{ return str_->data() + begin_; }

// DON'T USE THE FUNCTION BELOW!
// This member is deprecated and will be removed in a future version.
// It remains public only to maintain source compatibility.
inline const wchar_t* RWWSubString::data() const
{ return str_->data() + begin_; }

//------------------------------------------------------------------------------

// Access to elements of sub-string with bounds checking
#ifndef RW_NO_CONST_OVERLOAD
inline wchar_t RWWSubString::operator[](size_t i) const
{ assertElement(i); return str_->data_[begin_+i]; }

inline wchar_t RWWSubString::operator()(size_t i) const
{ 
#ifdef RWBOUNDS_CHECK    
   assertElement(i);
#endif
   return str_->data_[begin_+i];
}
#endif

// String Logical operators:
inline RWBoolean        operator==(const RWWString& s1, const RWWString& s2)
                                  { return ((s1.length() == s2.length()) &&
                                    !memcmp(s1.data(), s2.data(), s1.length()*sizeof(wchar_t))); }
inline RWBoolean        operator< (const RWWString& s1, const RWWString& s2)
                                  { return s1.compareTo(s2)< 0;}

#ifdef RW_USES_STD_OPERATORS /* defined near top of this file */
// nothing goes here: <utility> provides
#undef RW_USES_STD_OPERATORS /* prevent macro escape from header */
#else
// we must provide our own
inline RWBoolean        operator!=(const RWWString& s1, const RWWString& s2)
                                  { return !(s1 == s2); }
inline RWBoolean        operator> (const RWWString& s1, const RWWString& s2)
                                  { return s1.compareTo(s2)> 0;}
inline RWBoolean        operator<=(const RWWString& s1, const RWWString& s2)
                                  { return s1.compareTo(s2)<=0;}
inline RWBoolean        operator>=(const RWWString& s1, const RWWString& s2)
                                  { return s1.compareTo(s2)>=0;}
#endif

//     RWBoolean        operator==(const RWWString& s1, const wchar_t* s2);
inline RWBoolean        operator!=(const RWWString& s1, const wchar_t* s2)
                                  { return !(s1 == s2); }
inline RWBoolean        operator< (const RWWString& s1, const wchar_t* s2)
                                  { return s1.compareTo(s2)< 0; }
inline RWBoolean        operator> (const RWWString& s1, const wchar_t* s2)
                                  { return s1.compareTo(s2)> 0; }
inline RWBoolean        operator<=(const RWWString& s1, const wchar_t* s2)
                                  { return s1.compareTo(s2)<=0; }
inline RWBoolean        operator>=(const RWWString& s1, const wchar_t* s2)
                                  { return s1.compareTo(s2)>=0; }

inline RWBoolean        operator==(const wchar_t* s1, const RWWString& s2)
                                  { return (s2 == s1); }
inline RWBoolean        operator!=(const wchar_t* s1, const RWWString& s2)
                                  { return !(s2 == s1); }
inline RWBoolean        operator< (const wchar_t* s1, const RWWString& s2)
                                  { return s2.compareTo(s1)> 0; }
inline RWBoolean        operator> (const wchar_t* s1, const RWWString& s2)
                                  { return s2.compareTo(s1)< 0; }
inline RWBoolean        operator<=(const wchar_t* s1, const RWWString& s2)
                                  { return s2.compareTo(s1)>=0; }
inline RWBoolean        operator>=(const wchar_t* s1, const RWWString& s2)
                                  { return s2.compareTo(s1)<=0; }

// SubString Logical operators:
//     RWBoolean operator==(const RWWSubString& s1, const RWWSubString& s2);
//     RWBoolean operator==(const RWWSubString& s1, const wchar_t* s2);
//     RWBoolean operator==(const RWWSubString& s1, const RWWString& s2);
inline RWBoolean operator==(const RWWString& s1,    const RWWSubString& s2)
                           { return (s2 == s1); }
inline RWBoolean operator==(const wchar_t* s1,      const RWWSubString& s2)
                           { return (s2 == s1); }
inline RWBoolean operator!=(const RWWSubString& s1, const wchar_t* s2)
                           { return !(s1 == s2); }
inline RWBoolean operator!=(const RWWSubString& s1, const RWWString& s2)
                           { return !(s1 == s2); }

#if defined(RW_NO_STL) ||(defined __TURBOC__ && (__TURBOC__ < 0x500))
inline RWBoolean operator!=(const RWWSubString& s1, const RWWSubString& s2)
                           { return !(s1 == s2); }
#endif

inline RWBoolean operator!=(const RWWString& s1,    const RWWSubString& s2)
                           { return !(s2 == s1); }
inline RWBoolean operator!=(const wchar_t* s1,      const RWWSubString& s2)
                           { return !(s2 == s1); }

#endif /* __RWWSTRING_H__ */
