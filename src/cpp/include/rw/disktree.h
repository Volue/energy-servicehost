#ifndef __RWDISKTREE_H__
#define __RWDISKTREE_H__
/*
 * RWBTreeOnDisk -- disk-based B-Tree. Uses cache, filemanager.
 *
 * $Id: disktree.h 18623 2000-03-03 08:28:13Z sih $
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
 * Revision 1.1.1.1  2000/03/02 21:40:40  sih
 * RW710 MS VC 5.0 SP3
 *
 * Revision 7.11  1996/08/27 16:37:53  griswolf
 * Move memcmp_wrap from .h to .cpp file.
 *
 * Revision 7.10  1996/05/08 16:59:56  kyle
 * Added export pragma for Mac code fragments
 *
 * Revision 7.9  1996/02/18 01:39:54  griswolf
 * Replace tabs with spaces, per Rogue Wave standard.
 *
 * Revision 7.8  1996/01/05 17:44:29  pearson
 * Put in work-around for Watcom C++ for taking the address of memcmp
 *
 * Revision 7.7  1995/09/05 18:30:38  jims
 * Fix copyright macro
 *
 * Revision 7.6  1995/09/05  18:29:43  jims
 * Use new copyright macro
 *
 * Revision 7.5  1995/09/01  23:34:58  griswolf
 * Scopus #1057: Second of a two-part fix to make the code re-entrant.
 *
 * Revision 7.4  1995/09/01  00:43:34  griswolf
 * Scopus #1057: Fix problem with code that is non-reentrant. This is
 * step one of a two part fix.
 *
 * Revision 7.3  1995/07/18  22:59:29  griswolf
 * Scopus #2010: remove spurious include of mempool.h.
 *
 * Revision 7.2  1995/06/29  00:23:47  griswolf
 * Actually implement method nodeSize. Fix a const problem.
 *
 * Revision 7.1  1994/10/16  02:48:40  josh
 * Merged 6.1 and 7.0 development trees
 *
 * Revision 6.3  1994/07/12  18:46:56  vriezen
 * Update Copyright notice
 *
 * Revision 6.2  1994/06/02  16:43:44  foote
 * Port to Symantec 7.0 on Macintosh
 *
 * Revision 6.1  1994/04/15  19:09:50  vriezen
 * Move all files to 6.1
 *
 * Revision 2.13  1994/01/11  19:45:52  jims
 * ignoreNulls() now a "const" member function
 *
 * Revision 2.12  1994/01/03  18:54:44  griswolf
 * fix cacheCount() to have two prototypes
 *
 * Revision 2.11  1993/11/29  17:29:20  griswolf
 * added method rootLocation() to match the printed class reference.
 *
 * Revision 2.10  1993/09/14  18:19:50  griswolf
 * fixed problem handling imbedded nulls. Added unsigned long for flags.
 *
 * Revision 2.9  1993/09/11  22:13:11  griswolf
 * Changed constructor to match V6 reference manual
 *
 * Revision 2.8  1993/09/10  02:56:53  keffer
 * Switched RCS idents to avoid spurious diffs
 *
 * Revision 2.7  1993/05/18  21:47:27  keffer
 * Fixed badly formed RCS identifier
 *
 * Revision 2.6  1993/04/22  20:29:55  griswolf
 * Changed interface to return RWBoolean from some members, to accept
 * RWCString arguments in some of those members. Renamed one findValue
 * to replaceValue.
 *
 * Revision 2.5  1993/04/21  23:51:35  griswolf
 * Moved RWDiskTreeNode from disktree.h to disktree.cpp,
 * changed several inline functions to called functions.
 *
 * Revision 2.4  1993/04/13  07:14:49  randall
 * modified constructor to work on different style BTrees on disk
 *
 * Revision 2.3  1993/02/24  16:19:32  griswolf
 * Replaces the old RWBTreeOnDisk with a new implementation that
 * has considerable flexibility wrt keylength, file usage, order.
 *
 * 
 *    Rev 1.5   09 Jun 1992 12:43:48   KEFFER
 * Made retStatus public to satisfy stricter scoping rules.
 * 
 *    Rev 1.4   25 May 1992 15:48:56   KEFFER
 * Complete rewrite, improving efficiency and maintainability.
 * 
 *    Rev 1.3   18 Feb 1992 09:54:16   KEFFER
 * 
 *    Rev 1.2   28 Oct 1991 09:08:12   keffer
 * Changed inclusions to <rw/xxx.h>
 * 
 *    Rev 1.1   28 Jul 1991 12:36:04   keffer
 * No longer uses macro "Const"
 * 
 *    Rev 1.0   28 Jul 1991 08:14:14   keffer
 * Tools.h++ V4.0.5 PVCS baseline version
 *
 */

