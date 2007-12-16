
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
//  Scoped class registration.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FACTORY_SCOPED_REGISTRATION_CLASS_
#define _USUL_FACTORY_SCOPED_REGISTRATION_CLASS_

#include "Usul/Export/Export.h"

#include <string>

namespace Usul { namespace Factory { class ObjectFactory; class BaseCreator; } }


namespace Usul {
namespace Factory {


///////////////////////////////////////////////////////////////////////////////
//
//  Factory class.
//
///////////////////////////////////////////////////////////////////////////////

struct USUL_EXPORT ScopedRegistration 
{
  ScopedRegistration ( ObjectFactory &, BaseCreator * );
  ~ScopedRegistration();

private:

  ObjectFactory &_factory;
  std::string _name;
};


} // namespace Factory
} // namespace Usul


#endif // _USUL_FACTORY_OBJECT_FACTORY_CLASS_
