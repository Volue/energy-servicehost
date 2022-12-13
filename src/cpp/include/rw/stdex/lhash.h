
#ifndef _STDEX_LHASH_TABLE_H_
#define _STDEX_LHASH_TABLE_H_

/***************************************************************************
 *
 * Hash table implemented as a vector of iterators into an slist, with
 * "chunky" insertion, so that all equal items are adjacent.
 *
 * This class is intended primarily to be used as an underlying
 * implementation. As it is currently written, there are public
 * methods, since that facilitates testing. It may be that the better
 * course is to make all methods protected or private, and then
 * require that interface classes be derived or friends.
 * 
 * There is a subtle change from the "standard meaning" for the methods
 * lower_bound(key) and upper_bound(key).
 * Since hash tables are intrinsically unordered, we have relaxed the 
 * meaning very slightly so that:
 *    lower_bound(key) returns an iterator which points either
 *      "just past" the hash table slot where key would have been but was not
 *         or
 *      the "first" iterator which references something EQ to key.
 *
 *    upper_bound(key) returns an iterator which points either
 *      "just past" the hash table slot where key would have been but was not
 *         or
 *      "just past" the "last" iterator which references something EQ to key
 *
 * Note that all items which are EQ to each other are in consecutive
 * iterator locations. Thus, while equal_range(key) is useful, no other
 * range (except the entire range of the hash table) is likely to be
 * meaningful.
 *
 */

#include "rw/compiler.h"
#ifndef RW_HEADER_WITH_EXTENSION
#include <memory>
#include <functional>        // stl binary-function
#include <vector>            // stl vector
#include <iterator>          // stl forward_iterator for the tags
#else
#include <memory.hpp>
#include <functional.hpp>        
#include <vector.hpp>            
#include <iterator.hpp>          
#endif  /* RW_HEADER_WITH_EXTENSION */

#include "rw/stdex/slist.h"  // value based slist

// because finding a match requires an "equalitor" the class is
// templatized on "EQ"
// because there must be a hash function, the class is templatized
// on "Hash"

struct LHashTable_index_type_wrapper { unsigned long l; };

#ifndef RW_ALLOCATOR_PLAIN
template <class T, class Hash, class EQ, class A >
#else
template <class T, class Hash, class EQ >
#endif
class LHashTable 
{
public:

  struct bucket_t;
///// typedefs /////
  typedef RW_SL_STD(pair)<T,unsigned long>    node_t;
  typedef T                                   value_type;
  typedef T&                                  reference;
  typedef const T&                            const_reference;

#ifdef RW_ALLOC_INTERFACE_HACK
  typedef RW_SL_STD(allocator_interface)<RW_SL_STD(allocator), rw_slist<node_t> >
                                              list_allocator_t;
  typedef RW_SL_STD(allocator_interface)<RW_SL_STD(allocator), node_t >
                                              list_node_allocator_t;
  typedef RW_SL_STD(allocator_interface)<RW_SL_STD(allocator),
                                           RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)> >
                                              vector_allocator_t;
  typedef RW_SL_STD(allocator_interface)<RW_SL_STD(allocator), bucket_t>
                                              vector_bucket_allocator_t;
#else
#  ifdef RW_ALLOC_INTERFACE_STLV2X_HACK

  typedef RW_SL_STD(allocator)<node_t>      list_node_allocator_t;

  typedef RW_SL_STD(allocator_interface)<RW_SL_STD(allocator)<rw_slist<node_t,list_node_allocator_t> >,
                                         rw_slist<node_t,list_node_allocator_t> >        
                                            list_allocator_t;

 
  typedef RW_SL_STD(allocator)<bucket_t>    vector_bucket_allocator_t;

  typedef RW_SL_STD(allocator_interface)
  <RW_SL_STD(allocator)<RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> >,
                          RW_SL_STD(vector)<bucket_t, vector_bucket_allocator_t> >                                                          vector_allocator_t;

#  else
#    ifndef RW_ALLOCATOR_NO_REBIND
    typedef A::rebind<node_t>::other                              list_node_allocator_t;

#ifndef RW_ALLOCATOR_PLAIN 
    typedef A::rebind<rw_slist<node_t, list_node_allocator_t> >::other list_allocator_t;
#else
    typedef A::rebind<rw_slist<node_t> >::other list_allocator_t;
