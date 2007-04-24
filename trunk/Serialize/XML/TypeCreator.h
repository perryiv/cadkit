
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
//  Base creator class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SERIALIZE_XML_TYPE_CREATOR_CLASS_
#define _SERIALIZE_XML_TYPE_CREATOR_CLASS_

#include "Serialize/XML/BaseCreator.h"

#include <string>


namespace Serialize {
namespace XML {


///////////////////////////////////////////////////////////////////////////////
//
//  Concrete creator class.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > class TypeCreator : public BaseCreator 
{
public:

  USUL_DECLARE_REF_POINTERS ( TypeCreator );
  typedef BaseCreator BaseClass;
  typedef T ObjectType;

  TypeCreator ( const std::string &name ) : BaseClass ( name )
  {
  }

  virtual Usul::Base::Referenced *operator()()
  {
    return new T;
  }

protected:

  virtual ~TypeCreator()
  {
  }
};


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_TYPE_CREATOR_CLASS_
