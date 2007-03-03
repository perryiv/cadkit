
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

#include "Minerva/Core/Layers/PointTimeLayer.h"

namespace DT
{
  namespace Minerva
  {
    namespace Glue 
    {
      public ref class PointTimeLayerGlue : public LayerGlue
      {
      public:
        PointTimeLayerGlue();
        PointTimeLayerGlue( PointTimeLayerGlue ^ layer );
        virtual ~PointTimeLayerGlue();
        !PointTimeLayerGlue();

        property int PrimitiveID
        {
          int get()
          {
            return _pointTimeLayer->primitiveID();
          }
          void set ( int i )
          {
            _pointTimeLayer->primitiveID( i );
          }
        }


        property float Size
        {
          float get()
          {
            return _pointTimeLayer->size();
          }
          void set ( float f )
          {
            _pointTimeLayer->size ( f );
          }
        }


        PROPERTY_GET_SET(FirstDateColumn, System::String^);
        PROPERTY_GET_SET(LastDateColumn, System::String^);
        PROPERTY_GET_SET(PrimitiveType, System::String^);

        virtual ::Minerva::Core::Layers::Layer * layer() override;

      private:
        ::Minerva::Core::Layers::PointTimeLayer* _pointTimeLayer;
      };
    }
  }
}
