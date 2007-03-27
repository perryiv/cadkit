
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LayerGlue.h"

#include "Minerva/Core/Layers/LineLayer.h"

namespace DT
{
  namespace Minerva
  {
    namespace Glue 
    {
      public ref class LineLayerGlue : public LayerGlue,
        public DT::Minerva::Interfaces::IDataTables
      {
      public:
        LineLayerGlue();
        LineLayerGlue ( ::Minerva::Core::Layers::LineLayer *lineLayer );
        virtual ~LineLayerGlue();
        !LineLayerGlue();

        [
          System::ComponentModel::Category("Display"),
          System::ComponentModel::Description("Width for the lines."),
          System::ComponentModel::Browsable(true)
        ]
        property float LineWidth
        {
          float get();
          void set ( float f );
        }

        /// Get the all the line tables.
        [
          System::ComponentModel::Browsable(false)
        ]
        property array<System::String^>^ DataTables
        {
          virtual array<System::String^>^ get();
        }

        virtual ::Minerva::Core::Layers::Layer * layer() override;
      private:
        ::Minerva::Core::Layers::LineLayer *_lineLayer;
      };
    }
  }
}
