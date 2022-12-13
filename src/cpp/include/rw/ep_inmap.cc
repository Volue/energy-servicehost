/***************************************************************************
 *
 * ep_inmap.cc - template definitions for
 *               RW_PMapAssoc<StdColl,RWColl,Key,T>
 * 
 * $Id: ep_inmap.cc 18623 2000-03-03 08:28:13Z sih $
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

template <class StdColl, class RWColl, class Key, class T>
void
RW_PMapAssoc<StdColl,RWColl,Key,T>::
clearAndDestroy()
{
  // first delete items we point to 
  for(iterator it = this->begin(); it != this->end(); ++it) {
    delete (Key*) (*it).first;  // cast away const for non-std impls
    delete (*it).second;
  }
  clear(); // then empty self
}

/*
 * Special function for testing equality of two hash collections.
 * The standard algorithm 'equal' is not suitable due to the
 * inherent un-orderedness of hash collections.
 */
template <class RWColl>
#if defined( __GNUG__ )
   bool hashMapCollEqual( const RWColl& h1, const RWColl& h2, 
                          const typename RWColl::container_type &c ) {

      RWPRECONDITION( h1.entries() == h2.entries() );

      typename RWColl::const_iterator i = c.begin();
      while( i != c.end() ) {
         typename RWColl::size_type num = c.count( ( *i ).first );
         if ( num != c.count( ( *i ).first ) ) {
            return false;
         }
         else {
            while( num-- ) ++i;
         }
      }
      return true;
   }
#else
   bool hashMapCollEqual( const RWColl& h1, const RWColl& h2 ) {
      RWPRECONDITION( h1.entries() == h2.entries() );

      typename RWColl::const_iterator i = h1.std().begin();
      while( i != h1.std().end() ) {
         typename RWColl::size_type num = h1.std().count( (*i ).first );
         if ( num != h2.std().count( ( *i ).first) ) {
            return false;
         }
         else {
            while( num-- ) ++i;
         }
      }
      return true;
   }
#endif
