
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once;

namespace CadKit
{
  namespace Init
  {
    namespace Glue
    {
      public ref class Plugins
      {
      public:
        Plugins();

        static void loadPlugin( System::String^ filename );

        static void unloadPlugins();
      };
    }
  }
}
