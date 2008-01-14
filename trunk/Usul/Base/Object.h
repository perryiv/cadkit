
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convenient base class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_BASE_OBJECT_CLASS_H_
#define _USUL_BASE_OBJECT_CLASS_H_

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include <string>


namespace Usul {
namespace Base {


class USUL_EXPORT Object : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef Usul::Base::Referenced UserData;
  typedef Usul::Pointers::Configs::RefCountingNullOk RefCountingNullOk;
  typedef Usul::Pointers::SmartPointer<UserData,RefCountingNullOk> UserDataPtr;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Object );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Object );

  // Get the mutex.
  Mutex &                   mutex() const;

  // Set/get the name.
  std::string               name() const;
  void                      name ( const std::string &s );

  // Get/set the user-data. Caller has to lock the mutex.
  const UserData *          userData() const;
  UserData *                userData();
  void                      userData ( UserData *data );

protected:

  // Construction.
  Object();

  // Copy construction.
  Object ( const Object & );

  // No assignment.
  Object &operator = ( const Object & );

  // Use reference counting.
  virtual ~Object();

  // Reference to the name. Use wisely.
  std::string &             _getName() { return _name; }

private:

  void                      _destroy();

  UserDataPtr _userData;
  std::string _name;
  mutable Mutex *_mutex;
};


} // namespace Base
} // namespace Usul


#endif //_USUL_BASE_OBJECT_CLASS_H_