#include "rw/cacheman.h"
#include "rw/filemgr.h"
#include "rw/cstring.h"
#ifdef RDEBUG
#  include "rw/rstream.h"
#endif

typedef void    (*RWdiskTreeApply)  (const char*, RWstoredValue, void*);
extern "C" {
  typedef int   (*RWdiskTreeCompare)(const char*, const char*, size_t);
}

const RWstoredValue RWBTreeOnDiskCurrentVersion = 0x200;

class RWExport RWDiskTreeNode;
 
/****************************************************************
 *                                                              *
 *                      RWBTreeOnDisk                           *
 *                                                              *
 ****************************************************************/

#ifdef RW_PRAGMA_EXPORT
#pragma export on
#endif

class RWExport RWBTreeOnDisk {
friend class RWExport RWDiskTreeNode;

public:
  /* styleMode means:
   *  V6Style      -- Use the V 6.x and above version. Default
   *  V5Style      -- Use the V 5.x and below version.
   * createMode means:
   *  autoCreate   -- Look at start arg. If valid use, else create new
   *  create       -- Ignore start arg. Build new tree in the FileManger
   */
  enum styleMode        {V6Style, V5Style};
  enum createMode       {autoCreate, create};

  RWBTreeOnDisk(RWFileManager&,   // No default possible!
    unsigned cacheBlocks=10,      // how many nodes to cache in RAM
    createMode omode=autoCreate,  // use existing data in filemgr?
    unsigned keylen=16,           // default is 16 chars
    RWBoolean ignoreNull=FALSE,   // imbedded nulls ok? default to "no"
    RWoffset start=RWNIL,         // if RWNIL, use fmanager's start();
    styleMode smode=V6Style,      // default to current style
    unsigned order=10,            // default is 10-20 tree
    unsigned minorder=10          // default == order, may be less
    );  

  ~RWBTreeOnDisk();

  void                  applyToKeyAndValue(RWdiskTreeApply ap, void* x)
        { apl(baseInfo.rootLoc, ap, x); }
  RWoffset              baseLocation() const
        { return baseLoc; }
  void                  clear();
  unsigned              cacheCount() const { return cacheBlocks; }
  unsigned              cacheCount(unsigned blocks);
  RWBoolean             contains(const char* key) const
        {RWCString rK; RWstoredValue rV; return (findKeyAndValue(key, rK, rV) ? TRUE : FALSE); }
  unsigned long         entries() const;                // Total entries
  RWoffset              extraLocation(RWoffset off)
        { RWoffset r = baseInfo.extraLoc; baseInfo.extraLoc=off; return r; }
  RWBoolean             find(const char* key) const
        { return contains(key); }
  RWBoolean             findKey(const char* key, RWCString& retKey) const
        { RWstoredValue d; return findKeyAndValue(key,retKey, d); }
  RWBoolean             findKeyAndValue(const char* key, RWCString&, RWstoredValue&) const;
  RWstoredValue         findValue(const char* key) const
        { RWCString retK; RWstoredValue ret; return(findKeyAndValue(key,retK,ret)) ? ret : RWNIL ;}
  unsigned              height() const;
  RWBoolean             insertKeyAndValue(const char*key, RWstoredValue val);
  RWBoolean             isEmpty() const
        { return baseInfo.rootLoc == RWNIL; }
  unsigned              keyLength() const
        { return baseInfo.keylen; }
  unsigned              minOrder() const
        { return baseInfo.minorder; }
  unsigned              nodeSize() const { return nodeRefSize; }
  unsigned              occurrencesOf(const char* key) const
        { return contains(key) ? 1 : 0; }
  unsigned              order() const
        { return baseInfo.order; }
  void                  remove(const char* key)
        { RWstoredValue d; RWCString k; removeKeyAndValue(key,k,d); }
  RWBoolean             removeKeyAndValue(const char* key, RWCString& retKey, RWstoredValue& retVal);
  RWBoolean             removeKeyAndValue(const char* key, RWstoredValue& retVal)
        { RWCString retK; return removeKeyAndValue(key, retK, retVal); }
  RWBoolean             replaceValue(const RWCString& key, const RWstoredValue newval, RWstoredValue& oldVal);
  RWoffset              rootLocation() const
        { return baseLoc; }
  RWdiskTreeCompare     setComparison(RWdiskTreeCompare cf);
  RWstoredValue         version()
        { return (RWNIL == baseInfo.version) ? 0 : baseInfo.version; }
#ifdef RDEBUG
  void treePrint()    //for debugging purposes
    { cout <<endl<<"\t\tRWBTreeOnDisk::treePrint()"<<endl; tp(baseInfo.rootLoc);}
private:
  void tp(RWoffset);   //recursive function for treePrint
#endif

public:

