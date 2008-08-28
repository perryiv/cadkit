
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A class that queries for T::IID before adding to the container.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_CONTAINERS_UNKNOWNS_H_
#define _USUL_CONTAINERS_UNKNOWNS_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include <vector>

namespace Usul {
namespace Containers {


template < class T > class Unknowns
{
public:
  typedef T value_type;
  typedef typename T::RefPtr  PointerType;
  typedef std::vector<PointerType> Container;
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef typename Container::iterator iterator;
  typedef typename Container::const_iterator const_iterator;
  typedef typename Container::size_type size_type;


  /// Contructor.
  Unknowns() : _container(),
    _mutex ( Mutex::create() )
  {
  }

  /// Copy Contructor.
  Unknowns( const Unknowns& rhs ) : 
    _container ( rhs._container ),
    _mutex ( Mutex::create() )
  {
  }


  /// Destructor.
  ~Unknowns()
  {
    delete _mutex;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Iterators.  Guard before use.
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator begin()
  {
    return _container.begin();
  }
  const_iterator begin() const
  {
    return _container.begin();
  }
  iterator end()
  {
    return _container.end();
  }
  const_iterator end() const
  {
    return _container.end();
  }

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Is it empty?
  //
  /////////////////////////////////////////////////////////////////////////////

  bool empty() const
  {
    Guard guard ( this->mutex() );
    return _container.empty();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the size of the vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_type size() const
  {
    Guard guard ( this->mutex() );
    return _container.size();
  }


  /// Add an IUnknown.
  void add ( Usul::Interfaces::IUnknown* unknown )
  {
    // Don't add twice.
    this->remove ( unknown );

    // Check for needed interface.
    typename T::QueryPtr ptr ( unknown );
    if ( true == ptr.valid() )
    {
      Guard guard ( *_mutex );
      _container.push_back ( PointerType ( ptr.get() ) );
    }
  }


  /// Remove an IUnknown.
  void remove ( Usul::Interfaces::IUnknown* unknown )
  {
    // Check for correct interface.
    typename T::QueryPtr ptr ( unknown );
    if ( true == ptr.valid() )
    {
      Guard guard ( *_mutex );
      PointerType value ( ptr.get() );
      typename Container::iterator end ( std::remove ( _container.begin(), _container.end(), value ) );
      _container.erase ( end, _container.end() );
    }
  }


  /// Clear all IUknowns.
  void clear()
  {
    Guard guard ( *_mutex );
    _container.clear();
  }


  /// Call functor f for all IUnknowns.
  template < class F >
  void for_each ( const F& func )
  {
    Guard guard ( *_mutex );
    std::for_each ( _container.begin(), _container.end(), func );
  }

  /// Get the mutex.  Use with caution.
  Mutex& mutex() const { return *_mutex; }

private:

  Container _container;
  mutable Mutex *_mutex;
};


}
}

#endif // _USUL_CONTAINERS_UNKNOWNS_H_
