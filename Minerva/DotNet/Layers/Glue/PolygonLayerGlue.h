
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

#include "Minerva/Layers/PolygonLayer.h"

namespace DT
{
  namespace Minerva
  {
    namespace Glue 
    {
      public ref class PolygonLayerGlue : public LayerGlue
      {
      public:
        PolygonLayerGlue();
        PolygonLayerGlue( PolygonLayerGlue ^ layer );
        virtual ~PolygonLayerGlue();
        !PolygonLayerGlue();


        property System::String^ Format
        {
          System::String^ get();
          void set( System::String^ s );
        }

        virtual ::Minerva::Layers::Layer * layer() override;

      private:
        ::Minerva::Layers::PolygonLayer* _polygonLayer;
      };
    }
  }
}
