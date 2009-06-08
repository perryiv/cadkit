
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OSG/Glue/Node.h"

namespace CadKit 
{
  namespace OSG 
  { 
    namespace Glue 
    {
	    public ref class Tools
	    {
      public:
        static Node^ organizeScene ( Node^ existing, Node^ given );
	    };
    }
  }
}