#endif
    typedef A::rebind<RW_SL_STD(vector)<bucket_t> >::other        vector_allocator_t;
    typedef A::rebind<bucket_t>::other                            vector_bucket_allocator_t;
#    else
    typedef RW_SL_STD(allocator)<node_t>                 list_node_allocator_t;
    typedef RW_SL_STD(allocator)<rw_slist<RW_SL_STD(pair)<T,unsigned long>, list_node_allocator_t> >
		list_allocator_t;
    typedef RW_SL_STD(allocator)<bucket_t>               vector_bucket_allocator_t;
    typedef RW_SL_STD(allocator)<RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> >
		vector_allocator_t;
#    endif
#  endif
#endif

/* typename workaround */
#ifdef RW_NO_TYPENAME
 
#ifndef RW_ALLOCATOR_PLAIN
  typedef rw_slist<RW_SL_STD(pair)<T,unsigned long>, list_node_allocator_t>::iterator rw_slist_pA_iterator;
#else
  typedef rw_slist<RW_SL_STD(pair)<T,unsigned long> >::iterator rw_slist_p_iterator;
#endif
 
#endif

struct bucket_t {
    bool                        valid_;
#ifndef RW_ALLOCATOR_PLAIN
    typename rw_slist<RW_SL_STD(pair)<T,unsigned long>, list_node_allocator_t>::iterator chain_;

#ifdef RW_NO_TYPENAME
    bucket_t(bool v, const rw_slist_pA_iterator& c)
#else
    bucket_t(bool v, const typename rw_slist<RW_SL_STD(pair)<T,unsigned long>, list_node_allocator_t>::iterator& c)
#endif
      : valid_(v), chain_(c) {}

#ifdef RW_NO_TYPENAME
    rw_slist_pA_iterator&
#else
    typename rw_slist<RW_SL_STD(pair)<T,unsigned long>, list_node_allocator_t>::iterator& 
#endif
    chain() {return chain_;}

#ifdef RW_NO_TYPENAME
    rw_slist_pA_iterator
#else
    typename rw_slist<RW_SL_STD(pair)<T,unsigned long>, list_node_allocator_t>::iterator 
#endif
    chain() const {return chain_;}

#else

    typename rw_slist<RW_SL_STD(pair)<T,unsigned long> >::iterator chain_;

#ifdef RW_NO_TYPENAME
    bucket_t(bool v, const rw_slist_p_iterator& c)
#else
    bucket_t(bool v, const typename rw_slist<RW_SL_STD(pair)<T,unsigned long> >::iterator& c)
#endif
      : valid_(v), chain_(c) {}

#ifdef RW_NO_TYPENAME
    rw_slist_p_iterator&
#else
    typename rw_slist<RW_SL_STD(pair)<T,unsigned long> >::iterator& 
#endif
    chain() {return chain_;}

#ifdef RW_NO_TYPENAME
    rw_slist_p_iterator
#else
    typename rw_slist<RW_SL_STD(pair)<T,unsigned long> >::iterator 
#endif
    chain() const {return chain_;}

#endif
    bucket_t() : valid_(false) {}
    operator bool() const { return valid_; }
    bool&       valid() { return valid_; }

    ~bucket_t() {}

  };

#ifndef RW_ALLOCATOR_PLAIN
  typedef rw_slist<node_t,list_node_allocator_t>   list_t;
  typedef RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t> table_t;
#else
  typedef rw_slist<node_t>                    list_t;
  typedef RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)>          table_t;
#endif
  typedef typename list_t::size_type          size_type;
  typedef typename list_t::difference_type    difference_type;
  typedef EQ                                  key_equal_type;
  typedef Hash                                key_hash_type;

/* typename workaround */
#ifdef RW_NO_TYPENAME
 
#ifndef RW_ALLOCATOR_PLAIN
  typedef rw_slist<node_t,list_node_allocator_t>::iterator rw_slist_nA_iterator;
  typedef LHashTable<T,Hash,EQ,A>::const_reference LHashTable_THEA_const_reference;
  typedef LHashTable<T,Hash,EQ,A>::size_type LHashTable_THEA_size_type;
  typedef LHashTable<T,Hash,EQ,A> LHashTable_THEA;
