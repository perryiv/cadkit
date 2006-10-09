
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"

namespace CadKit
{
  namespace OSG
  {
    namespace Glue
    {
      public ref class ReadFile
      {
      public:
        ReadFile ()  { }

        static CadKit::OSG::Glue::Node^ readNodeFile ( System::String^ filename );
      };
    }
  }
}
