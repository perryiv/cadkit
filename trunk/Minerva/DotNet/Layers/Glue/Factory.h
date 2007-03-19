
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace DT{
  namespace Minerva {
    namespace Glue {

  public ref class Factory
  {
  public:
    static CadKit::Interfaces::ILayer^ create( System::IntPtr pointer );

  private:
    Factory()
    {
    }

    ~Factory()
    {
    }
  };
}
}
}