#else
  typedef rw_slist<node_t>::iterator rw_slist_n_iterator;
  typedef LHashTable<T,Hash,EQ>::const_reference LHashTable_THE_const_reference;
  typedef LHashTable<T,Hash,EQ>::size_type LHashTable_THE_size_type;
  typedef LHashTable<T,Hash,EQ> LHashTable_THE;
#endif

#endif


  ///// nested Iterator classes //////////////////////////////////////////////
                                                                            //
  class ConstIterator;                                                      //
#ifndef RW_ITER_NO_TAGS
#  ifndef RW_XITER_ARGS_REQ
  class Iterator : public RW_SL_STD(iterator)<RW_SL_STD(forward_iterator_tag),T,difference_type>  //
#  else
  class Iterator : public RW_SL_STD(iterator)<RW_SL_STD(forward_iterator_tag),T,difference_type, T*, T&>  //
#  endif
#else
  class Iterator : public RW_SL_STD(forward_iterator)<T,difference_type>               //
#endif
  {
#ifndef RW_ALLOCATOR_PLAIN
    friend LHashTable<T,Hash,EQ,A>;                                         //
#else
    friend class LHashTable<T,Hash,EQ>;                                     //
#endif
    typename list_t::iterator iter_;                                        //
    friend class ConstIterator;                                             //
  public:                                                                   //
    Iterator() {}                                                           //
#ifndef RW_ALLOCATOR_PLAIN

#ifdef RW_NO_TYPENAME
    Iterator(const rw_slist_nA_iterator& i)
#else
    Iterator(const typename rw_slist<node_t, list_node_allocator_t>::iterator& i) 
#endif
    : iter_(i) {}  // 

#else

#ifdef RW_NO_TYPENAME
    Iterator(const rw_slist_n_iterator& i)
#else
    Iterator(const typename rw_slist<node_t>::iterator& i) 
#endif
    : iter_(i) {}    // 

#endif
    Iterator(const Iterator& i) : iter_(i.iter_) {}                         //
    ~Iterator() {}                                                          //
                                                                            //
    reference           operator*()  const { return (*iter_).first; }       //    
    Iterator&           operator++()       { ++iter_; return *this; }       //
    Iterator            operator++(int)                                     //
    { Iterator t(*this); ++(*this); return t; }                             //
                                                                            //
    bool                operator==(const Iterator& rhs) const               //
    { return iter_ == rhs.iter_; }                                          //
                                                                            //
    bool                operator==(const ConstIterator& rhs) const          //
    { return rhs == *this; }                                                //
                                                                            //

 //   Dot operator used here to access != to prevent conflict with 
 //   C++ Std. Lib's templated != operator                                      

    bool                operator!=(const Iterator& rhs) const               //
    { return iter_.operator!=(rhs.iter_); }                                 //
                                                                            //
    bool                operator!=(const ConstIterator& rhs) const          //
    { return rhs != *this; }                                                //
                                                                            //
    Iterator&           operator=(const Iterator& rhs)                      //
    { iter_ = rhs.iter_; return *this; }                                    //
  };                                                                        //
                                                                            //
#ifndef RW_ITER_NO_TAGS
#  ifndef RW_XITER_ARGS_REQ
  class ConstIterator : public RW_SL_STD(iterator)<RW_SL_STD(forward_iterator_tag),T,difference_type>//
#  else
  class ConstIterator : public RW_SL_STD(iterator)<RW_SL_STD(forward_iterator_tag),T,difference_type, T*, T&>//
#  endif
#else
  class ConstIterator : public RW_SL_STD(forward_iterator)<T,difference_type>          //
