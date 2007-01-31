
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
#include "Minerva/Layers/PatentLayer.h"

namespace DT
{
  namespace Minerva
  {
    namespace Glue
    {
      public ref class PatentLayerGlue : public LayerGlue
      {
      public:
        typedef DT::Minerva::Glue::LayerGlue BaseClass;

        PatentLayerGlue();
        virtual ~PatentLayerGlue();
        !PatentLayerGlue();

        virtual ::Minerva::Layers::Layer* layer() override;

      private:
        ::Minerva::Layers::PatentLayer *_layer;
      };
    }
  }
}