  /******* used as signal during recursion unwrap ******/
  enum retStatus {more, success, ignored};

private:
  RWDiskTreeNode*       root;                   // root = first node in tree.

  // the following 2 members were used in a way that broke re-entrancy.
  // they are retained here to keep the binary image the same size.
  RWDiskTreeNode*       deadNode;               // was workNode
  RWoffset              deadWorkOff;            // was workOffset

  RWdiskTreeCompare     compareKeys;            // Compare function for keys

  // the following 3 members were used in a way that broke re-entrancy.
  // they are retained here to keep the binary image the same size.
  char*                 deadKey;                // was "moreKey"
  RWstoredValue         deadItem;               // was "moreItem"
  RWoffset              deadOffset;             // was "moreOffset"

  RWoffset              baseLoc;                // offset to baseInfo in fmgr

  // information about the nodeRef pseudo-struct:
  unsigned              nodeRefSize;

private:
  // recursive functions to: apply, delete, insert or remove
  // doApl is so we can take apply...\'s call to apl out of line... ugh.
  void                  apl(RWoffset, RWdiskTreeApply, void*);
  void                  doApl(RWoffset, RWDiskTreeNode&, RWdiskTreeApply, void*);
  void                  del(RWoffset, RWDiskTreeNode&);
  retStatus             ins(const char*, RWstoredValue, RWoffset,
                            RWoffset& workO, RWDiskTreeNode&,
                            RWCString&, RWstoredValue&, RWoffset&);
  retStatus             rem(const char* key, RWoffset start, RWCString&,
                            RWstoredValue&);
  // utility functions
  // opposite of splitNode
  void                  combineNodes(int,RWoffset,RWDiskTreeNode&,RWoffset&);
  // move a key,value pair from node at this offset left or right
  // leave the parent node in the reference arguments.
  void                  moveItLeft(int,RWoffset,
                                   RWoffset&,RWDiskTreeNode&);
  void                  moveItRight(int,RWoffset,
                                   RWoffset&,RWDiskTreeNode&);
  void                  readcache(RWoffset, RWDiskTreeNode*) const;
  void                  writecache(RWoffset, RWDiskTreeNode*);
  void                  readRoot();             // get root node from file
  // if node got too small, restore it to size by stealing from a neighbor
  retStatus             restoreNode(int,RWoffset&, RWDiskTreeNode&);
  // split a node at location loc passing new data back up call stack
  void                  splitNode(int loc, RWoffset&,RWDiskTreeNode&,
                                  RWCString&,RWstoredValue&,RWoffset&);
  void                  swapWithSuccessor(int,RWoffset&,RWDiskTreeNode&);
/*
 * The next two functions are private to prevent copies: only one
 * RWBTreeOnDisk can look at the data file at a time. Implementing locks so
 * that multiple accessors to the same data could co-exist might be 
 * interesting.
 */
  void                  operator=(const RWBTreeOnDisk&);
  RWBTreeOnDisk(const RWBTreeOnDisk&);
      
protected:
  RWFileManager*        fmgr;                   // knows where data is 
  RWCacheManager*       cmgr;                   // optimizes reads/writes
  unsigned              cacheBlocks;            // how many nodes cmgr keeps
  struct baseInfoStruct {
    RWoffset            rootLoc;        // offset to root node in file
    RWoffset            extraLoc;       // offset to "extra" info in file
    union {
      RWstoredValue     version;        // version of this B-Tree
      RWstoredValue     cacheFlag;      // RWNIL if entries() not cached
    };
    unsigned long       entries;        // number of items in this B-Tree
    unsigned            keylen;         // length of keys
    unsigned            order;          // 1/2 max  entries per node
    unsigned            minorder;       // fewest allowed entries per node
    RWoffset            reserved1;      // In case we ever want it
    RWstoredValue       reserved2;      // ditto
    unsigned long       flags;          // holds ignoreNulls for now
  } baseInfo;
protected:
  /* ignoreNulls() prototypes getting individual flags from flags */
  RWBoolean             ignoreNulls() const
  { return baseInfo.flags & 1 ? TRUE : FALSE; }
  void                  infoInit(RWstoredValue version,
                                  unsigned keylen,
                                  unsigned order,
                                  unsigned minorder,
                                  RWBoolean ignoreNull
                                 );
  void                  infoReInit();
  void                  readInfo();
  void                  startup();
  void                  writeInfo() const;      // has version knowledge

};    

#ifdef RW_PRAGMA_EXPORT
#pragma export off
#endif

#endif /*  __RWDISKTREE_H__ */
