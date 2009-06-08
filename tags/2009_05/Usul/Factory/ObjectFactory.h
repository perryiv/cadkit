
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
//  Factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FACTORY_OBJECT_FACTORY_CLASS_
#define _USUL_FACTORY_OBJECT_FACTORY_CLASS_

#include "Usul/Export/Export.h"
#include "Usul/Factory/BaseFactory.h"
#include "Usul/Preprocess/Singleton.h"


namespace Usul
{
  namespace Factory
  {
    class USUL_EXPORT USUL_DEFINE_SINGLETON ( ObjectFactory, BaseFactory < Usul::Base::Referenced > );
  }
}


#endif // _USUL_FACTORY_OBJECT_FACTORY_CLASS_
