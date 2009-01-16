
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
#include <stdexcept>

namespace Usul { namespace Factory { class ObjectFactory; class BaseCreator; } }


namespace Usul {
namespace Factory {


///////////////////////////////////////////////////////////////////////////////
//
//  Factory class.
//
///////////////////////////////////////////////////////////////////////////////

template < class FactoryType, class CreatorType > struct ScopedRegistration 
{
  ScopedRegistration ( FactoryType &f, CreatorType *c ) :
    _f ( f ),
    _name ( ( 0x0 == c ) ? "" : c->name() )
  {
    if ( 0x0 == _c )
    {
      throw std::invalid_argument ( "Error 4027331109: Null creator given" );
    }

    if ( true == _name.empty() )
    {
      throw std::invalid_argument ( "Error 3108874680: Empty name given when registering creator" );
    }

    _factory.add ( _c );
  }

  ~ScopedRegistration()
  {
    _factory.remove ( _name );
  }

private:

  FactoryType &_f;
  std::string _name;
};


} // namespace Factory
} // namespace Usul


#endif // _USUL_FACTORY_OBJECT_FACTORY_CLASS_
