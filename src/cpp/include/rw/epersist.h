#ifndef __RWEPERSIST_H__
#define __RWEPERSIST_H__

/***************************************************************************
 *
 * epersist.h - declarations for external template persistence.
 *
 * $Id: epersist.h 18623 2000-03-03 08:28:13Z sih $
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
 ****************************************************************************/

#include "rw/vstream.h"
#include "rw/rwfile.h"
#include "rw/toolerr.h"
#include "rw/rwstore.h"

#define RWDEFINE_PERSIST_IO(CLASS,ISTR,OSTR)                            \
                                                                        \
extern  void rwSaveGuts(OSTR&, const CLASS&);                           \
                                                                        \
RWEXPORT_RETURN(OSTR&) operator<<(OSTR& stream, const CLASS& item)      \
{                                                                       \
  int objectNum;                                                        \
                                                                        \
  RWUseStoreTable storeTable;                                           \
                                                                        \
  if(storeTable.add(&item, objectNum)){                                 \
    stream << ':';                                                      \
    rwSaveGuts(stream,item);                                            \
  }                                                                     \
  else                                                                  \
    stream << '@' << objectNum;                                         \
                                                                        \
  return stream;                                                        \
}                                                                       \
                                                                        \
extern void rwRestoreGuts(ISTR&, CLASS&);                               \
                                                                        \
