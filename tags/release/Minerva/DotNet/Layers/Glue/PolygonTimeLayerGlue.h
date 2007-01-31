
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef _MSC_VER
#pragma warning ( disable : 4561 )
#pragma warning ( disable : 4793 )
#endif

#include "LayerGlue.h"

#include "Minerva/Layers/PolygonTimeLayer.h"

namespace DT
{
  namespace Minerva
  {
    namespace Glue
    {
      public ref class PolygonTimeLayerGlue : public LayerGlue
      {
      public:
        typedef LayerGlue BaseClass;

        PolygonTimeLayerGlue();
        PolygonTimeLayerGlue( PolygonTimeLayerGlue ^ layer );
        ~PolygonTimeLayerGlue();
        !PolygonTimeLayerGlue();

        enum class Mode
        {
          DATE,
          NUMBER
        };

        property Mode StepMode
        {
          Mode get()
          {
            return (Mode) _polygonTimeLayer->mode();
          }
          void set ( Mode m )
          {
            _polygonTimeLayer->mode ( (::Minerva::Layers::PolygonTimeLayer::Mode ) m );
          }
        }

        property System::String^ FieldName
        {
          System::String^ get()
          {
            return this->toManagedString( _polygonTimeLayer->fieldName() );
          }
          void set( System::String^ s )
          {
            _polygonTimeLayer->fieldName( this->toNativeString( s ) );
          }
        }

        property System::String^ StepColumn
        {
          System::String^ get()
          {
            return this->toManagedString( _polygonTimeLayer->stepColumn() );
          }
          void set ( System::String^ s )
          {
            _polygonTimeLayer->stepColumn( this->toNativeString( s ) );
          }
        }

        PROPERTY_GET_SET(ZScaleFactor, float);
        PROPERTY_GET_SET(XOffset, float);
        PROPERTY_GET_SET(YOffset, float);


        virtual ::Minerva::Layers::Layer * layer() override;

      private:
        ::Minerva::Layers::PolygonTimeLayer *_polygonTimeLayer;
      };
    }
  }
}
