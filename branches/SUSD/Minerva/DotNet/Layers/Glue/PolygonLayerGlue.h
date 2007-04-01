
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
      public ref class PolygonLayerGlue : public LayerGlue,
        public DT::Minerva::Interfaces::IDataTables
      {
      public:
        PolygonLayerGlue();
        PolygonLayerGlue( ::Minerva::Core::Layers::PolygonLayer* layer );
        virtual ~PolygonLayerGlue();
        !PolygonLayerGlue();

        [
          System::ComponentModel::Category("DisplayOptions")
        ]
        PROPERTY_GET_SET(ShowInterior, bool);

        [
          System::ComponentModel::Category("DisplayOptions")
        ]
        PROPERTY_GET_SET(ShowBorder, bool);

        [
          System::ComponentModel::Category("DisplayOptions")
        ]
        PROPERTY_GET_SET(BorderWidth, float);

        [
          System::ComponentModel::Category("DisplayOptions")
        ]
        PROPERTY_GET_SET(BorderColor, System::Drawing::Color);

        [
          System::ComponentModel::Browsable(false)
        ]
        property array<System::String^>^ DataTables
        {
          virtual array<System::String^>^ get();
        }

        virtual ::Minerva::Core::Layers::Layer * layer() override;

      private:
        ::Minerva::Core::Layers::PolygonLayer* _polygonLayer;
      };
    }
  }
}
