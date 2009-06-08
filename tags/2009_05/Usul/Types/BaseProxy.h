
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all type proxies.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_TYPES_BASE_PROXY_CLASS_
#define _USUL_TYPES_BASE_PROXY_CLASS_

#include "Usul/Export/Export.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include <string>


namespace Usul {
namespace Types {


///////////////////////////////////////////////////////////////////////////////
//
//  Base proxy class.
//
///////////////////////////////////////////////////////////////////////////////

class USUL_EXPORT BaseProxy : public Usul::Base::Referenced
{
public:

  typedef Usul::Base::Referenced BaseClass;
  typedef unsigned long TypeId;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard < Mutex > Guard;

  USUL_DECLARE_REF_POINTERS ( BaseProxy );

  std::string             name() const;

  TypeId                  type() const;

protected:

  BaseProxy ( const std::string &name, Mutex &mutex, TypeId type );

  virtual ~BaseProxy();

  // Get the mutex. No leading '_' to work with Guard's template constructor.
  Mutex &                 mutex();

private:

  std::string _name;
  Mutex &_mutex;
  TypeId _type;
};


} // namespace Types
} // namespace Usul


#endif // _USUL_TYPES_BASE_PROXY_CLASS_
