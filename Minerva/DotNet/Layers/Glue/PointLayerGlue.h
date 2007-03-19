
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
      public ref class PointLayerGlue : public LayerGlue,
        public DT::Minerva::Interfaces::IDataTables
	    {
      public:
        PointLayerGlue();
        PointLayerGlue( ::Minerva::Core::Layers::PointLayer* pointLayer );
        virtual ~PointLayerGlue();
        !PointLayerGlue();

        typedef System::Collections::Generic::List< System::String ^ >  Strings;

        [
          System::ComponentModel::Browsable(false)
        ]
        property int PrimitiveID
        {
          int get();
          void set ( int i );
        }


        /// Get/Set the primitive size.
        [
          System::ComponentModel::Category("Primitive"),
          System::ComponentModel::Description("Size of primitive"),
          System::ComponentModel::Browsable(true)
        ]
        property float Size
        {
          float get();
          void set ( float f );
        }

        /// Get/Set the primitive type.
        [
          System::ComponentModel::Category("Primitive"),
          System::ComponentModel::Description("Type of primitive"),
          System::ComponentModel::Browsable(true),
          System::ComponentModel::TypeConverter( DT::Minerva::Layers::TypeConverters::PointPrimitiveTypes::typeid )
        ]
        PROPERTY_GET_SET(PrimitiveType, System::String^);


        /// Get/Set stack points flag.
        PROPERTY_GET_SET(StackPoints, bool );


        /// <summary>
        /// 
        /// </summary>
        [
          System::ComponentModel::Browsable(false)
        ]
        property array<System::String^> ^ DataTables
        {
          virtual array<System::String^> ^ get ();
        }

        virtual ::Minerva::Core::Layers::Layer * layer() override;

      private:
        ::Minerva::Core::Layers::PointLayer *_pointLayer;
	    };
    }
  }
}