#endif
  {
    typename list_t::iterator iter_;                                        //
#ifndef RW_ALLOCATOR_PLAIN
    friend LHashTable<T,Hash,EQ,A>;                                         //
#else
    friend class LHashTable<T,Hash,EQ>;                                     //
#endif
  public:                                                                   //
    ConstIterator() {}                                                      //
#ifndef RW_ALLOCATOR_PLAIN

#ifdef RW_NO_TYPENAME
    ConstIterator(const rw_slist_nA_iterator& i)
#else
    ConstIterator(const typename rw_slist<node_t, list_node_allocator_t>::iterator& i) 
#endif
    : iter_(i)        //

#else

#ifdef RW_NO_TYPENAME
    ConstIterator(const rw_slist_n_iterator& i)
#else
    ConstIterator(const typename rw_slist<node_t>::iterator& i) 
#endif
    : iter_(i)           //

#endif
    {}                                                                      //
    ConstIterator(const ConstIterator& i) : iter_(i.iter_) {}               //
    ConstIterator(const Iterator& i) : iter_(i.iter_) {}                    //
    ~ConstIterator() {}                                                     //
                                                                            //
    const_reference             operator*() const                           //
    { return (*iter_).first; }                                              //
    ConstIterator&              operator++() { ++iter_; return *this; }     //
    ConstIterator               operator++(int)                             //
    { ConstIterator t(*this); ++(*this); return t; }                        //
                                                                            //
    bool                operator==(const ConstIterator& rhs) const          //
    { return iter_ == rhs.iter_; }                                          //
                                                                            //
    bool                operator==(const Iterator& rhs) const               //
    { return iter_ == rhs.iter_; }                                          //
                                                                            //

 //  Dot operator used here to access != to prevent conflict with 
 //  C++ Std. Lib's templated != operator       

    bool                operator!=(const ConstIterator& rhs) const          //
    { return iter_.operator!=(rhs.iter_); }                                 //


 //  Dot operator used here to access != to prevent conflict with 
 //  C++ Std. Lib's templated != operator       

    bool                operator!=(const Iterator& rhs) const               //
    { return iter_.operator!=(rhs.iter_); }                                 //
                                                                            //
    ConstIterator&              operator=(const ConstIterator& rhs)         //
    { iter_ = rhs.iter_; return *this; }                                    //
                                                                            //
    ConstIterator&              operator=(const Iterator& rhs)              //
    { iter_ = rhs.iter_; return *this; }                                    //
  };                                                                        //
  ///// end of nested Iterator classes ///////////////////////////////////////
      

  typedef Iterator iterator; // users may not alter table directly
  typedef ConstIterator const_iterator;

  /*
   * There is no standard specification for hash tables in the STL,
   * but they are more similar to Associative Containers than anything
   * else.  STL containers are supposed to have a typedef key_compare
   * for their comparitors.  Since items in a hash table are not ordered
   * in that way, we are providing a trivial comparitor that always
   * returns false (from STL's point of view that means everything is
   * equivalent).  This is mainly to guard against over-achieving
   * compilers that insist on compiling even those template functions 
   * that are never used.  This will help in using hashed collections
   * in code that makes use of their commonality of interface with
   * the standard Associative Containers.
   */

   struct degenerative_compare : public RW_SL_STD(binary_function)<T, T, bool>
   {
     bool operator() (const T&, const T&) const {

#ifndef __TURBOC__
        return false;
#else
        RWBoolean tmpry = false;
        return tmpry;
#endif

     }
   };

   typedef degenerative_compare key_compare;
   
///// constructors /////
/*
 * Unsolved problem with msvc precludes the default params 
 * commented out below.
 */



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
#include "rw/stdex/lhash.cc"
#undef RW_GET_INLINES_FOR_CLASS_BODY	  
#define RW_GOT_INLINES_FOR_CLASS_BODY_LHASH_CC

#else  // defined RW_BROKEN_NAME_RESOLUTION

#if defined(RW_NO_DEFAULT_TEMPLATE_ARGS)
  LHashTable();
  LHashTable(size_type sz);
  LHashTable(size_type sz, const Hash& h);

  LHashTable(ConstIterator first, ConstIterator bound);
  LHashTable(ConstIterator first, ConstIterator bound, size_type);
  LHashTable(ConstIterator first, ConstIterator bound, size_type, const Hash&);
  
  LHashTable(const T* first, const T* bound);
  LHashTable(const T* first, const T* bound,size_type);
  LHashTable(const T* first, const T* bound,size_type, const Hash&);
#endif /* RW_NO_DEFAULT_TEMPLATE_ARGS */
  LHashTable(size_type sz RW_DEFAULT_ARG(1024),
             const Hash& h RW_DEFAULT_ARG(Hash()),
             const EQ& equals RW_DEFAULT_ARG(EQ()));

#ifndef RW_ALLOCATOR_PLAIN
  LHashTable(const LHashTable<T,Hash,EQ,A>&);
#else
  LHashTable(const LHashTable<T,Hash,EQ>&);
