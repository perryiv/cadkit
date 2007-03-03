
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning ( disable : 4561 )
#include "LayerGlue.h"

#include "Minerva/Core/Layers/PolygonLayer.h"

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

        virtual ::Minerva::Core::Layers::Layer * layer() override;

      private:
        ::Minerva::Core::Layers::PolygonLayer* _polygonLayer;
      };
    }
  }
}
