
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

#ifndef _USUL_FACTORY_BASE_CREATOR_CLASS_
#define _USUL_FACTORY_BASE_CREATOR_CLASS_

#include "Usul/Export/Export.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Base/Object.h"

#include <string>


namespace Usul {
namespace Factory {


///////////////////////////////////////////////////////////////////////////////
//
//  Base creator class.
//
///////////////////////////////////////////////////////////////////////////////

class USUL_EXPORT BaseCreator : public Usul::Base::Object
{
public:

  typedef Usul::Base::Object BaseClass;

  USUL_DECLARE_REF_POINTERS ( BaseCreator );

  virtual Usul::Base::Referenced *  operator()() = 0;

protected:

  BaseCreator ( const std::string &name );

  virtual ~BaseCreator();
};


} // namespace Factory
} // namespace Usul


#endif // _USUL_FACTORY_BASE_CREATOR_CLASS_
