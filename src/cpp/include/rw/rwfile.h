#ifndef __RWFILE_H__
#define __RWFILE_H__

/*
 * Class RWFile encapsulates ANSI-C binary file operations.   
 *
 * $Id: rwfile.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 7.15  1996/09/05 15:20:49  griswolf
 * Scopus #4418: Fix problem with diff type and size of size_t.
 *
 * Revision 7.14  1996/05/07 22:23:57  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.13  1996/04/08 20:29:57  griswolf
 * Use rw/defs.h, instead of rw/tooldefs.h for RWoffset typdef.
 *
 * Revision 7.12  1996/03/15 05:06:49  kevinj
 * Bug #2805: Added RWEXPORT_RETURN(x) to new extraction and insertion
 * operators to make them visible to DLL's import libraries.
 *
 * Revision 7.11  1996/02/18 01:46:19  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.10  1996/01/27 00:49:37  kevinj
 * T61 ETP.
 *
 * Revision 7.9  1996/01/17 19:06:07  kevinj
 * Simple External Template Persistence (ETP).
 *
 * Revision 7.8  1995/09/05 19:33:04  jims
 * Use new copyright macro
 *
 * Revision 7.7  1995/08/23  22:40:51  kevinj
 * Added eof() and good().
 *
 * Revision 7.6  1995/07/12  20:28:19  griswolf
 * Scopus #1522: Add method Access().
 *
 * Revision 7.5  1995/06/29  15:39:46  kevinj
 * #1423:Changed "long" to "RWoffset" in RWFile::CurrOffset and RWFile::SeekTo.
 *
 * Revision 7.3  1995/06/14  00:51:37  griswolf
 * scopus 1364. Enhance static Exists(char*) method.
 *
 * Revision 7.2  1995/04/11  23:13:54  griswolf
 * added method ClearErr()
 *
 * Revision 7.1  1994/10/16  02:55:53  josh
 * Merged 6.1 and 7.0 development trees
 *
 *
 * Revision 6.3  1994/07/12  19:58:19  vriezen
 * Update Copyright notice
 *
 * Revision 6.1.7.4  1994/08/19  22:15:19  kevinj
 * Changed unsigned wchar_t to wchar_t so that xlC could compile.
 *
 * Revision 6.2  1994/06/13  19:50:07  vriezen
 * 429.  Fixed dtor to use delete [] and declared copy ctor and op= as private.
 *
 * Revision 6.1  1994/04/15  19:10:47  vriezen
 * Move all files to 6.1
 *
 * Revision 6.1.7.3  1994/06/13  19:51:34  vriezen
 * Bug #429.  Fixed dtor to use delete [] and declared copy ctor and op= as private.
 *
 * Revision 6.1.7.2  1994/04/28  19:48:14  vriezen
 * Add << and >> operators for builtin types.
 *
 * Revision 6.1.7.1  1994/04/15  22:45:18  vriezen
 * Version 7, Initial code
 *
 * Revision 6.1  1994/04/15  19:10:47  vriezen
 * Move all files to 6.1
 *
 * Revision 2.11  1994/02/05  02:02:17  jims
 * Add huge pointer support for character reads and writes
 *
 * Revision 2.10  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.9  1993/05/18  00:41:46  keffer
 * Now uses new exception handling facility
 *
 * Revision 2.8  1993/05/01  18:17:09  keffer
 * Added overloaded versions for unsigned short and long.
 *
 * Revision 2.7  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.6  1993/03/25  01:52:09  keffer
 * Added optional "mode" argument to the constructor;
 * Added member function "isValid()".
 *
 * Revision 2.5  1993/03/20  17:44:17  keffer
 * Array lengths now given by 'unsigned'
 *
 * Revision 2.2  1992/11/26  21:04:33  myersn
 * add members to operate on wchar_t values and strings.
 *
 * Revision 2.1  1992/11/19  05:45:01  keffer
 * Introduced new <rw/compiler.h> macro directives
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.5   18 Feb 1992 09:54:38   KEFFER
 * 
 *    Rev 1.4   17 Oct 1991 09:12:56   keffer
 * Changed include path to <rw/xxx.h>
 * 
 *    Rev 1.2   29 Jul 1991 11:03:38   keffer
 * Made Exists(const char*) a static function.
 * 
 *    Rev 1.1   24 Jul 1991 13:06:48   keffer
 * Added pvcs keywords
 */

/*
 * Under Unix, there there is no difference between text and binary 
 * files.  However, under MS-DOS, and some other operating systems, 
 * files opened for *text* output will use a CR/LF convention to end 
 * text lines, but do nothing for binary files.  Because RWFile is 
 * intended to model *binary* files, we must shut off this translation.
 * This requires opening the file in an explicit "binary" mode.  
 *
 * IF YOU INTEND FOR RWFILE TO WORK UNDER DOS AND USE THE OPTIONAL
 * "MODE" PARAMETER IN THE CONSTRUCTOR, YOU MUST BE SURE TO USE THE
 * "b" QUALIFIER!
 *
 *  RWFile myFile("junk.dat", "wb+");   // NB: "wb+"  NOT  "w+" !!
 */

