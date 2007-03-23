
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
      public ref class PointTimeLayerGlue : public LayerGlue,
        DT::Minerva::Interfaces::IDataTables
      {
      public:
        PointTimeLayerGlue();
        PointTimeLayerGlue ( ::Minerva::Core::Layers::PointTimeLayer* pointTimeLayer );
        virtual ~PointTimeLayerGlue();
        !PointTimeLayerGlue();

        [
          System::ComponentModel::Browsable(false)
        ]
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


        /// Get/Set the primitive size.
        [
          System::ComponentModel::Category("Primitive"),
          System::ComponentModel::Description("Size of primitive"),
          System::ComponentModel::Browsable(true)
        ]
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


        [
          System::ComponentModel::Category("Date"),
          System::ComponentModel::Description("Column that contains the first date."),
          System::ComponentModel::Browsable(true),
          System::ComponentModel::TypeConverter(DT::Minerva::Layers::TypeConverters::ColumnNames::typeid)
        ]
        PROPERTY_GET_SET(FirstDateColumn, System::String^);

        [
          System::ComponentModel::Category("Date"),
          System::ComponentModel::Description("Column that contains the last date."),
          System::ComponentModel::Browsable(true),
          System::ComponentModel::TypeConverter(DT::Minerva::Layers::TypeConverters::ColumnNames::typeid)
        ]
        PROPERTY_GET_SET(LastDateColumn, System::String^);


        /// Get/Set the primitive type.
        [
          System::ComponentModel::Category("Primitive"),
          System::ComponentModel::Description("Type of primitive"),
          System::ComponentModel::Browsable(true)
        ]
        PROPERTY_GET_SET(PrimitiveType, System::String^);

        property array< System::String ^ >^ DataTables
        {
          virtual array< System::String ^ >^ get();
        };

        virtual ::Minerva::Core::Layers::Layer * layer() override;


        /// Get/Set the minimum date to show.
        [
          System::ComponentModel::Category("Date"),
          System::ComponentModel::Description("First date to show"),
          System::ComponentModel::Browsable(true)
        ]
        PROPERTY_GET_SET(FirstDate, System::DateTime);

        /// Get/Set the maximium date to show.
        [
          System::ComponentModel::Category("Date"),
          System::ComponentModel::Description("Last date to show"),
          System::ComponentModel::Browsable(true)
        ]
        PROPERTY_GET_SET(LastDate, System::DateTime);

      private:
        ::Minerva::Core::Layers::PointTimeLayer* _pointTimeLayer;
      };
    }
  }
}
