
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
#include "Minerva/Layers/RLayer.h"

namespace DT
{
  namespace Minerva
  {
    namespace Glue
    {
      public ref class RLayerGlue : public LayerGlue
      {
      public:
        typedef DT::Minerva::Glue::LayerGlue BaseClass;

        enum class Mode
        {
          CYLINDER,
          POLYGONS
        };

        RLayerGlue();
        RLayerGlue( RLayerGlue ^ layer );
        virtual ~RLayerGlue();
        !RLayerGlue();

        PROPERTY_GET_SET(DrawMode, Mode);

        virtual ::Minerva::Layers::Layer* layer() override;

      private:
        ::Minerva::Layers::RLayer *_layer;
      };
    }
  }
}