
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

#include "Minerva/Layers/PointTimeLayer.h"

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

        property System::String^ Format
        {
          System::String^ get()
          {
            return this->toManagedString( _pointTimeLayer->format() );
          }
          void set( System::String^ s )
          {
            _pointTimeLayer->format ( this->toNativeString( s ) );
          }
        }

        property System::String^ Columns
        {
          System::String^ get()
          {
            return this->toManagedString( _pointTimeLayer->columns() );
          }
          void set( System::String^ s )
          {
            _pointTimeLayer->columns ( this->toNativeString( s ) );
          }
        }

        property System::String^ DateColumn
        {
          System::String^ get()
          {
            return this->toManagedString( _pointTimeLayer->dateColumn() );
          }
          void set ( System::String^ s )
          {
            _pointTimeLayer->dateColumn( this->toNativeString( s ) );
          }
        }

        PROPERTY_GET_SET(PrimitiveType, System::String^);

        virtual ::Minerva::Layers::Layer * layer() override;

      private:
        ::Minerva::Layers::PointTimeLayer* _pointTimeLayer;
      };
    }
  }
}
