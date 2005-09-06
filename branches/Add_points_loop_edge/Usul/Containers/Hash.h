
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Hash table with buckets.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_CONTAINERS_HASH_CLASS_H_
#define _USUL_CONTAINERS_HASH_CLASS_H_

#include <vector>
#include <list>
#include <hash_map>


namespace Usul {
namespace Containers {


template
<
  class T,
  class OuterContainer_ = std::vector<T>,
  class InnerContainer_ = std::list<T>
>
class Hash
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef OuterContainer_                         OuterContainer;
  typedef InnerContainer_                         InnerContainer;
  typedef typename InnerContainer::valud_type     value_type;
  typedef typename container_type::size_type      size_type;
  typedef Hash<T>                                 ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy construction.
  //
  /////////////////////////////////////////////////////////////////////////////

  Hash ( const ThisType &h )
  {
    _c.resize ( h.size() );
    std::copy ( h.container().begin(), h.container().end(), _c.begin() );
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < typename T2 > this_type &operator = ( const Hash<T2> &h )
  {
    this->resize ( h.size() );
    std::copy ( h.begin(), h.end(), this->begin() );
    return *this;
  }


private:

  container_type _c;
};


}; // namespace Containers
}; // namespace Usul


#endif // _USUL_CONTAINERS_HASH_CLASS_H_