#endif

  LHashTable(ConstIterator first, ConstIterator bound, size_type sz RW_DEFAULT_ARG(1024),
            const Hash& h RW_DEFAULT_ARG(Hash()), const EQ& equals RW_DEFAULT_ARG(EQ())
           );

  LHashTable(const T* first, const T* bound,size_type sz RW_DEFAULT_ARG(1024),
            const Hash& h RW_DEFAULT_ARG(Hash()), const EQ& equals RW_DEFAULT_ARG(EQ())
           );

  ~LHashTable();

  ConstIterator erase(ConstIterator first, ConstIterator bound); 
  Iterator erase(Iterator first, Iterator bound);                
  Iterator erase(Iterator iter);                                 
  ConstIterator erase(ConstIterator iter);  
  size_type erase(const_reference key);
  

#endif // #ifndef RW_BROKEN_NAME_RESOLUTION   


///// operators /////
#ifndef RW_ALLOCATOR_PLAIN
  LHashTable<T,Hash,EQ,A>& operator=(const LHashTable<T,Hash,EQ,A>& t);

  bool          operator==(const LHashTable<T,Hash,EQ,A>& t) const
  { return size() == t.size() && testEqual(t); }
#else
  LHashTable<T,Hash,EQ>& operator=(const LHashTable<T,Hash,EQ>& t);
    
  bool          operator==(const LHashTable<T,Hash,EQ>& t) const
  { return size() == t.size() && testEqual(t); }
#endif

///// various public methods /////
  Iterator
    begin()
    { return Iterator(list_->begin()); }

  ConstIterator
    begin() const
    { return ConstIterator(list_->begin()); }

  size_type
    capacity() const { return table_->size(); } // number of buckets

  void
    clear();

  size_type
#ifndef RW_ALLOCATOR_PLAIN

#ifdef RW_NO_TYPENAME
    count(LHashTable_THEA_const_reference) const;
#else
    count(typename LHashTable<T,Hash,EQ,A>::const_reference) const;
#endif

#else

#ifdef RW_NO_TYPENAME
    count(LHashTable_THE_const_reference) const;
#else
    count(typename LHashTable<T,Hash,EQ>::const_reference) const;
#endif

#endif
  bool
    empty() const { return size() == 0; }

  Iterator
    end()
    { return Iterator(list_->end()); }

  ConstIterator
    end() const
    { return ConstIterator(list_->end()); }

  RW_SL_STD(pair)<Iterator,Iterator>

#ifndef RW_ALLOCATOR_PLAIN

#ifdef RW_NO_TYPENAME
    equal_range(LHashTable_THEA_const_reference key)
#else
    equal_range(typename LHashTable<T,Hash,EQ,A>::const_reference key)
#endif

#else   

#ifdef RW_NO_TYPENAME
    equal_range(LHashTable_THE_const_reference key)
#else
    equal_range(typename LHashTable<T,Hash,EQ>::const_reference key)
#endif

#endif
{
  typename table_t::iterator slot = index(key);
  typename list_t::iterator lower = list_->end();
  typename list_t::iterator upper = list_->end();
  bool hit;
  if(slot != table_->end() && *slot) {
    lower = chunkyLowerBound((*slot).chain(), list_->end(), key, hit);
    upper = lower;
    if(hit)
      upper = chunkyUpperBound(lower,list_->end(), key);
  }
  return RW_SL_STD(pair)<Iterator,Iterator>(lower,upper);
}

  RW_SL_STD(pair)<ConstIterator,ConstIterator>
#ifndef RW_ALLOCATOR_PLAIN

#ifdef RW_NO_TYPENAME
    equal_range(LHashTable_THEA_const_reference key) const
#else
    equal_range(typename LHashTable<T,Hash,EQ,A>::const_reference key) const
#endif

#else

#ifdef RW_NO_TYPENAME
    equal_range(LHashTable_THE_const_reference key) const
#else
    equal_range(typename LHashTable<T,Hash,EQ>::const_reference key) const
#endif

#endif
{
  typename table_t::const_iterator slot = index(key);
  typename list_t::iterator lower = list_->end();
  typename list_t::iterator upper = list_->end();
  bool hit;
  if(slot != table_->end() && *slot) {
    lower = chunkyLowerBound((*slot).chain(), list_->end(), key, hit);
    upper = lower;
    if(hit)
      upper = chunkyUpperBound(lower,list_->end(), key);
  }
  return RW_SL_STD(pair)<ConstIterator,ConstIterator>(lower,upper);
}


