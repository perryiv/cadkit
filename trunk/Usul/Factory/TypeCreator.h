
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

#ifndef _USUL_FACTORY_TYPE_CREATOR_CLASS_
#define _USUL_FACTORY_TYPE_CREATOR_CLASS_

#include "Usul/Factory/BaseCreator.h"

#include <string>


namespace Usul {
namespace Factory {


///////////////////////////////////////////////////////////////////////////////
//
//  Concrete creator class.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class BaseClassType > class TypeCreator : public BaseCreator < BaseClassType >
{
public:

  USUL_DECLARE_REF_POINTERS ( TypeCreator );
  typedef BaseClassType BaseType;
  typedef BaseCreator<BaseType> BaseClass;
  typedef T ObjectType;
  typedef TypeCreator<ObjectType,BaseType> ThisType;

  TypeCreator ( const std::string &name ) : BaseClass ( name )
  {
  }

  virtual BaseType *operator()()
  {
    return new ObjectType;
  }

protected:

  virtual ~TypeCreator()
  {
  }
};


} // namespace Factory
} // namespace Usul


#endif // _USUL_FACTORY_TYPE_CREATOR_CLASS_
