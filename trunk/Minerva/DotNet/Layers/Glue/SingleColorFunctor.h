
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
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

        virtual ::Minerva::Layers::Functors::BaseColorFunctor * colorFunctor() override;

      private:
        System::Drawing::Color^ _color;
      };
    }
  }
}
