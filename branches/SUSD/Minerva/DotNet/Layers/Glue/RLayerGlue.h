
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
#include "Minerva/Core/Layers/RLayer.h"

namespace DT
{
  namespace Minerva
  {
    namespace Glue
    {
      public ref class RLayerGlue : public LayerGlue,
        public DT::Minerva::Interfaces::IDataTables
      {
      public:
        typedef DT::Minerva::Glue::LayerGlue BaseClass;

        enum class Mode
        {
          CYLINDER,
          POLYGONS
        };

        RLayerGlue();
        RLayerGlue ( ::Minerva::Core::Layers::RLayer* rLayer );
        virtual ~RLayerGlue();
        !RLayerGlue();

        PROPERTY_GET_SET(DrawMode, Mode);

        /// <summary>
        /// 
        /// </summary>
        property array<System::String^>^ DataTables
        {
          virtual array<System::String^>^ get ();
        }

        virtual ::Minerva::Core::Layers::Layer* layer() override;

      private:
        ::Minerva::Core::Layers::RLayer *_layer;
      };
    }
  }
}