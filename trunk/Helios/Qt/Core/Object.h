
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all commands.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_CORE_OBJECT_CLASS_H_
#define _CADKIT_HELIOS_QT_CORE_OBJECT_CLASS_H_

#include "Helios/Qt/Core/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include <string>


namespace CadKit {
namespace Helios {
namespace Core {


class HELIOS_QT_CORE_EXPORT Object : public Usul::Base::Referenced
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

private:

  void                      _destroy();

  UserDataPtr _userData;
  std::string _name;
  mutable Mutex *_mutex;
};


} // namespace Core
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_CORE_OBJECT_CLASS_H_
