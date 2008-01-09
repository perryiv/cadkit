
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace Minerva { namespace Core { namespace Functors { struct BaseColorFunctor; } } }

namespace DT{
  namespace Minerva {
    namespace Glue {

      ref class BaseColorFunctor;

  public ref class Factory
  {
  public:
    static CadKit::Interfaces::ILayer^ createIUnknown( System::IntPtr pointer );
    static CadKit::Interfaces::ILayer^ createFromMinervaLayer( System::IntPtr pointer );
    static CadKit::Interfaces::ILayer^ create( System::String^ datatable, DT::Minerva::Interfaces::IDatabaseConnection^ connection );

    static BaseColorFunctor ^          createColorFunctor ( ::Minerva::Core::Functors::BaseColorFunctor * );

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
