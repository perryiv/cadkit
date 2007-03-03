
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
#include "Minerva/Core/Layers/PatentLayer.h"

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

        virtual ::Minerva::Core::Layers::Layer* layer() override;

      private:
        ::Minerva::Core::Layers::PatentLayer *_layer;
      };
    }
  }
}