Iterator                      //  return successor to range
    erase(RW_SL_STD(pair)<Iterator,Iterator> range)
    { return erase(range.first, range.second); }

  float
    fill_ratio() const { return ((float)size())/((float)capacity()); }

#ifndef RW_ALLOCATOR_PLAIN

#ifdef RW_NO_TYPENAME
  Iterator
    find(LHashTable_THEA_const_reference);
 
  ConstIterator
    find(LHashTable_THEA_const_reference) const;
 
  bool
    insertMaybe(
                LHashTable_THEA_const_reference key,
                Iterator& ret, // holds returned iterator
                bool isSingle
                );
#else
  Iterator
    find(typename LHashTable<T,Hash,EQ,A>::const_reference);

  ConstIterator
    find(typename LHashTable<T,Hash,EQ,A>::const_reference) const;

  bool
    insertMaybe(
                typename LHashTable<T,Hash,EQ,A>::const_reference key,
                Iterator& ret, // holds returned iterator
                bool isSingle
                );
#endif

#else

#ifdef RW_NO_TYPENAME
  Iterator
    find(LHashTable_THE_const_reference);
 
  ConstIterator
    find(LHashTable_THE_const_reference) const;
 
  bool
    insertMaybe(
                LHashTable_THE_const_reference key,
                Iterator& ret, // holds returned iterator
                bool isSingle
                );
#else
  Iterator
    find(typename LHashTable<T,Hash,EQ>::const_reference);

  ConstIterator
    find(typename LHashTable<T,Hash,EQ>::const_reference) const;

  bool
    insertMaybe(
                typename LHashTable<T,Hash,EQ>::const_reference key,
                Iterator& ret, // holds returned iterator
                bool isSingle
                );
#endif

#endif

  size_type
    insertMaybe(
                ConstIterator first,
                ConstIterator bound,
                Iterator& ret, // holds returned iterator
                bool isSingle
                )
  {
    size_type count(0);
    while(first != bound)  {
      count += (insertMaybe(*first,ret,isSingle)) ? 1 : 0;
      ++first;
    }
    return count;
  }

  size_type
    insertMaybe(
                const T* first,
                const T* bound,
                Iterator& ret, // holds returned iterator
                bool isSingle
                )
  {
    size_type count(0);
    while(first != bound) 
      count += (insertMaybe(*first++,ret,isSingle)) ? 1 : 0;
    return count;
  }

#ifndef RW_ALLOCATOR_PLAIN

#ifdef RW_NO_TYPENAME
  Iterator
    lower_bound(LHashTable_THEA_const_reference);
 
  ConstIterator
    lower_bound(LHashTable_THEA_const_reference) const;
 
//  resize() fills a new table with data, rehashing everything,
//  except that resize(0) is a no-op.
  void
    resize(LHashTable_THEA_size_type);
#else
  Iterator
    lower_bound(typename LHashTable<T,Hash,EQ,A>::const_reference);

  ConstIterator
    lower_bound(typename LHashTable<T,Hash,EQ,A>::const_reference) const;

//  resize() fills a new table with data, rehashing everything, 
//  except that resize(0) is a no-op.
  void
    resize(typename LHashTable<T,Hash,EQ,A>::size_type);
#endif

#else

#ifdef RW_NO_TYPENAME
  Iterator
    lower_bound(LHashTable_THE_const_reference);
 
  ConstIterator
    lower_bound(LHashTable_THE_const_reference) const;
 
//  resize() fills a new table with data, rehashing everything,
//  except that resize(0) is a no-op.
  void
    resize(LHashTable_THE_size_type);
#else
  Iterator
    lower_bound(typename LHashTable<T,Hash,EQ>::const_reference);

  ConstIterator
    lower_bound(typename LHashTable<T,Hash,EQ>::const_reference) const;

//  resize() fills a new table with data, rehashing everything, 
//  except that resize(0) is a no-op.
  void
    resize(typename LHashTable<T,Hash,EQ>::size_type);
#endif

#endif

  size_type
    size() const { return list_->size(); }

#ifndef RW_ALLOCATOR_PLAIN

#ifdef RW_NO_TYPENAME
  void
    swap(LHashTable_THEA& t);
 
  Iterator
    upper_bound(LHashTable_THEA_const_reference);
 
  ConstIterator
    upper_bound(LHashTable_THEA_const_reference) const;
