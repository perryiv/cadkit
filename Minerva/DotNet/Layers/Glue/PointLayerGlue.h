
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
////////  ///////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning ( disable : 4561 )
#include "LayerGlue.h"

#include "Minerva/Core/Layers/PointLayer.h"

using namespace System;

namespace DT
{
  namespace Minerva
  {
    namespace Glue 
    {
      public ref class PointLayerGlue : public LayerGlue
	    {
      public:
        PointLayerGlue();
        PointLayerGlue( ::Minerva::Core::Layers::PointLayer* pointLayer );
        virtual ~PointLayerGlue();
        !PointLayerGlue();

        typedef System::Collections::Generic::List< System::String ^ >  Strings;


        /// Get/Set the primitive size.
        [
          System::ComponentModel::Category("Primitive"),
          System::ComponentModel::Description("Size of primitive"),
          System::ComponentModel::Browsable(true)
        ]
        PROPERTY_GET_SET(Size, float )

        /// Get/Set the primitive type.
        [
          System::ComponentModel::Category("Primitive"),
          System::ComponentModel::Description("Type of primitive"),
          System::ComponentModel::Browsable(true),
          System::ComponentModel::TypeConverter( DT::Minerva::Layers::TypeConverters::PointPrimitiveTypes::typeid )
        ]
        PROPERTY_GET_SET(PrimitiveType, System::String^);


        /// Get/Set the column to base primitive size from.
        [
          System::ComponentModel::Category("Primitive"),
          System::ComponentModel::Description("Column to base size of primitive from"),
          System::ComponentModel::Browsable(true),
          System::ComponentModel::TypeConverter( DT::Minerva::Layers::TypeConverters::ColumnNames::typeid )
        ]
        PROPERTY_GET_SET( PrimitiveSizeColumn, System::String^ )


        /// Get/Set stack points flag.
        PROPERTY_GET_SET(StackPoints, bool );

        /// Get/Set primitive quality
        [
          System::ComponentModel::Category("Primitive"),
          System::ComponentModel::Description("Quality of primitive"),
          System::ComponentModel::Browsable(true)
        ]
        PROPERTY_GET_SET ( Quality, float );


        /// Get/Set use auto transforms flag.
        [
          System::ComponentModel::Category("Primitive"),
          System::ComponentModel::Description("Use auto transforms?"),
          System::ComponentModel::Browsable(true)
        ]
        PROPERTY_GET_SET ( AutoTransform, bool );

        virtual ::Minerva::Core::Layers::Layer * layer() override;

      private:
        ::Minerva::Core::Layers::PointLayer *_pointLayer;
	    };
    }
  }
}