#include "rw/defs.h"
STARTWRAP
#include <stdio.h>
ENDWRAP

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWFile
{
  RWFile(const RWFile&);            // Not implemented!
  void    operator=(const RWFile&); // Not implemented!
public:
  RWFile(const char* name, const char* mode = rwnil);
  ~RWFile();
       
  const char*           Access() const;
  const char*           GetName()   const {return filename;    }
  FILE*                 GetStream() const {return filep;       }
  RWBoolean             isValid()   const {return filep!=rwnil;}
  RWBoolean             Exists();
  static RWBoolean      Exists(const char* name);
  static RWBoolean      Exists(const char* name, int); // other permissions?
       
#ifndef RW_NO_BOOL
  RWBoolean             Read(RWBoolean& b);
#endif
  RWBoolean             Read(char& c);
  RWBoolean             Read(short& i);
  RWBoolean             Read(int& i);
  RWBoolean             Read(long& i);
#ifndef RW_NO_OVERLOAD_WCHAR
  RWBoolean             Read(wchar_t& w);
#endif
#ifndef RW_NO_OVERLOAD_UCHAR
  RWBoolean             Read(unsigned char& c);
#endif
  RWBoolean             Read(unsigned short&);
  RWBoolean             Read(unsigned int&  );
  RWBoolean             Read(unsigned long& );
  RWBoolean             Read(float& );
  RWBoolean             Read(double&);
#ifdef RW_FIX_XSIZE_T
  RWFile&               getSizeT(size_t& t) { Read(t); return *this; }
#endif
#ifndef RW_NO_BOOL
  RWBoolean             Read(RWBoolean*, size_t N);
#endif
  RWBoolean             Read(char*  , size_t N);
#ifdef RW_SEGMENTED_ARCHITECTURE
  RWBoolean             Read(char rwhuge*  , unsigned long N);
#endif
  RWBoolean             Read(short* , size_t N);
  RWBoolean             Read(int*   , size_t N);
  RWBoolean             Read(long*  , size_t N);
#ifndef RW_NO_OVERLOAD_WCHAR
  RWBoolean             Read(wchar_t* w, size_t N);
#endif
#ifndef RW_NO_OVERLOAD_UCHAR
  RWBoolean             Read(unsigned char* c, size_t N)
        {return Read((char*)c, N);}
#endif
  RWBoolean             Read(unsigned short* i, size_t N)
        {return Read((short*)i, N);}
  RWBoolean             Read(unsigned int*   i, size_t N)
        {return Read((int  *)i, N);}
  RWBoolean             Read(unsigned long*  i, size_t N)
        {return Read((long *)i, N);}
  RWBoolean             Read(float* , size_t N);
  RWBoolean             Read(double*, size_t N);

  // Read to null terminator or EOF; no CR/LF xlation done. Beware of overflow.
  RWBoolean             Read(char* string);     
  
#ifndef RW_NO_BOOL
  RWBoolean             Write(RWBoolean b);
#endif
  RWBoolean             Write(char c);
  RWBoolean             Write(short s);
  RWBoolean             Write(int i);
  RWBoolean             Write(long l);
#ifndef RW_NO_OVERLOAD_WCHAR
  RWBoolean             Write(wchar_t w);
#endif
#ifndef RW_NO_OVERLOAD_UCHAR
  RWBoolean             Write(unsigned char c);
#endif
  RWBoolean             Write(unsigned short s);
  RWBoolean             Write(unsigned int i);
  RWBoolean             Write(unsigned long l);
  RWBoolean             Write(float f);
  RWBoolean             Write(double d);
#ifdef RW_FIX_XSIZE_T
  RWFile&               putSizeT(size_t t) { Write(t); return *this; }
#endif
  RWBoolean             Write(const char* string);
#ifndef RW_NO_BOOL
  RWBoolean             Write(const RWBoolean* b, size_t N);
#endif
  RWBoolean             Write(const short* , size_t N);
  RWBoolean             Write(const int*   , size_t N);
  RWBoolean             Write(const long*  , size_t N);
#ifndef RW_NO_OVERLOAD_WCHAR
  RWBoolean             Write(const wchar_t* w, size_t N);
#endif
#ifndef RW_NO_OVERLOAD_UCHAR
  RWBoolean             Write(const unsigned char* c, size_t N)
        {return Write((const char*)c, N);}
#endif
  RWBoolean             Write(const unsigned short* i, size_t N)
        {return Write((const short*)i, N);}
  RWBoolean             Write(const unsigned int*   i, size_t N)
          {return Write((const int  *)i, N);}
  RWBoolean             Write(const unsigned long*  i, size_t N)
        {return Write((const long *)i, N);}
  RWBoolean             Write(const float*  , size_t N);
  RWBoolean             Write(const double* , size_t N);
  RWBoolean             Write(const char* string, size_t N);
#ifdef RW_SEGMENTED_ARCHITECTURE
  RWBoolean             Write(const char rwhuge* string, unsigned long N);
#endif

  RWoffset  CurOffset(); // Returns current offset of file
  void      ClearErr();  // reset to !Eof() and !Error()
  RWBoolean Eof();           // TRUE if file at EOF
  RWBoolean     Erase();
  RWBoolean     Error();     // TRUE if the file has had an error.
  RWBoolean     Flush();     // Writes all pending output
  RWBoolean     IsEmpty();   // TRUE if the file is empty
  RWBoolean     SeekTo(RWoffset offset); /* offset from beginning of file */
  RWBoolean     SeekToBegin()   {return SeekTo(0);}
  RWBoolean     SeekToEnd();

  // Used by RWMakeNewPtr<Source, T> in rw/epersist.h:
  RWBoolean eof()   {return Eof();}
  RWBoolean good()  {return (isValid() && !Error() && !Eof());}

protected:
  char*                 filename;
  FILE*                 filep;
};

