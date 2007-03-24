
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace DT {
  namespace Minerva {
    namespace Glue {

      public ref class Info
      {
      public:
        static array< System::String^ >^  geometryTables ( DT::Minerva::Interfaces::IDatabaseConnection^ connection );
      };

    }
  }
}
