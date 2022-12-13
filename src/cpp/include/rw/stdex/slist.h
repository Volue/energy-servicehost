#ifndef __STDEX_SLIST_H__
#define __STDEX_SLIST_H__

/***************************************************************************
 *
 * STL Sequence-compliant singly-linked list
 * 
 * $Id: slist.h 18623 2000-03-03 08:28:13Z sih $
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
 ****************************************************************************
 */

#include "rw/compiler.h"
#include "rw/defs.h"
#ifndef RW_HEADER_WITH_EXTENSION
#include <memory>
#include <iterator>
#include <algorithm> // equal filter
#else
#include <memory.hpp>
#include <iterator.hpp>
#include <algorithm.hpp> 
#endif /* RW_HEADER_WITH_EXTENSION */

#ifndef RW_ALLOCATOR_PLAIN
template <class T, class A>
#else
template <class T>
#endif
class rw_slist {
// internal class predeclarations
protected:
  struct Node;
public:
  class Iterator;
  class ConstIterator;
// friendships
friend class Iterator;
friend class ConstIterator;
protected:
  //////// scoped Node class /////////////////////////////////////////////////
  struct Node {                                                             //
    // data                                                                 //
    Node        *next;                                                      //
    T           item_;                                                      //
    // methods                                                              //
                                                                            //
    Node(const T& t, Node* nx) : item_(t), next(nx) {}                      //
    /* void     init(const T&t, Node* nx) {item_=t;next=nx;} */             //
                                                                            //
    Node**      mynext() { return &next; }                                  //
    T&          item() { return item_; }                                    //
    const T&    item() const { return item_; }                              //
  };                                                                        //
                                                                            //
  //////// end of scoped Node class //////////////////////////////////////////

public:      
// typedefs
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;

#ifndef RW_ALLOCATOR_PLAIN
#  ifdef RW_ALLOC_INTERFACE_STLV2X_HACK
  typedef  RW_SL_STD(allocator_interface)<RW_SL_STD(allocator)<Node>, Node>
                                                     allocator_type;
#  else
#    ifndef RW_ALLOCATOR_NO_REBIND
  typedef A::rebind<Node>::other                   allocator_type;
#    else
  typedef RW_SL_STD(allocator)<Node>                 allocator_type;
#    endif /*  RW_ALLOCATOR_NO_REBIND */
#  endif  /* RW_ALLOC_INTERFACE_STLV2X_HACK */
  typedef allocator_type::difference_type difference_type;
  typedef allocator_type::size_type size_type;
#else
  typedef RW_SL_STD(allocator) allocator_type;
  typedef RW_SL_STD(allocator)::difference_type difference_type;
  typedef RW_SL_STD(allocator)::size_type size_type;
#endif /* RW_ALLOCATOR_PLAIN */

