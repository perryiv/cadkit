
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Minerva/Layers/Layer.h"

#include "BaseColorFunctor.h"

using namespace System;

#define PROPERTY_GET_SET(name, type) property type name { type get(); void set ( type t ); }
#define PROPERTY_SET(name, type) property type name { void set ( type t ); }
#define PROPERTY_GET(name, type) property type name { type get(); }

namespace DT
{
  namespace Minerva
  {
    namespace Glue 
    {
      public ref class LayerGlue abstract : public DT::Minerva::Interfaces::ILayer
	    {
      public:
        LayerGlue();
        LayerGlue( LayerGlue ^ );
        virtual ~LayerGlue();
        !LayerGlue();

        /// Get/Set the layer id.
        PROPERTY_GET_SET(LayerID, int);

        /// Set the connection
        PROPERTY_GET_SET(Connection, DT::Minerva::Interfaces::IDatabaseConnection^);

        /// Get/Set the tablename.
        PROPERTY_GET_SET(Tablename, System::String^);

        /// Get/Set the column name.
        PROPERTY_GET_SET(LabelColumn, System::String^);

        /// Get/Set the render bin.
        PROPERTY_GET_SET(RenderBin, int);

        /// Get/Set the offset.
        PROPERTY_GET_SET(Offset, float);

        /// Get/Set the query.
        PROPERTY_GET_SET(Query, System::String^);

        /// Set the color functor.
        PROPERTY_GET_SET(ColorFunctor, DT::Minerva::Glue::BaseColorFunctor^);

        /// Get/Set the legend text.
        PROPERTY_GET_SET(LegendText, System::String^);

        /// Get/Set show label.
        PROPERTY_GET_SET( ShowLabel, bool );

        /// Get/Set show layer.
        PROPERTY_GET_SET( ShowLayer, bool );

        /// Get the layer.
        virtual ::Minerva::Layers::Layer* layer() = 0;
        virtual System::IntPtr      layerPtr();

        void clear();
        
      protected:
        void _setProperties( LayerGlue ^ layer );

        System::String^ toManagedString( const std::string& source )
        {
          return gcnew System::String( source.c_str() );
        }

        std::string toNativeString( System::String^ source )
        {
          if( source == nullptr )
            return std::string();

          System::IntPtr ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( source );
          char* s = (char*)(void*) ptr;
          
          std::string string ( s );
          
          System::Runtime::InteropServices::Marshal::FreeHGlobal( ptr );

          return string;
        }

      private:
        DT::Minerva::Interfaces::IDatabaseConnection^ _connection;
        DT::Minerva::Glue::BaseColorFunctor ^_colorFunctor;
	    };
    }
  }
}
