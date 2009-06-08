
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
//  Classes and functions for serialization.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FACTORY_REGISTER_CREATOR_H_
#define _USUL_FACTORY_REGISTER_CREATOR_H_

#include "Usul/Factory/ObjectFactory.h"
#include "Usul/Factory/TypeCreator.h"
#include "Usul/Preprocess/UniqueName.h"


namespace Usul {
namespace Factory {


///////////////////////////////////////////////////////////////////////////////
//
//  Class for automatically registering types.
//
///////////////////////////////////////////////////////////////////////////////

template < class FactoryType, class CreatorType > struct RegisterCreator
{
  typedef typename CreatorType::BaseType BaseType;
  typedef FactoryType Factory;
  RegisterCreator ( const std::string &name )
  {
    Factory::instance().add ( new CreatorType ( name ) );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Macros for automatically registering types.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_FACTORY_REGISTER_CREATOR_WITH_NAME_AND_BASE_CLASS(factory_type,the_name,the_type,the_base_type)\
  namespace { Usul::Factory::RegisterCreator < factory_type, Usul::Factory::TypeCreator < the_type, the_base_type > > USUL_UNIQUE_NAME ( the_name ); }

#define USUL_FACTORY_REGISTER_CREATOR_WITH_NAME(the_name,the_type)\
  USUL_FACTORY_REGISTER_CREATOR_WITH_NAME_AND_BASE_CLASS ( Usul::Factory::ObjectFactory, the_name, the_type, Usul::Base::Referenced )

#define USUL_FACTORY_REGISTER_CREATOR(the_type)\
  USUL_FACTORY_REGISTER_CREATOR_WITH_NAME ( #the_type, the_type )


} // namespace Factory
} // namespace Usul


#endif // _USUL_FACTORY_REGISTER_CREATOR_H_