  //////// scoped iterator classes ///////////////////////////////////////////
                                                                            //
  class Iterator : public                                                   //
#ifndef RW_ITER_NO_TAGS

#  ifndef RW_XITER_ARGS_REQ
        RW_SL_STD(iterator)<RW_SL_STD(forward_iterator_tag), T, allocator_type::difference_type> {  //
#  else
        RW_SL_STD(iterator)<RW_SL_STD(forward_iterator_tag), T, allocator_type::difference_type, T*, T&> {  //
#  endif

#else
        RW_SL_STD(forward_iterator)<T, allocator_type::difference_type> {              //
#endif
  friend class ConstIterator;                                               //
  public:                                                                   //
    Iterator(const Iterator& i) : prelink_(i.prelink()) {}                  //
    Iterator&           operator=(const Iterator& i)                        //
          { prelink() = i.prelink(); return *this; }                        //
                                                                            //
    bool                operator==(const Iterator& i) const                 //
                { return *prelink() == *i.prelink(); }                      //
                                                                            //
    bool                operator!=(const Iterator& i) const                 //
                                                                            //
                { return *prelink() != *i.prelink(); }                      //
                                                                            //
    T&          operator*() const { return (*prelink())->item(); }          //
                                                                            //
    Iterator&           operator++()            //++iter                    //
        { prelink() = (*prelink())->mynext(); return *this; }               //
    Iterator            operator++(int)         //iter++                    //
        { Node** tmp = prelink(); ++(*this); return Iterator(tmp); }        //
    // The following causes ambiguities under bcc45 for mysterious reasons: //
    //    operator bool() const { return isValid(); }                       //
    bool                isValid() const                                     //
        { return prelink() != 0 && *prelink() != 0; }                       //
                                                                            //
    Iterator()          : prelink_(0) {}                                    //
    Iterator(Node** spot) : prelink_(spot) {}                               //
    Node**              prelink() const { return prelink_; }                //
    Node**&             prelink() { return prelink_; }                      //
    bool                reset()                                             //
        { bool ret = isValid(); prelink_ = 0; return ret; }                 //
  private:                                                                  //
    Node**      prelink_;                                                   //
  };                                                                        //
                                                                            //
class ConstIterator : public                                                //
#ifndef RW_ITER_NO_TAGS

#  ifndef RW_XITER_ARGS_REQ
        RW_SL_STD(iterator)<RW_SL_STD(forward_iterator_tag), T, allocator_type::difference_type> {  //
#  else
        RW_SL_STD(iterator)<RW_SL_STD(forward_iterator_tag), T, allocator_type::difference_type, T*, T&> {  //
#  endif

#else
        RW_SL_STD(forward_iterator)<T, allocator_type::difference_type> {              //
#endif
  friend class Iterator;                                                    //
  public:                                                                   //
    ConstIterator(const ConstIterator& i) : prelink_(i.prelink()) {}        //
    ConstIterator(const Iterator& i) : prelink_(i.prelink()) {}             //
    ConstIterator&              operator=(const ConstIterator& i)           //
          { prelink() = i.prelink(); return *this; }                        //
    ConstIterator&              operator=(const Iterator& i)                //
          { prelink() = i.prelink(); return *this; }                        //
                                                                            //
    bool                operator==(const ConstIterator& i) const            //
                { return *prelink() == *i.prelink(); }                      //
    bool                operator!=(const ConstIterator& i) const            //
                { return *prelink() != *i.prelink(); }                      //
                                                                            //
    const T&            operator*() const { return (*prelink())->item(); }  //
                                                                            //
    ConstIterator&              operator++()            //++iter            //
        { prelink() = (*prelink())->mynext(); return *this; }               //
    ConstIterator               operator++(int)         //iter++            //
        { Node** tmp = prelink(); ++(*this); return ConstIterator(tmp); }   //
                                                                            //
    operator bool() const { return isValid(); }                             //
    bool                isValid() const                                     //
        { return prelink() != 0 && *prelink() != 0; }                       //
                                                                            //
    ConstIterator()             : prelink_(0) {}                            //
    ConstIterator(Node** spot) : prelink_(spot) {}                          //
    Node**              prelink() const { return prelink_; }                //
    Node**&             prelink() { return prelink_; }                      //
    bool                reset()                                             //
        { bool ret = isValid(); prelink_ = 0; return ret; }                 //
  private:                                                                  //
    Node**      prelink_;                                                   //
  };                                                                        //
  /////// end of scoped iterator classes /////////////////////////////////////

  typedef Iterator iterator;
  typedef ConstIterator const_iterator;




// allocation deallocation and assignment
  rw_slist();
  // rw_slist(size_type, const T& value = T()); /* don't require def ctor */


  
#ifndef RW_ALLOCATOR_PLAIN
  rw_slist(const rw_slist<T,A>&);
#else
  rw_slist(const rw_slist<T>&);
#endif
  rw_slist(ConstIterator begin, ConstIterator end); 
  rw_slist(const T* begin, const T* end); 
  ~rw_slist();



#ifndef RW_ALLOCATOR_PLAIN
  rw_slist<T,A>& operator=(const rw_slist<T,A>&);
  void swap(rw_slist<T,A>&);
#else
  rw_slist<T>& operator=(const rw_slist<T>&);
  void swap(rw_slist<T>&);
#endif

// accessors

  iterator begin()
        { return iterator(head_); }
  const_iterator begin() const
        { return const_iterator(head_); }
  iterator end()
        { return iterator(tail_); }
  const_iterator end() const
        { return const_iterator(tail_); }
  bool empty() const
        { return 0 == entries_; }
  size_type size() const
        { return entries_; }
  size_type max_size() const; /** use todd\'s templatized stuff? **/
  /** for the moment leave it undefined **/
  /** or use Alex's Allocator::max_size() which doesn't seem right
   ** since it limits us to one allocation buffer  **/
  T& front()
        { return *(begin()); }
  const T& front() const
        { return *(begin()); }
  T& back()  
/**     { return (*((dynamic_cast<Node*>)tail_))->item(); }
 * warning: this is a re-interpretation cast from a pointer to the
 * (first item in  a struct) to a pointer to the struct itself
 **/
        { return ((Node*)tail_)->item(); }
  const T& back() const 
/**{ return ((dynamic_cast<Node*>)tail_)->item(); }
 * same warning for the const version too
 **/
        { return ((Node*)tail_)->item(); }
// insert/erase
  void push_front(const T& t)
        { insert(begin(), t); }
  void push_back(const T& t)
        { insert(end(), t); }


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
#include "rw/stdex/slist.cc"
#undef RW_GET_INLINES_FOR_CLASS_BODY	  
#define RW_GOT_INLINES_FOR_CLASS_BODY_SLIST_CC

#else
  iterator insert(iterator loc, const T& val);
  void insert(iterator, const_iterator, const_iterator);
  void insert(iterator, const T*, const T*);

  // don't require def ctor:
  // void insert(iterator, size_type, const T& value = T());
  void insert(iterator, size_type, const T& value);
  rw_slist(size_type n, const T& value);
#endif // RW_BROKEN_NAME_RESOLUTION


  void pop_front()
        { erase(begin()); }
  iterator erase(iterator);
  iterator erase(iterator, iterator);

// special slist mutators
#ifndef RW_ALLOCATOR_PLAIN
  void splice(iterator pos, rw_slist<T,A> &donor)
        { cutPaste(pos,donor.size(),donor,donor.begin(),donor.end()); }
  void splice(iterator pos, rw_slist<T,A>& donor, iterator item);
  void splice(iterator pos, rw_slist<T,A>& donor,iterator begin, iterator end);
#else
  void splice(iterator pos, rw_slist<T> &donor)
        { cutPaste(pos,donor.size(),donor,donor.begin(),donor.end()); }
  void splice(iterator pos, rw_slist<T>& donor, iterator item);
  void splice(iterator pos, rw_slist<T>& donor,iterator begin, iterator end);
#endif
  void remove(const T& val);
/**  template <class UnaryPredicate>
 **  void remove_if(UnaryPredicate<T,bool>&);
 **/

  void unique(); // assumes equal elements are adjacent

/**  template <class BinaryPredicate>
 **  void unique(BinaryPredicate<T,T,bool>&);
 **/

#ifndef RW_ALLOCATOR_PLAIN
  void merge(rw_slist<T,A>&);
#else
  void merge(rw_slist<T>&);
#endif

/**  template <class Compare>
 **  void merge(rw_slist<T>&, Comparison<T>&);
 **/

  void reverse();
  void sort();

/**  tempate<class Compare>
 **  void sort(Comparison<T>&);
 **/

// RW internal debugging methods
#if 0
public:
  void showOff(const char *s= "LIST") const;
  void iprint(const char *p,const_iterator i,const char* t) const;
#endif
protected:
#ifndef RW_ALLOCATOR_PLAIN
  iterator cutPaste(iterator,size_type,rw_slist<T,A>&,iterator,iterator);
#else
  iterator cutPaste(iterator,size_type,rw_slist<T>&,iterator,iterator);
#endif
  void     sortPairs();
  iterator adjacentMerge(iterator,iterator,iterator,size_type,size_type);

/** (protected template members)
 **  void          sortPairs(Comparison<T>&);
 **  iterator adjacentMerge(iterator,iterator,iterator,
 **                      size_type,size_type, Comparison<T>&);
 **/
private:
  Node          *toHead_;
  Node          **head_;
  Node          **tail_;
  size_type     entries_;

  allocator_type nodeAllocator;
};

// global boolean operators on rw_slists

#ifndef RW_ALLOCATOR_PLAIN
template <class T, class A>
inline bool 
operator==(const rw_slist<T,A> &a, const rw_slist<T,A> &b)
{
  return a.size() == b.size() && RW_SL_STD(equal)(a.begin(),a.end(), b.begin());
}

template <class T, class A>
inline bool 
operator<(const rw_slist<T,A> &a, const rw_slist<T,A> &b )
{
  return RW_SL_STD(lexicographical_compare)(a.begin(), a.end(), b.begin(), b.end());
}
#else
template <class T>
inline bool 
operator==(const rw_slist<T> &a, const rw_slist<T> &b)
{
  return a.size() == b.size() && RW_SL_STD(equal)(a.begin(),a.end(), b.begin());
}

template <class T>
inline bool 
operator<(const rw_slist<T> &a, const rw_slist<T> &b )
{
  return RW_SL_STD(lexicographical_compare)(a.begin(), a.end(), b.begin(), b.end());
}
#endif

/*
 *   This is (possibly) the second include of the same ".cc" file. It 
 *   may have been included fron inside the class body above. Please see the
 *   comments in the class body above and at the beginning of the ".cc" file
 *   if you need to further understand what is accomplished with these macros.
 */

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("slist.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE

#include "rw/stdex/slist.cc"

#endif // RW_COMPILE_INSTANTIATE



#endif /* __STDEX_SLIST_H__ */