#else
  void
    swap(LHashTable<T,Hash,EQ,A>& t);

  Iterator
    upper_bound(typename LHashTable<T,Hash,EQ,A>::const_reference);

  ConstIterator
    upper_bound(typename LHashTable<T,Hash,EQ,A>::const_reference) const;
#endif

#else

#ifdef RW_NO_TYPENAME
  void
    swap(LHashTable_THE& t);
  Iterator
    upper_bound(LHashTable_THE_const_reference);
 
  ConstIterator
    upper_bound(LHashTable_THE_const_reference) const;
#else
  void
    swap(LHashTable<T,Hash,EQ>& t);
  Iterator
    upper_bound(typename LHashTable<T,Hash,EQ>::const_reference);

  ConstIterator
    upper_bound(typename LHashTable<T,Hash,EQ>::const_reference) const;
#endif

#endif

  key_hash_type
    key_hash() const { return hash_; }

  key_equal_type
    key_equal() const { return eq_; }

//////////////////////////////////////////////////////////////////////////
///// protected and private methods //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

protected:

  typename list_t::iterator
    chunkyLowerBound(
                     typename list_t::iterator start,
                     typename list_t::iterator bound,
                     const_reference key,
                     bool& hit
                    ) const
{
  hit = false;
  if(0 == capacity())
    return bound;
  // else
  size_type slot = ((*start).second) % capacity();
  while(start != bound && ((*start).second)%capacity() == slot) {
    if(eq_((*start).first,key)) {
      hit = true;
      break;
    }
    ++start;
  }
  return start;
}

  typename list_t::iterator
    chunkyUpperBound(
                     typename list_t::iterator start,
                     typename list_t::iterator bound,
                     const_reference key
                    ) const
{
  if(0 == capacity())
    return bound;
  // else
  size_type slot = ((*start).second) % capacity();
  bool dum;
  start = chunkyLowerBound(start, bound, key, dum);

  while(start != bound && ((*start).second)%capacity() == slot) {
    if(! eq_((*start).first,key))
      break;
    ++start;
  }
  return start;
}

  typename table_t::iterator
    index(LHashTable_index_type_wrapper u) const
    {
			unsigned long hv = u.l;
      if(0 == capacity())
        return table_->end();
      else
        return table_->begin() + (hv % capacity());
    }

  typename table_t::iterator
    index(const_reference t) const { 
			LHashTable_index_type_wrapper u; 
			u.l = hash_(t);
    	return index(u);
    }
  	

#if __SUNPRO_CC!=0x420 &&  !defined(RW_NO_STL) 
#ifndef RW_ALLOCATOR_PLAIN
  bool
    testEqual(const LHashTable<T,Hash,EQ,A>& t) const;
#else
  bool
    testEqual(const LHashTable<T,Hash,EQ>& t) const;
#endif
#else
 bool testEqual(const LHashTable<T,Hash,EQ>& t) const
{
/** precondition(size() == t.size()) **/
  ConstIterator i = begin();
  
  while(i != end()) {
    size_type num = t.count(*i);
    if(num != count(*i)) {
      return false;
    }
    else {
      while(num-- ) ++i;
    }
  }
  return true;
}
#endif /* !SunPro*/

private:
  list_allocator_t                        listAllocator;
  vector_allocator_t                      vectorAllocator;
#ifndef RW_ALLOCATOR_PLAIN
  rw_slist<node_t,list_node_allocator_t>       *list_;
  RW_SL_STD(vector)<bucket_t,vector_bucket_allocator_t>     *table_;
#else
  rw_slist<node_t>                        *list_;
  RW_SL_STD(vector)<bucket_t,RW_SL_STD(allocator)>              *table_;
#endif
  EQ                                      eq_;
  Hash                                    hash_;
};

#if defined(_AIX) && !defined(RW_COMPILE_INSTANTIATE)
#pragma implementation ("lhash.cc")
#endif

#ifdef RW_COMPILE_INSTANTIATE

/*
 *   This is (possibly) the second include of the same ".cc" file. It 
 *   may have been included fron inside the class body above. Please see the
 *   comments in the class body above and at the beginning of the ".cc" file
 *   if you need to further understand what is accomplished with these macros.
 */

#include "rw/stdex/lhash.cc"
#endif // RW_COMPILE_INSTANTIATE


#endif /* _STDEX_LHASH_TABLE_H_ */

