#ifndef __RWTOOLDEFS_H__
#define __RWTOOLDEFS_H__

/*
 * General declarations for Tools.h++
 *
 * $Id: tooldefs.h 18623 2000-03-03 08:28:13Z sih $
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

#define RWTOOLS 0x0710  /* Version number */

#ifndef __RWDEFS_H__
#  include "rw/defs.h"
#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

// Function to return the version of Tools.h++:
unsigned rwexport rwToolsVersion();

class RWExport RWCollectable;

#ifdef RW_STD_TYPEDEFS

/*
 * Standard Smalltalk interface, activated by defining
 * RW_STD_TYPEDEFS before including this file.
 */

class RWExport RWBag;
class RWExport RWBagIterator;
class RWExport RWBinaryTree;
class RWExport RWBinaryTreeIterator;
class RWExport RWCollectableDate;
class RWExport RWCollectableInt;
class RWExport RWCollectableString;
class RWExport RWCollectableTime;
class RWExport RWCollection;
class RWExport RWDlistCollectables;
class RWExport RWDlistCollectablesIterator;
class RWExport RWHashDictionary;
class RWExport RWHashDictionaryIterator;
class RWExport RWIdentityDictionary;
class RWExport RWIdentitySet;
class RWExport RWOrdered;
class RWExport RWOrderedIterator;
class RWExport RWSequenceable;
class RWExport RWSet;
class RWExport RWSetIterator;
class RWExport RWSlistCollectables;
class RWExport RWSlistCollectablesIterator;
class RWExport RWSlistCollectablesQueue;
class RWExport RWSlistCollectablesStack;

typedef RWBag                           Bag;
typedef RWBagIterator                   BagIterator;
typedef RWBinaryTree                    SortedCollection;
typedef RWBinaryTreeIterator            SortedCollectionIterator;
typedef RWCollectable                   Object;                 // Careful: All-to-common type
typedef RWCollectableDate               Date;
typedef RWCollectableInt                Integer;
typedef RWCollectableString             String;
typedef RWCollectableTime               Time;
typedef RWCollection                    Collection;
typedef RWHashDictionary                Dictionary;
typedef RWHashDictionaryIterator        DictionaryIterator;
typedef RWIdentityDictionary            IdentityDictionary;
typedef RWIdentitySet                   IdentitySet;
typedef RWOrdered                       OrderedCollection;
typedef RWOrderedIterator               OrderedCollectionIterator;
typedef RWSequenceable                  SequenceableCollection;
typedef RWSet                           Set;
typedef RWSetIterator                   SetIterator;
typedef RWSlistCollectables             LinkedList;
typedef RWSlistCollectablesIterator     LinkedListIterator;
typedef RWSlistCollectablesQueue        Queue;
typedef RWSlistCollectablesStack        Stack;

#endif  /* RW_STD_TYPEDEFS */

const long MAGIC_CONSTANT = 0x8888; // Safety check on binary files.

/*
 * Class ID definitions:
 */

const RWClassID REFFLAG   = 0xFFFF; // Signal reference, instead of ClassID
const RWClassID INSTFLAG   = 0xFFFE; // Signal instance for template collection persistence

#define __RWUNKNOWN                     0x8000
#define __RWGDLIST                      0x8014
#define __RWGDLISTITERATOR              0x8015
#define __RWGQUEUE                      0x8016
#define __RWGSTACK                      0x8017
#define __RWGVECTOR                     0x8018
#define __RWBAG                         0x801b
#define __RWBAGITERATOR                 0x801c
#define __RWBINARYTREE                  0x8001
#define __RWBINARYTREEITERATOR          0x8002
#define __RWBTREE                       0x8003
#define __RWBTREEDICTIONARY             0x8004
#define __RWBTREENODE                   0x8005
#define __RWBTREEONDISK                 0x8006
#define __RWCOLLECTABLE                 0x8007
#define __RWCOLLECTABLEASSOCIATION      0x8008
#define __RWCOLLECTABLEDATE             0x8009
#define __RWCOLLECTABLEINT              0x800a
#define __RWCOLLECTABLESTRING           0x800b
#define __RWCOLLECTABLETIME             0x800c
#define __RWCOLLECTION                  0x801d
#define __RWDATE                        0x801e
#define __RWDISKTREENODE                0x800d
#define __RWDLINK                       0x800e
#define __RWDLIST                       0x800f
#define __RWDLISTCOLLECTABLES           0x8010
#define __RWDLISTCOLLECTABLESITERATOR   0x8011
#define __RWDLISTITERATOR               0x8012
#define __RWFACTORY                     0x801f
#define __RWFILEMANAGER                 0x8013
#define __RWFREELISTDATA                0x8021
#define __RWHASHDICTIONARY              0x8022
#define __RWHASHDICTIONARYITERATOR      0x8023
#define __RWIDENTITYDICTIONARY          0x8024
#define __RWIDENTITYSET                 0x8025
#define __RWINTEGER                     0x8026
#define __RWITERATOR                    0x8027
#define __RWORDERED                     0x8028
#define __RWORDEREDITERATOR             0x8029
#define __RWSEQUENCEABLE                0x802a
#define __RWSET                         0x802b
#define __RWSETITERATOR                 0x802c
#define __RWSLINK                       0x8031
#define __RWSLIST                       0x8032
#define __RWSLISTCOLLECTABLES           0x8033
#define __RWSLISTCOLLECTABLESITERATOR   0x8034
#define __RWSLISTCOLLECTABLESQUEUE      0x8035
#define __RWSLISTCOLLECTABLESSTACK      0x8036
#define __RWSLISTITERATOR               0x8037
//#define __RWSTRING                      0x802d
#define __RWSTRINGREF                   0x802e
//#define __RWSUBSTRING                   0x802f
#define __RWTIME                        0x8030
#define __RWTREENODE                    0x8038

