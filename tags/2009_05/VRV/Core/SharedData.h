
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for sharing data across all nodes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SHARED_DATA_H__
#define __SHARED_DATA_H__

#include "VRV/Export.h"
#include "VRV/Core/VprIO.h"

#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Mutex.h"

#include "vpr/vprParam.h"
#include "vpr/IO/SerializableObject.h"

namespace vpr { class ObjectReader; class ObjectWriter; }

#if __VPR_version < 1001005
#define RETURN_TYPE vpr::ReturnStatus
#else
#define RETURN_TYPE void
#endif

namespace VRV {
namespace Core {

template < class T >
class SharedData : public vpr::SerializableObject
{
public:
  typedef vpr::SerializableObject BaseClass;
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;

  /// Constructor.
  SharedData() : BaseClass(), _mutex ( Mutex::create() )
  {
  }

  /// Destructor.
  virtual ~SharedData()
  {
    delete _mutex; _mutex = 0x0;
  }

  /// Read.
  virtual RETURN_TYPE readObject ( vpr::ObjectReader *reader )
  {
    // Do not guard here.

    T value;
    ReaderWriter<T>::read ( reader, value );
    this->data ( value );

#if __VPR_version < 1001005
    return vpr::ReturnStatus::Succeed;
#endif
  }

  /// Write.
  virtual RETURN_TYPE writeObject ( vpr::ObjectWriter *writer )
  {
    // Do not guard here.

    ReaderWriter<T>::write ( writer, this->data() );

#if __VPR_version < 1001005
    return vpr::ReturnStatus::Succeed;
#endif
  }

  /// Return the data.
  T data() const
  {
    Guard guard ( *_mutex );
    return _data;
  }
  
  /// Set the data.
  void data ( const T& t )
  {
    Guard guard ( *_mutex );
    _data = t;
  }

private:
  T _data;
  mutable Mutex *_mutex;
};

}
}

#endif // __SHARED_DATA_H__
