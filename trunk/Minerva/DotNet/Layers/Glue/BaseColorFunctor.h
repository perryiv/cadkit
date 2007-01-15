
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "osg/Vec4"
#include "Minerva/Layers/Functors/BaseColorFunctor.h"

namespace DT
{
  namespace Minerva
  {
    namespace Glue
    {
      public ref class BaseColorFunctor abstract
      {
      public:
        virtual ::Minerva::Layers::Functors::BaseColorFunctor * colorFunctor() = 0;

      protected:
        System::Drawing::Color ^ toManagedColor ( const osg::Vec4& color )
        {
          System::Drawing::Color ^c = gcnew System::Drawing::Color();
          int a = static_cast < int > ( color.w() * 255 );
          int r = static_cast < int > ( color.x() * 255 );
          int g = static_cast < int > ( color.y() * 255 );
          int b = static_cast < int > ( color.z() * 255 );
          c = System::Drawing::Color::FromArgb( a, r, g, b );

          return c;
        }

        osg::Vec4 toOsgColor ( System::Drawing::Color ^ color )
        {
          osg::Vec4 c ( (float) color->R / 255, (float) color->G / 255, (float) color->B / 255, (float) color->A / 255 );

          return c;
        }
      };
    }
  }
}
