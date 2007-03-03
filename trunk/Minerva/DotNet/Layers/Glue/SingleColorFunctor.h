
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BaseColorFunctor.h"

namespace DT
{
  namespace Minerva
  {
    namespace Glue
    {
      public ref class SingleColorFunctor : public BaseColorFunctor
      {
      public:
        SingleColorFunctor();

        property System::Drawing::Color^ Color
        {
          System::Drawing::Color^ get()
          {
            return _color;
          }
          void set( System::Drawing::Color^ color )
          {
            _color = color;
          }
        }

        virtual ::Minerva::Core::Functors::BaseColorFunctor * colorFunctor() override;

      private:
        System::Drawing::Color^ _color;
      };
    }
  }
}