RWEXPORT_RETURN(ISTR&) operator>>(ISTR& stream, CLASS& object)          \
{                                                                       \
  if (!stream.good()) return stream;                                    \
                                                                        \
  int            objectNum;                                             \
  char           refFlag;                                               \
  RWUseReadTable readTable;                                             \
                                                                        \
  stream >> refFlag;                                                    \
  if (!stream.good()) return stream;                                    \
                                                                        \
  if(refFlag == '@')                                                    \
  {                                                                     \
    stream >> objectNum;                                                \
    if (!stream.good()) return stream;                                  \
    if (   (size_t) objectNum >= readTable.entries()                    \
        || (&object != (const void*)readTable(objectNum))               \
       )                                                                \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  else if (refFlag == ':') {                                            \
    if (!stream.good()) return stream;                                  \
    readTable.append(&object);                                          \
    rwRestoreGuts(stream, object);                                      \
  }                                                                     \
  else {                                                                \
    /* Neither '@' nor ':' was found.  Bad input stream. */             \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  return stream;                                                        \
}                                                                       \
                                                                        \
extern  void rwRestoreGuts(ISTR&, CLASS&);                              \
                                                                        \
RWEXPORT_RETURN(ISTR&) operator>>(ISTR& stream, CLASS*& objectPtr)      \
{                                                                       \
  if (!stream.good()) return stream;                                    \
                                                                        \
  int            objectNum;                                             \
  char           refFlag;                                               \
  RWUseReadTable readTable;                                             \
                                                                        \
  stream >> refFlag;                                                    \
  if (!stream.good()) return stream;                                    \
                                                                        \
  if(refFlag == '@')                                                    \
  {                                                                     \
    stream >> objectNum;                                                \
    if (!stream.good()) return stream;                                  \
    if ((size_t) objectNum >= readTable.entries())                      \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
    else                                                                \
      objectPtr = (CLASS*) readTable.getPtr(objectNum);                 \
  }                                                                     \
  else if (refFlag == ':') {                                            \
    if (!stream.good()) return stream;                                  \
    objectPtr = new CLASS;                                              \
    readTable.append(objectPtr);                                        \
    rwRestoreGuts(stream,*objectPtr);                                   \
  }                                                                     \
  else {                                                                \
    /* Neither '@' nor ':' was found.  Bad input stream. */             \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  return stream;                                                        \
}

// Hack to help Borland 2.0 for OS/2, by casting away reference
// so Borland will instantiate global template function.
#  ifdef RW_NO_TEMPLATE_REF_INSTANTIATED
#     define RW_CAST_TEMPLATE_IO(T__CLASS) (const T__CLASS<T>)
#     define RW_CAST_TEMPLATE_IO_2(T__CLASS) (const T__CLASS<T1,T2>)
#     define RW_CAST_TEMPLATE_IO_3(T__CLASS) (const T__CLASS<T1,T2,T3>)
#     define RW_CAST_TEMPLATE_IO_4(T__CLASS) (const T__CLASS<T1,T2,T3,T4>)
#     define RW_CAST_TEMPLATE_IO_5(T__CLASS) (const T__CLASS<T1,T2,T3,T4,T5>)
#  else
#     define RW_CAST_TEMPLATE_IO(T__CLASS)
#     define RW_CAST_TEMPLATE_IO_2(T__CLASS)
#     define RW_CAST_TEMPLATE_IO_3(T__CLASS)
#     define RW_CAST_TEMPLATE_IO_4(T__CLASS)
#     define RW_CAST_TEMPLATE_IO_5(T__CLASS)
#  endif


#define RWDEFINE_PERSIST_TEMPLATE_IO(T__CLASS,ISTR,OSTR)                \
template <class T>                                                      \
RWTEXPORT_RETURN(OSTR&) operator<<(OSTR& stream, const T__CLASS<T>& item)\
{                                                                       \
  int objectNum;                                                        \
                                                                        \
  RWUseStoreTable storeTable;                                           \
                                                                        \
  if(storeTable.add(&item, objectNum)){                                 \
    stream << ':';                                                      \
    rwSaveGuts(stream,RW_CAST_TEMPLATE_IO(T__CLASS) item);              \
  }                                                                     \
  else                                                                  \
    stream << '@' << objectNum;                                         \
                                                                        \
  return stream;                                                        \
}                                                                       \
                                                                        \
template <class T>                                                      \
RWTEXPORT_RETURN(ISTR&) operator>>(ISTR& stream, T__CLASS<T>& object)   \
{                                                                       \
  if (!stream.good()) return stream;                                    \
                                                                        \
  int            objectNum;                                             \
  char           refFlag;                                               \
  RWUseReadTable readTable;                                             \
                                                                        \
  stream >> refFlag;                                                    \
  if (!stream.good()) return stream;                                    \
                                                                        \
  if(refFlag == '@')                                                    \
  {                                                                     \
    stream >> objectNum;                                                \
    if (!stream.good()) return stream;                                  \
    if (   (size_t) objectNum >= readTable.entries()                    \
        || (&object != (const void*)readTable(objectNum))               \
       )                                                                \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  else if (refFlag == ':') {                                            \
    if (!stream.good()) return stream;                                  \
    readTable.append(&object);                                          \
    rwRestoreGuts(stream, object);                                      \
  }                                                                     \
  else {                                                                \
    /* Neither '@' nor ':' was found.  Bad input stream. */             \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  return stream;                                                        \
}                                                                       \
                                                                        \
template <class T>                                                      \
RWTEXPORT_RETURN(ISTR&) operator>>(ISTR& stream, T__CLASS<T>*& objectPtr) \
{                                                                       \
  if (!stream.good()) return stream;                                    \
                                                                        \
  int            objectNum;                                             \
  char           refFlag;                                               \
  RWUseReadTable readTable;                                             \
                                                                        \
  stream >> refFlag;                                                    \
  if (!stream.good()) return stream;                                    \
                                                                        \
  if(refFlag == '@')                                                    \
  {                                                                     \
    stream >> objectNum;                                                \
    if (!stream.good()) return stream;                                  \
    if ((size_t) objectNum >= readTable.entries())                      \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
    else                                                                \
      objectPtr = (T__CLASS<T>*) readTable.getPtr(objectNum);           \
  }                                                                     \
  else if (refFlag == ':') {                                            \
    if (!stream.good()) return stream;                                  \
    objectPtr = new T__CLASS<T>;                                        \
    readTable.append(objectPtr);                                        \
    rwRestoreGuts(stream, *objectPtr);                                  \
  }                                                                     \
  else {                                                                \
    /* Neither '@' nor ':' was found.  Bad input stream. */             \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  return stream;                                                        \
}

#define RWDEFINE_PERSIST_TEMPLATE_IO_2(T__CLASS,ISTR,OSTR)              \
template <class T1, class T2>                                           \
RWTEXPORT_RETURN(OSTR&) operator<<(OSTR& stream, const T__CLASS<T1,T2>& item) \
{                                                                       \
  int objectNum;                                                        \
                                                                        \
  RWUseStoreTable storeTable;                                           \
                                                                        \
  if(storeTable.add(&item, objectNum)){                                 \
    stream << ':';                                                      \
    rwSaveGuts(stream, RW_CAST_TEMPLATE_IO_2(T__CLASS) item);           \
  }                                                                     \
  else                                                                  \
    stream << '@' << objectNum;                                         \
                                                                        \
  return stream;                                                        \
}                                                                       \
                                                                        \
template <class T1, class T2>                                           \
RWTEXPORT_RETURN(ISTR&) operator>>(ISTR& stream, T__CLASS<T1,T2>& object) \
{                                                                       \
  if (!stream.good()) return stream;                                    \
                                                                        \
  int            objectNum;                                             \
  char           refFlag;                                               \
  RWUseReadTable readTable;                                             \
                                                                        \
  stream >> refFlag;                                                    \
  if (!stream.good()) return stream;                                    \
                                                                        \
  if(refFlag == '@')                                                    \
  {                                                                     \
    stream >> objectNum;                                                \
    if (!stream.good()) return stream;                                  \
    if (   (size_t) objectNum >= readTable.entries()                    \
        || (&object != (const void*)readTable(objectNum))               \
       )                                                                \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  else if (refFlag == ':') {                                            \
    if (!stream.good()) return stream;                                  \
    readTable.append(&object);                                          \
    rwRestoreGuts(stream, object);                                      \
  }                                                                     \
  else {                                                                \
    /* Neither '@' nor ':' was found.  Bad input stream. */             \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  return stream;                                                        \
}                                                                       \
                                                                        \
template <class T1, class T2>                                           \
RWTEXPORT_RETURN(ISTR&) operator>>(ISTR& stream, T__CLASS<T1,T2>*& objectPtr) \
{                                                                       \
  if (!stream.good()) return stream;                                    \
                                                                        \
  int            objectNum;                                             \
  char           refFlag;                                               \
  RWUseReadTable readTable;                                             \
                                                                        \
  stream >> refFlag;                                                    \
  if (!stream.good()) return stream;                                    \
                                                                        \
  if(refFlag == '@')                                                    \
  {                                                                     \
    stream >> objectNum;                                                \
    if (!stream.good()) return stream;                                  \
    if ((size_t) objectNum >= readTable.entries())                      \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
    else                                                                \
      objectPtr = (T__CLASS<T1,T2>*) readTable.getPtr(objectNum);       \
  }                                                                     \
  else if (refFlag == ':') {                                            \
    if (!stream.good()) return stream;                                  \
    objectPtr = new T__CLASS<T1,T2>;                                    \
    readTable.append(objectPtr);                                        \
    rwRestoreGuts(stream, *objectPtr);                                  \
  }                                                                     \
  else {                                                                \
    /* Neither '@' nor ':' was found.  Bad input stream. */             \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  return stream;                                                        \
}

#define RWDEFINE_PERSIST_TEMPLATE_IO_3(T__CLASS,ISTR,OSTR)              \
template <class T1, class T2, class T3>                                 \
RWTEXPORT_RETURN(OSTR&)                                                 \
     operator<<(OSTR& stream, const T__CLASS<T1,T2,T3>& item)           \
{                                                                       \
  int objectNum;                                                        \
                                                                        \
  RWUseStoreTable storeTable;                                           \
                                                                        \
  if(storeTable.add(&item, objectNum)){                                 \
    stream << ':';                                                      \
    rwSaveGuts(stream,RW_CAST_TEMPLATE_IO_3(T__CLASS) item);            \
  }                                                                     \
  else                                                                  \
    stream << '@' << objectNum;                                         \
                                                                        \
  return stream;                                                        \
}                                                                       \
                                                                        \
template <class T1, class T2, class T3>                                 \
RWTEXPORT_RETURN(ISTR&) operator>>(ISTR& stream, T__CLASS<T1,T2,T3>& object) \
{                                                                       \
  if (!stream.good()) return stream;                                    \
                                                                        \
  int            objectNum;                                             \
  char           refFlag;                                               \
  RWUseReadTable readTable;                                             \
                                                                        \
  stream >> refFlag;                                                    \
  if (!stream.good()) return stream;                                    \
                                                                        \
  if(refFlag == '@')                                                    \
  {                                                                     \
    stream >> objectNum;                                                \
    if (!stream.good()) return stream;                                  \
    if (   (size_t) objectNum >= readTable.entries()                    \
        || (&object != (const void*)readTable(objectNum))               \
       )                                                                \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  else if (refFlag == ':') {                                            \
    if (!stream.good()) return stream;                                  \
    readTable.append(&object);                                          \
    rwRestoreGuts(stream, object);                                      \
  }                                                                     \
  else {                                                                \
    /* Neither '@' nor ':' was found.  Bad input stream. */             \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  return stream;                                                        \
}                                                                       \
                                                                        \
template <class T1, class T2, class T3>                                 \
RWTEXPORT_RETURN(ISTR&) operator>>(ISTR& stream, T__CLASS<T1,T2,T3>*& objectPtr) \
{                                                                       \
  if (!stream.good()) return stream;                                    \
                                                                        \
  int            objectNum;                                             \
  char           refFlag;                                               \
  RWUseReadTable readTable;                                             \
                                                                        \
  stream >> refFlag;                                                    \
  if (!stream.good()) return stream;                                    \
                                                                        \
  if(refFlag == '@')                                                    \
  {                                                                     \
    stream >> objectNum;                                                \
    if (!stream.good()) return stream;                                  \
    if ((size_t) objectNum >= readTable.entries())                      \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
    else                                                                \
      objectPtr = (T__CLASS<T1,T2,T3>*) readTable.getPtr(objectNum);    \
  }                                                                     \
  else if (refFlag == ':') {                                            \
    if (!stream.good()) return stream;                                  \
    objectPtr = new T__CLASS<T1,T2,T3>;                                 \
    readTable.append(objectPtr);                                        \
    rwRestoreGuts(stream, *objectPtr);                                  \
  }                                                                     \
  else {                                                                \
    /* Neither '@' nor ':' was found.  Bad input stream. */             \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  return stream;                                                        \
}

#define RWDEFINE_PERSIST_TEMPLATE_IO_4(T__CLASS,ISTR,OSTR)              \
template <class T1, class T2, class T3, class T4>                       \
RWTEXPORT_RETURN(OSTR&)                                                 \
     operator<<(OSTR& stream, const T__CLASS<T1,T2,T3,T4>& item)        \
{                                                                       \
  int objectNum;                                                        \
                                                                        \
  RWUseStoreTable storeTable;                                           \
                                                                        \
  if(storeTable.add(&item, objectNum)){                                 \
    stream << ':';                                                      \
    rwSaveGuts(stream, RW_CAST_TEMPLATE_IO_4(T__CLASS) item);           \
  }                                                                     \
  else                                                                  \
    stream << '@' << objectNum;                                         \
                                                                        \
  return stream;                                                        \
}                                                                       \
                                                                        \
template <class T1, class T2, class T3, class T4>                       \
RWTEXPORT_RETURN(ISTR&) operator>>(ISTR& stream, T__CLASS<T1,T2,T3,T4>& object) \
{                                                                       \
  if (!stream.good()) return stream;                                    \
                                                                        \
  int            objectNum;                                             \
  char           refFlag;                                               \
  RWUseReadTable readTable;                                             \
                                                                        \
  stream >> refFlag;                                                    \
  if (!stream.good()) return stream;                                    \
                                                                        \
  if(refFlag == '@')                                                    \
  {                                                                     \
    stream >> objectNum;                                                \
    if (!stream.good()) return stream;                                  \
    if (   (size_t) objectNum >= readTable.entries()                    \
        || (&object != (const void*)readTable(objectNum))               \
       )                                                                \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  else if (refFlag == ':') {                                            \
    if (!stream.good()) return stream;                                  \
    readTable.append(&object);                                          \
    rwRestoreGuts(stream, object);                                      \
  }                                                                     \
  else {                                                                \
    /* Neither '@' nor ':' was found.  Bad input stream. */             \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  return stream;                                                        \
}                                                                       \
                                                                        \
template <class T1, class T2, class T3, class T4>                       \
RWTEXPORT_RETURN(ISTR&) operator>>(ISTR& stream, T__CLASS<T1,T2,T3,T4>*& objectPtr) \
{                                                                       \
  if (!stream.good()) return stream;                                    \
                                                                        \
  int            objectNum;                                             \
  char           refFlag;                                               \
  RWUseReadTable readTable;                                             \
                                                                        \
  stream >> refFlag;                                                    \
  if (!stream.good()) return stream;                                    \
                                                                        \
  if(refFlag == '@')                                                    \
  {                                                                     \
    stream >> objectNum;                                                \
    if (!stream.good()) return stream;                                  \
    if ((size_t) objectNum >= readTable.entries())                      \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
    else                                                                \
      objectPtr = (T__CLASS<T1,T2,T3,T4>*) readTable.getPtr(objectNum); \
  }                                                                     \
  else if (refFlag == ':') {                                            \
    if (!stream.good()) return stream;                                  \
    objectPtr = new T__CLASS<T1,T2,T3,T4>;                              \
    readTable.append(objectPtr);                                        \
    rwRestoreGuts(stream, *objectPtr);                                  \
  }                                                                     \
  else {                                                                \
    /* Neither '@' nor ':' was found.  Bad input stream. */             \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  return stream;                                                        \
}

#define RWDEFINE_PERSIST_TEMPLATE_IO_5(T__CLASS,ISTR,OSTR)              \
template <class T1, class T2, class T3, class T4, class T5>             \
RWTEXPORT_RETURN(OSTR&)                                                 \
     operator<<(OSTR& stream, const T__CLASS<T1,T2,T3,T4,T5>& item)     \
{                                                                       \
  int objectNum;                                                        \
                                                                        \
  RWUseStoreTable storeTable;                                           \
                                                                        \
  if(storeTable.add(&item, objectNum)){                                 \
    stream << ':';                                                      \
    rwSaveGuts(stream, RW_CAST_TEMPLATE_IO_5(T__CLASS) item);           \
  }                                                                     \
  else                                                                  \
    stream << '@' << objectNum;                                         \
                                                                        \
  return stream;                                                        \
}                                                                       \
                                                                        \
template <class T1, class T2, class T3, class T4, class T5>             \
RWTEXPORT_RETURN(ISTR&) operator>>(ISTR& stream, T__CLASS<T1,T2,T3,T4,T5>& object) \
{                                                                       \
  if (!stream.good()) return stream;                                    \
                                                                        \
  int            objectNum;                                             \
  char           refFlag;                                               \
  RWUseReadTable readTable;                                             \
                                                                        \
  stream >> refFlag;                                                    \
  if (!stream.good()) return stream;                                    \
                                                                        \
  if(refFlag == '@')                                                    \
  {                                                                     \
    stream >> objectNum;                                                \
    if (!stream.good()) return stream;                                  \
    if (   (size_t) objectNum >= readTable.entries()                    \
        || (&object != (const void*)readTable(objectNum))               \
       )                                                                \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  else if (refFlag == ':') {                                            \
    if (!stream.good()) return stream;                                  \
    readTable.append(&object);                                          \
    rwRestoreGuts(stream, object);                                      \
  }                                                                     \
  else {                                                                \
    /* Neither '@' nor ':' was found.  Bad input stream. */             \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  return stream;                                                        \
}                                                                       \
                                                                        \
template <class T1, class T2, class T3, class T4, class T5>             \
RWTEXPORT_RETURN(ISTR&) operator>>(ISTR& stream, T__CLASS<T1,T2,T3,T4,T5>*& objectPtr) \
{                                                                       \
  if (!stream.good()) return stream;                                    \
                                                                        \
  int            objectNum;                                             \
  char           refFlag;                                               \
  RWUseReadTable readTable;                                             \
                                                                        \
  stream >> refFlag;                                                    \
  if (!stream.good()) return stream;                                    \
                                                                        \
  if(refFlag == '@')                                                    \
  {                                                                     \
    stream >> objectNum;                                                \
    if (!stream.good()) return stream;                                  \
    if ((size_t) objectNum >= readTable.entries())                      \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
    else                                                                \
      objectPtr = (T__CLASS<T1,T2,T3,T4,T5>*) readTable.getPtr(objectNum); \
  }                                                                     \
  else if (refFlag == ':') {                                            \
    if (!stream.good()) return stream;                                  \
    objectPtr = new T__CLASS<T1,T2,T3,T4,T5>;                           \
    readTable.append(objectPtr);                                        \
    rwRestoreGuts(stream, *objectPtr);                                  \
  }                                                                     \
  else {                                                                \
    /* Neither '@' nor ':' was found.  Bad input stream. */             \
      RWTHROW(RWInternalErr(RWMessage(RWTOOL_REF)));                    \
  }                                                                     \
  return stream;                                                        \
}


#define RWDEFINE_PERSISTABLE(CLASS)                                     \
RWDEFINE_PERSIST_IO(CLASS,RWvistream,RWvostream)                        \
RWDEFINE_PERSIST_IO(CLASS,RWFile,RWFile)

#define RWDEFINE_PERSISTABLE_TEMPLATE(T__CLASS)                         \
RWDEFINE_PERSIST_TEMPLATE_IO(T__CLASS,RWvistream,RWvostream)            \
RWDEFINE_PERSIST_TEMPLATE_IO(T__CLASS,RWFile,RWFile)

#define RWDEFINE_PERSISTABLE_TEMPLATE_2(T__CLASS)                       \
RWDEFINE_PERSIST_TEMPLATE_IO_2(T__CLASS,RWvistream,RWvostream)          \
RWDEFINE_PERSIST_TEMPLATE_IO_2(T__CLASS,RWFile,RWFile)

#define RWDEFINE_PERSISTABLE_TEMPLATE_3(T__CLASS)                       \
RWDEFINE_PERSIST_TEMPLATE_IO_3(T__CLASS,RWvistream,RWvostream)          \
RWDEFINE_PERSIST_TEMPLATE_IO_3(T__CLASS,RWFile,RWFile)

#define RWDEFINE_PERSISTABLE_TEMPLATE_4(T__CLASS)                       \
RWDEFINE_PERSIST_TEMPLATE_IO_4(T__CLASS,RWvistream,RWvostream)          \
RWDEFINE_PERSIST_TEMPLATE_IO_4(T__CLASS,RWFile,RWFile)

#define RWDEFINE_PERSISTABLE_TEMPLATE_5(T__CLASS)                       \
RWDEFINE_PERSIST_TEMPLATE_IO_5(T__CLASS,RWvistream,RWvostream)          \
RWDEFINE_PERSIST_TEMPLATE_IO_5(T__CLASS,RWFile,RWFile)

#ifndef RW_NO_TEMPLATES
#  ifdef RW_NO_STL
// provide functions so we can use apply() to stream data out
#    ifndef RW_NO_REF_TO_REF
template<class T>
void rwInsertInStream(T& item, void*x);

template<class T>
void rwInsertInFile(T& item, void*x);

#    else

template<class T>
void rwInsertInStream(T item, void*x);

template<class T>
void rwInsertInFile(T item, void*x);

#    endif /* RW_NO_REF_TO_REF */
#  endif /* RW_NO_STL */
#endif /* RW_NO_TEMPLATES */

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("epersist.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE
#include "rw/epersist.cc"
#endif

#endif /* __RWEPERSIST_H__ */
