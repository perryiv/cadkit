
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning ( disable : 4561 )
#include "LayerGlue.h"

#include "Minerva/Layers/PointLayer.h"

using namespace System;

namespace DT
{
  namespace Minerva
  {
    namespace Glue 
    {
      public ref class PointLayerGlue : public LayerGlue
	    {
      public:
        PointLayerGlue();
        PointLayerGlue( PointLayerGlue ^ layer );
        virtual ~PointLayerGlue();
        !PointLayerGlue();

        typedef System::Collections::Generic::List< System::String ^ >  Strings;
        static Strings^ getPointPrimitiveTypes();

        property int PrimitiveID
        {
          int get();
          void set ( int i );
        }

        property float Size
        {
          float get();
          void set ( float f );
        }

        PROPERTY_GET_SET(PrimitiveType, System::String^);

        virtual ::Minerva::Layers::Layer * layer() override;

      private:
        ::Minerva::Layers::PointLayer *_pointLayer;
	    };
    }
  }
}
