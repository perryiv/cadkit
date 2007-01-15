
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
      public ref class GradientColorFunctor : public BaseColorFunctor
      {
      public:
        GradientColorFunctor();

        property System::Drawing::Color^ MinColor
        {
          System::Drawing::Color^ get()
          {
            return _minColor;
          }
          void set( System::Drawing::Color^ color )
          {
            _minColor = color;
          }
        }


        property System::Drawing::Color^ MaxColor
        {
          System::Drawing::Color^ get()
          {
            return _maxColor;
          }
          void set( System::Drawing::Color^ color )
          {
            _maxColor = color;
          }
        }

        property double MinValue
        {
          double get()
          {
            return _minValue;
          }
          void set ( double value )
          {
            _minValue = value;
          }
        }

        property double MaxValue
        {
          double get()
          {
            return _maxValue;
          }
          void set ( double value )
          {
            _maxValue = value;
          }
        }

        virtual ::Minerva::Layers::Functors::BaseColorFunctor * colorFunctor() override;

      private:
        System::Drawing::Color^ _minColor;
        System::Drawing::Color^ _maxColor;
        double _minValue;
        double _maxValue;
      };
    }
  }
}
