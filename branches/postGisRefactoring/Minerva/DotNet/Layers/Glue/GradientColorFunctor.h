
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

#include "Minerva/Core/Functors/GradientColorFunctor.h"

namespace DT
{
  namespace Minerva
  {
    namespace Glue
    {
      public ref class GradientColorFunctor : public BaseColorFunctor
      {
      public:
        GradientColorFunctor();
        GradientColorFunctor ( ::Minerva::Core::Functors::GradientColorFunctor* );
        ~GradientColorFunctor();
        !GradientColorFunctor();

        property System::Drawing::Color MinColor
        {
          System::Drawing::Color get();
          void set( System::Drawing::Color color );
        }


        property System::Drawing::Color MaxColor
        {
          System::Drawing::Color get();
          void set( System::Drawing::Color color );
        }

        property double MinValue
        {
          double get();
          void set ( double value );
        }

        property double MaxValue
        {
          double get();
          void set ( double value );
        }

        virtual ::Minerva::Core::Functors::BaseColorFunctor * colorFunctor() override;

        virtual DT::Minerva::Layers::Colors::ColorProperties^ createColorProperties() override;

      private:
        ::Minerva::Core::Functors::GradientColorFunctor * _functor;
      };
    }
  }
}
