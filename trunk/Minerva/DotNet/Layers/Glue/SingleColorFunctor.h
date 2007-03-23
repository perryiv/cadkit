
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

#include "Minerva/Core/Functors/SingleColorFunctor.h"

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
        SingleColorFunctor ( ::Minerva::Core::Functors::SingleColorFunctor* );
        ~SingleColorFunctor();
        !SingleColorFunctor();

        property System::Drawing::Color Color
        {
          System::Drawing::Color get();
          void set( System::Drawing::Color color );
        }

        virtual ::Minerva::Core::Functors::BaseColorFunctor * colorFunctor() override;

        virtual DT::Minerva::Layers::Colors::ColorProperties^ createColorProperties() override;

      private:
        ::Minerva::Core::Functors::SingleColorFunctor* _functor;
      };
    }
  }
}
