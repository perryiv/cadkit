
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

#ifndef _SERIALIZE_XML_REGISTER_CREATOR_H_
#define _SERIALIZE_XML_REGISTER_CREATOR_H_

#include "Serialize/XML/Factory.h"
#include "Serialize/XML/TypeCreator.h"


namespace Serialize {
namespace XML {


///////////////////////////////////////////////////////////////////////////////
//
//  Class for automatically registering types.
//
///////////////////////////////////////////////////////////////////////////////

template < class CreatorType > struct RegisterCreator
{
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

#define SERIALIZE_XML_REGISTER_CREATOR_WITH_NAME(the_name,the_type)\
  namespace { Serialize::XML::RegisterCreator < Serialize::XML::TypeCreator < the_type > > _creator_for_##the_type ( the_name ); }

#define SERIALIZE_XML_REGISTER_CREATOR(the_type)\
  SERIALIZE_XML_REGISTER_CREATOR_WITH_NAME ( #the_type, the_type )


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_REGISTER_CREATOR_H_
