
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  Thread safe, referenced counted wrapper around a vector.
//  Not all functionality of a vector is exposed because of thread-safe issues.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DATA_VECTOR_H__
#define __MINERVA_CORE_DATA_VECTOR_H__

#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Pointers/Pointers.h"

#include "boost/thread/shared_mutex.hpp"
#include "boost/thread/locks.hpp"

#include <vector>

namespace Minerva {
namespace Core {
namespace Data {

template<class T, class MutexType = boost::shared_mutex >
class Vector : public Usul::Base::Referenced
{
  typedef std::vector<T> VectorType;
public:
  
  typedef Usul::Base::Referenced BaseClass;
  typedef Vector<T,MutexType> ThisType;
  typedef MutexType Mutex;
  typedef boost::shared_lock<Mutex> ReadLock;
  typedef boost::unique_lock<Mutex> WriteLock;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef typename VectorType::value_type value_type;
  typedef typename VectorType::size_type size_type;
  typedef typename VectorType::difference_type difference_type;
  typedef typename VectorType::iterator iterator;
  typedef typename VectorType::const_iterator const_iterator;
  typedef typename VectorType::reverse_iterator reverse_iterator;
  typedef typename VectorType::const_reverse_iterator const_reverse_iterator;
  typedef typename VectorType::reference reference;
  typedef typename VectorType::const_reference const_reference;

  USUL_DECLARE_REF_POINTERS ( ThisType );
  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor/destructor
  //
  /////////////////////////////////////////////////////////////////////////////
  
  Vector() : _v(), _mutex()
  {
  }
  Vector ( const ThisType &v ) : _v(), _mutex()
  {
    Guard guard ( v.mutex() );
    _v = v._v;
  }
  Vector ( const VectorType &v ) : _v ( v ), _mutex()
  {
  }
  Vector ( size_type n, const value_type &t ) : _v ( n, t ), _mutex()
  {
  }
  ~Vector()
  {
  }
  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////
  
  Vector &operator = ( const Vector &v )
  {
    Guard guard0 ( v.mutex() );
    Guard guard1 ( this->mutex() );
    _v = v;
    return *this;
  }
  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Is it empty?
  //
  /////////////////////////////////////////////////////////////////////////////
  
  bool empty() const
  {
    ReadLock guard ( this->mutex() );
    return _v.empty();
  }
  
  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Push an element onto the back of the vector.
  //
  /////////////////////////////////////////////////////////////////////////////
  
  void push_back ( const value_type &value )
  {
    Guard guard ( this->mutex() );
    _v.push_back ( value );
  }
  
  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Push an element onto the front of the vector.
  //
  /////////////////////////////////////////////////////////////////////////////
  
  void push_front ( const value_type &value )
  {
    Guard guard ( this->mutex() );
    _v.push_front ( value );
  }
  
  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Pop an element off the back of the vector.
  //
  /////////////////////////////////////////////////////////////////////////////
  
  void pop_back()
  {
    Guard guard ( this->mutex() );
    _v.pop_back();
  }
  
  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Pop an element off the front of the vector.
  //
  /////////////////////////////////////////////////////////////////////////////
  
  void pop_front()
  {
    Guard guard ( this->mutex() );
    _v.pop_front();
  }
  
  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the size of the vector.
  //
  /////////////////////////////////////////////////////////////////////////////
  
  size_type size() const
  {
    ReadLock guard ( this->mutex() );
    return _v.size();
  }
  
  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the capacity of the vector.
  //
  /////////////////////////////////////////////////////////////////////////////
  
  size_type capacity() const
  {
    ReadLock guard ( this->mutex() );
    return _v.capacity();
  }
  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Iterators. Mutex must be locked before accessing these functions!
  //
  /////////////////////////////////////////////////////////////////////////////
  
  iterator begin()
  {
    return _v.begin();
  }
  const_iterator begin() const
  {
    return _v.begin();
  }
  iterator end()
  {
    return _v.end();
  }
  const_iterator end() const
  {
    return _v.end();
  }
  
  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Resize.
  //
  /////////////////////////////////////////////////////////////////////////////
  
  void resize ( size_type i )
  {
    Guard guard ( this->mutex() );
    _v.resize ( i );
  }
  
  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Reserve.
  //
  /////////////////////////////////////////////////////////////////////////////
  
  void reserve ( size_type i )
  {
    Guard guard ( this->mutex() );
    _v.reserve ( i );
  }
  
  /// Execute a functor with read only capability.
  template<class F>
  void for_each_read_only ( F f ) const
  {
    ReadLock guard ( this->mutex() );
    std::for_each ( _v.begin(), _v.end(), f );
  }
  
  /// Return the mutex.
  Mutex& mutex() const
  {
    return _mutex;
  }
  
private:
  
  VectorType _v;
  mutable Mutex _mutex;

};

}
}
}

#endif // __MINERVA_CORE_DATA_VECTOR_H__
