
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convenient base class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_BASE_BASE_OBJECT_CLASS_H_
#define _USUL_BASE_BASE_OBJECT_CLASS_H_

#ifdef _MSC_VER
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
#endif

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/ReadWriteMutex.h"
#include "Usul/Threads/ReadWriteLocks.h"

#include <string>


namespace Usul {
namespace Base {


class USUL_EXPORT BaseObject : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Threads::ReadWriteMutex Mutex;
  typedef Usul::Threads::ReadLock<Mutex> ReadLock;
  typedef Usul::Threads::WriteLock<Mutex> WriteLock;

  // Type information.
  USUL_DECLARE_TYPE_ID ( BaseObject );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BaseObject );

  // Get the mutex.
  Mutex &                   mutex() const;

protected:

  // Construction.
  BaseObject();

  // Copy construction.
  BaseObject ( const BaseObject & );

  // Assignment.
  BaseObject &operator = ( const BaseObject & );

  // Use reference counting.
  virtual ~BaseObject();

private:

  void                      _destroy();

  mutable Mutex _mutex;
};


} // namespace Base
} // namespace Usul


#endif //_USUL_BASE_BASE_OBJECT_CLASS_H_