inline RWFile& operator<<(RWFile& f, char a) 
{ f.Write(a); return f; }

#ifndef RW_NO_BOOL
inline RWFile& operator<<(RWFile& f, RWBoolean a) 
{ f.Write(a); return f; }
#endif

inline RWFile& operator<<(RWFile& f, short a) 
{ f.Write(a); return f; }

inline RWFile& operator<<(RWFile& f, int a)
{ f.Write(a); return f; }

inline RWFile& operator<<(RWFile& f, long a) 
{ f.Write(a); return f; }

#ifndef RW_NO_OVERLOAD_WCHAR
inline RWFile& operator<<(RWFile& f, wchar_t a) 
{ f.Write(a); return f; }
#endif

#ifndef RW_NO_OVERLOAD_UCHAR
inline RWFile& operator<<(RWFile& f, unsigned char a) 
{ f.Write(a); return f; }
#endif

inline RWFile& operator<<(RWFile& f, unsigned short a) 
{ f.Write(a); return f; }

inline RWFile& operator<<(RWFile& f, unsigned int a) 
{ f.Write(a); return f; }

inline RWFile& operator<<(RWFile& f, unsigned long a) 
{ f.Write(a); return f; }

inline RWFile& operator<<(RWFile& f, float a) 
{ f.Write(a); return f; }

inline RWFile& operator<<(RWFile& f, double a) 
{ f.Write(a); return f; }

inline RWFile& operator<<(RWFile& f, const char * a) 
{ f.Write(a); return f; }

inline RWFile& operator>>(RWFile& f, char& a) 
{ f.Read(a); return f; }

#ifndef RW_NO_BOOL
inline RWFile& operator>>(RWFile& f, RWBoolean& a) 
{ f.Read(a); return f; }
#endif

inline RWFile& operator>>(RWFile& f, short& a) 
{ f.Read(a); return f; }

inline RWFile& operator>>(RWFile& f, int& a) 
{ f.Read(a); return f; }

inline RWFile& operator>>(RWFile& f, long& a) 
{ f.Read(a); return f; }

#ifndef RW_NO_OVERLOAD_WCHAR
inline RWFile& operator>>(RWFile& f, wchar_t& a) 
{ f.Read(a); return f; }
#endif

#ifndef RW_NO_OVERLOAD_UCHAR
inline RWFile& operator>>(RWFile& f, unsigned char& a) 
{ f.Read(a); return f; }
#endif 

inline RWFile& operator>>(RWFile& f, unsigned short& a) 
{ f.Read(a); return f; }

inline RWFile& operator>>(RWFile& f, unsigned int& a) 
{ f.Read(a); return f; }

inline RWFile& operator>>(RWFile& f, unsigned long& a) 
{ f.Read(a); return f; }

inline RWFile& operator>>(RWFile& f, float& a) 
{ f.Read(a); return f; }

inline RWFile& operator>>(RWFile& f, double& a)
{ f.Read(a); return f; }

// The following are used for simple External Template Persistence (ETP)
// for builtin types.

RWEXPORT_RETURN(RWFile&) operator>>(RWFile& s, char*& ip);
#ifndef RW_NO_OVERLOAD_WCHAR
  RWEXPORT_RETURN(RWFile&) operator>>(RWFile& s, wchar_t*& ip);
#endif
#ifndef RW_NO_OVERLOAD_UCHAR
  RWEXPORT_RETURN(RWFile&) operator>>(RWFile& s, unsigned char*& ip);
#endif

RWEXPORT_RETURN(RWFile&) operator>>(RWFile& s, double*& ip);
RWEXPORT_RETURN(RWFile&) operator>>(RWFile& s, float*& ip);
RWEXPORT_RETURN(RWFile&) operator>>(RWFile& s, int*& ip);
RWEXPORT_RETURN(RWFile&) operator>>(RWFile& s, long*& ip);
RWEXPORT_RETURN(RWFile&) operator>>(RWFile& s, short*& ip);
RWEXPORT_RETURN(RWFile&) operator>>(RWFile& s, unsigned int*& ip);
RWEXPORT_RETURN(RWFile&) operator>>(RWFile& s, unsigned long*& ip);
RWEXPORT_RETURN(RWFile&) operator>>(RWFile& s, unsigned short*& ip);

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif  /* __RWFILE_H__ */
