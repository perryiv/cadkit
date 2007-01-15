
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LayerGlue.h"

#include "Minerva/Layers/LineLayer.h"

namespace DT
{
  namespace Minerva
  {
    namespace Glue 
    {
      public ref class LineLayerGlue : public LayerGlue
      {
      public:
        LineLayerGlue();
        LineLayerGlue ( LineLayerGlue ^layer );
        virtual ~LineLayerGlue();
        !LineLayerGlue();

        property float LineWidth
        {
          float get();
          void set ( float f );
        }

        virtual ::Minerva::Layers::Layer * layer() override;
      private:
        ::Minerva::Layers::LineLayer *_lineLayer;
      };
    }
  }
}