// --- V4.0 new objects ---
#define __GBITVEC                       0x8040
#define __GSLIST                        0x8041
#define __GSLISTITERATOR                0x8042
#define __GSORTEDVECTOR                 0x8043
#define __RWBITREF                      0x8045
#define __RWBITVEC                      0x8046
#define __RWCACHEMANAGER                0x8048
#define __RWREGEXP                      0x8049
#define __RWSORTEDVECTOR                0x804a
#define __RWTOKENIZER                   0x804b

// --- V4.1 new objects ---
#define __RWDLLDICTIONARY               0x804c
#define __RWDLLSET                      0x804d
#define __RWFACTORYMANAGER              0x804e
#define __RWINSTANCEDATA                0x804f
#define __RWINSTANCEDATAMANAGER         0x8050
#define __RWINSTANCEMANAGER             0x8051
#define __RWMODEL                       0x8052
#define __RWPROCESS                     0x8053
#define __RWPROCESSMANAGER              0x8054
#define __RWREADMANAGER                 0x8055
#define __RWREADTABLE                   0x8056
#define __RWSTOREMANAGER                0x8057
#define __RWSTORETABLE                  0x8058
#define __RWTHREAD                      0x8059

// --- V5.0 new objects ---
#define __RWCSTRING                     0x802d  /* Recycled from RWString */
#define __RWCSUBSTRING                  0x802f  /* Recycled from RWSubString */
#define __RWBUFFER                      0x805a
#define __RWTEMPLATE                    0x805b
#define __RWVIRTUALREF                  0x805c
#define __RWDISKPAGEHEAP                0x805d
#define __RWHASHTABLE                   0x805e
#define __RWCOLLECTABLEIDASSOCIATION    0x805f

// --- V6.0 new objects ---
#define __RWCREGEXP                     0x8049  /* Recycled from RWRegexp */
#define __RWWSTRING                     0x8060
#define __RWWSUBSTRING                  0x8061
#define __RWCLASSIDASSOCIATION          0x8062
#define __RWSTRINGIDASSOCIATION         0x8063

// --- V7.0.x new IDs
#define __RWSTOREENTRY                  0x8064
#define __RWSTORESTRINGID               0x8065

/*
 * Misc. typedefs:
 */
typedef RWCollectable*  RWCollectableP;
typedef RWCollectable*  (*RWuserCreator)();
typedef long            RWstoredValue;  // Type of value in disk dictionaries.
typedef void*           RWvoid;         // Useful for arrays of void*.
typedef unsigned long   RWspace;        // Size of file allocations

/*
 * Pointer to functions to test and compare objects.
 */
typedef RWBoolean  (*RWtestGeneric)     (const void*, const void*);
typedef RWBoolean  (*RWtestCollectable) (const RWCollectable*, const void*);
typedef RWBoolean  (*RWtestCollectablePair)
     (const RWCollectable*, const RWCollectable*, const void*);
typedef int        (*RWcompare)         (const void*, const void*);

/*
 * Pointer to functions to apply operation to objects: 
 */
typedef void (*RWapplyGeneric)( void*, void*);
typedef void (*RWapplyCollectable)(RWCollectable*, void*);

/*
 * Prototype for the binary search routine, used for insertions and finds:
 */
extern "C" RWBoolean rwexport
RWbsearch(const void* key, const void* base, size_t nelem, size_t width, 
          RWcompare cmf, size_t&);

/*
 * Function used to hash a pointer value.  Choose an appropriate 
 * algorithm based on whether a pointer will fit into a word:
 */

#if RW_BYTES_PER_PTR != RW_BYTES_PER_WORD

# define RW_IDHASH_SHIFT ((RW_BYTES_PER_PTR - RW_BYTES_PER_WORD) << 3)
inline unsigned RWhashAddress(const void* a)
{
  // Algorithm for where word size is not equal to pointer size.
  // This works for compact and large models under MS-DOS as well.
  return (unsigned)((unsigned long)a                 )
       ^ (unsigned)((unsigned long)a >> RW_IDHASH_SHIFT);
# undef RW_IDHASH_SHIFT
}

#else

inline unsigned RWhashAddress(const void* a)
{
  // For other addresses, mix in some higher order bits
  // into the lower order bits:
  return (unsigned)a ^ ((unsigned)a>>3);
}

#endif

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

// Declarations for backwards compatibility:
#if defined(RWTOOLS4_COMPATIBLE) || defined(RWTOOLS5_COMPATIBLE)
#  include "rw/backward.h"
#endif

#endif /* __RWTOOLDEFS_H__ */
