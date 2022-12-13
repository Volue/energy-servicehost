#ifndef __RWGSORTVEC_H__
#define __RWGSORTVEC_H__

/*
 * <generic.h> style sorted vector --- uses insertion sort
 *
 * $Id: gsortvec.h 18623 2000-03-03 08:28:13Z sih $
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
 * Declares a generic sorted vector.  This means that an item is inserted
 * so as to be after items "less than" itself, and before items 
 * "greater than" itself.  
 *
 * To use this class you must declare and implement the 
 * base class RWGVector(val), as well as the sorted vector itself.
 *
 *   #include <rw/gsortvec.h>
 *   declare(RWGVector,int)             // Declare the base class
 *   declare(RWGSortedVector,int)       // Declares a sorted vector of ints
 *
 *   // In one and only one .cpp file you must put the following:
 *   implement(RWGVector,int)           // Implement the base class
 *   implement(RWGSortedVector,int)     // Implement the sorted vector
 *
 * An instance of this vector could be used as follows:
 *
 *   int compFun(const int* a, const int* b)    // Define a comparison function
 *      { return *a - *b; }
 *   RWGSortedVector(compFun, int) avec;        // Create a sorted vector
 *   avec.insert(3);                            // Insert an item
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1.1.1  2000/03/02 21:40:42  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.7  1996/09/09 21:14:30  griswolf
 * Quite warning about extraneous semicolon in generic classes.
 *
 * Revision 7.6  1996/07/18 04:29:17  hart
 * ObjectStore changes
 *
 * Revision 7.5  1996/04/26 22:37:53  groening
 * removed the RW_BROKEN_TOKEN_PASTE wrapper.  it is never defined anywhere.
 * so yell at me if i broke things for MPW C++ 3.0 port.
 *
 * Revision 7.4  1996/02/18 01:44:41  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.3  1995/09/05 18:39:38  jims
 * Use new copyright macro
 *
 * Revision 7.2  1995/06/05  22:25:13  griswolf
 * Added use of NPOSINDEX error as needed.
 *
 * Revision 7.1  1994/10/16  02:50:32  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/14  21:04:32  jims
 * Use RWGExport for generic collection classes
 *
 * Revision 6.2  1994/07/12  18:46:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.1  1994/04/15  19:10:08  vriezen
 * Move all files to 6.1
 *
 * Revision 2.12  1994/03/02  19:19:33  jims
 * New message INDEXERR shows invalid index and number of
 * items in collection
 *
 * Revision 2.11  1993/09/16  04:18:06  keffer
 * Changed function prototype for RWbsearch to satisfy CSet++
 *
 * Revision 2.10  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.9  1993/08/03  21:17:57  dealys
 * Ported to MPW C++ 3.3 - RW_BROKEN_TOKEN_PASTE
 *
 * Revision 2.8  1993/05/31  21:45:26  keffer
 * New messaging architecture for localization
 *
 * Revision 2.7  1993/05/18  00:41:46  keffer
 * Now uses new exception handling facility
 *
 * Revision 2.6  1993/04/12  16:07:49  keffer
 * Now uses RW error facility instead of "generic" error facility.
 *
 * Revision 2.5  1993/04/09  19:35:45  keffer
 * Indexing is now done using size_t
 *
 * Revision 2.4  1993/03/25  03:50:41  keffer
 * Added prefix RW to class name
 *
 * Revision 2.3  1993/03/24  01:29:51  keffer
 * Indexing operations now used unsigned
 *
 * Revision 2.2  1993/02/08  00:24:55  keffer
 * Changed member data names
 *
 * Revision 2.0  1992/10/23  03:27:32  keffer
 * RCS Baseline version
 *
 * 
 *    Rev 1.5   14 May 1992 11:53:06   KEFFER
 * Changed name of RWGVector::resize() to reshape().
 * 
 *    Rev 1.3   04 Mar 1992 18:57:22   KEFFER
 * Now requires the user to declare and implement the base class RWGVector(val)
 * 
 *    Rev 1.2   18 Feb 1992 19:23:02   KEFFER
 * Now includes "rw/generic.h".
 * Class tag is now RWExport instead of huge.
 * 
 *    Rev 1.1   28 Oct 1991 09:08:16   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.0   28 Jul 1991 08:11:22   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 */

#include "rw/tooldefs.h"
#include "rw/gvector.h"

#  define RWGSortedVector(val) name2(val,RWGSortedVector)

#define RWGSortedVectordeclare(val)                                           \
RW_GENERIC_TYPESPEC_DECLARE_NAME(val)                                         \
class RWGExport RWGSortedVector(val) : public RWGVector(val) {                \
public:                                                                       \
  RWGSortedVector(val)(int(*f)(const val*,const val*))                        \
  : RWGVector(val)(RWDEFAULT_CAPACITY), nitems_(0), cf(f) {;}                 \
  RWGSortedVector(val)(int(*f)(const val*,const val*),size_t capac)           \
  : RWGVector(val)(capac),              nitems_(0), cf(f) {;}                 \
  val           operator()(size_t i) const {return array_[i];}                \
  val           operator[](size_t i) const {boundsCheck(i); return array_[i];}\
  size_t        entries() const            {return nitems_;}                  \
  size_t        index(val);                                                   \
  RWBoolean     insert(val);                                                  \
  size_t        length() const {return nitems_;}                              \
  void          removeAt(size_t);                                             \
  void          resize(size_t newCapacity);                                   \
protected:                                                                    \
  void          boundsCheck(size_t) const;                                    \
  size_t        nitems_; /* Number of slots in use */                         \
  int           (*cf)(const val*, const val*); /* Comparison function */      \
  RW_T_TYPESPEC  /* This macro usually expands to nothing */                  \
};

#define RWGSortedVectorimplement(val)                                         \
size_t RWGSortedVector(val)::index(val item)                                  \
{                                                                             \
  size_t idx;                                                                 \
  if(!RWbsearch(&item, array_, nitems_, sizeof(val), (RWcompare)cf, idx))     \
    return RW_NPOS;                                                           \
  while (idx && (*cf)(array_+idx-1, &item)==0) --idx;                         \
  return idx;                                                                 \
}                                                                             \
                                                                              \
RWBoolean RWGSortedVector(val)::insert(val item)                              \
{                                                                             \
  size_t idx;                                                                 \
  if(RWbsearch(&item, array_, nitems_, sizeof(val), (RWcompare)cf, idx))      \
    while (idx<nitems_ && (*cf)(array_+idx, &item) == 0) ++idx;               \
  else                                                                        \
    while (idx<nitems_ && (*cf)(array_+idx, &item) <  0) ++idx;               \
                                                                              \
  if( nitems_==npts_ ) resize(npts_+RWDEFAULT_RESIZE);                        \
  for(size_t j=nitems_; j>idx; j--) array_[j] = array_[j-1]; /* Slide right */\
  array_[idx] = item;                                                         \
  nitems_++;                                                                  \
  return TRUE;                                                                \
}                                                                             \
                                                                              \
void RWGSortedVector(val)::removeAt(size_t ipt)                               \
{                                                                             \
  /* Slide left (could be very expensive): */                                 \
  for(register size_t i=ipt; i<nitems_-1; i++) array_[i] = array_[i+1];       \
  nitems_--;                                                                  \
}                                                                             \
                                                                              \
void RWGSortedVector(val)::resize(size_t N)                                   \
{  if(N>nitems_) RWGVector(val)::reshape(N); }                                \
                                                                              \
void RWGSortedVector(val)::boundsCheck(size_t i) const {                      \
   if (i>=nitems_) {                                                          \
     if(RW_NPOS == i)                                                         \
       RWTHROW(RWBoundsErr(RWMessage(RWTOOL_NPOSINDEX)));                     \
     else                                                                     \
       RWTHROW(RWBoundsErr(RWMessage(RWTOOL_INDEXERR,                         \
                                   (unsigned)i, (unsigned)nitems_)) );        \
   }                                                                          \
}

#endif /*__RWGSORTVEC_H__*/
