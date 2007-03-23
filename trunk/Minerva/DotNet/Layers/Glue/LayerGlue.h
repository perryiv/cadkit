
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Minerva/Core/Layers/Layer.h"

#include "Connection.h"
#include "BaseColorFunctor.h"

using namespace System;

#define PROPERTY_GET_SET(name, type) property type name { type get(); void set ( type value ); }
#define PROPERTY_SET(name, type) property type name { void set ( type value ); }
#define PROPERTY_GET(name, type) property type name { type get(); }

namespace DT
{
  namespace Minerva
  {
    namespace Glue 
    {
      public ref class LayerGlue abstract : 
        public CadKit::Referenced::Base,
        public DT::Minerva::Interfaces::ILayer,
        public DT::Minerva::Interfaces::IColumnNames,
        public DT::Minerva::Interfaces::IColorMode,
        public DT::Minerva::Interfaces::IColorProperties,
        public DT::Minerva::Interfaces::IDataSource,
        public CadKit::Interfaces::ILayer,
        public CadKit::Interfaces::IPropertyGridObject
	    {
      public:
        LayerGlue();
        virtual ~LayerGlue();
        !LayerGlue();

        /// Get/Set the layer id.
        PROPERTY_GET(LayerID, int);

        /// Set the data source
        [
          System::ComponentModel::Browsable(false)
        ]
        property System::Object^ DataSource
        {
          virtual System::Object^ get();
          virtual void set ( System::Object^ value );
        }

        [
          System::ComponentModel::Browsable(false)
        ]
        PROPERTY_GET(Connection, DT::Minerva::Glue::Connection^);


        /// Get/Set the tablename.
        [
          System::ComponentModel::Category("Database"),
          System::ComponentModel::TypeConverter( DT::Minerva::Layers::TypeConverters::DataTables::typeid )
        ]
        PROPERTY_GET_SET(Tablename, System::String^);


        /// Get/Set the column name.
        [
          System::ComponentModel::Category("Label"),
          System::ComponentModel::Description("Column to use as the label"),
          System::ComponentModel::TypeConverter(DT::Minerva::Layers::TypeConverters::ColumnNames::typeid)
        ]
        PROPERTY_GET_SET(LabelColumn, System::String^);

        /// Get/Set the render bin.
        [
          System::ComponentModel::Category("Display")
        ]
        PROPERTY_GET_SET(RenderBin, int);

        /// Get/Set the offset.
        [
          System::ComponentModel::Category("Display")
        ]
        PROPERTY_GET_SET(Offset, float);

        
        /// Get/Set the query.
        [
          System::ComponentModel::Category("Database")
        ]
        PROPERTY_GET_SET(Query, System::String^);


        /// Get/Set the column to color by.
        [
          System::ComponentModel::Category("Color"),
          System::ComponentModel::Description("Column to use as the column for gradient color"),
          System::ComponentModel::TypeConverter(DT::Minerva::Layers::TypeConverters::ColumnNames::typeid)
        ]
        PROPERTY_GET_SET(ColorColumn, System::String^);

        /// Set the color functor.
        [
          System::ComponentModel::Browsable(false)
        ]
        PROPERTY_GET_SET(ColorFunctor, DT::Minerva::Glue::BaseColorFunctor^);

        /// Get/Set the legend text.
        [
          System::ComponentModel::Category("Display")
        ]
        PROPERTY_GET_SET(LegendText, System::String^);

        /// Get/Set show label.
        PROPERTY_GET_SET( ShowLabel, bool );

        /// Get/Set show layer.
        [
          System::ComponentModel::Category("Label"),
          System::ComponentModel::Description("Should a label be shown?"),
          System::ComponentModel::Browsable(true)
        ]
        PROPERTY_GET_SET( ShowLayer, bool );

        /// Get/Set label color.
        [
          System::ComponentModel::Category("Label")
        ]
        PROPERTY_GET_SET( LabelColor, System::Drawing::Color );

        /// Get/Set label z offset
        [
          System::ComponentModel::Category("Label")
        ]
        PROPERTY_GET_SET( LabelZOffset, float );

        /// Get/Set label size
        [
          System::ComponentModel::Category("Label")
        ]
        PROPERTY_GET_SET( LabelSize, float );

        /// Get the number of data objects in this layer.
        [
          System::ComponentModel::Browsable(true)
        ]
        PROPERTY_GET (Number, unsigned int);
        

        /// <summary>
        /// 
        /// </summary>
        [
          System::ComponentModel::Browsable(false)
        ]
        property array<System::String^>^ ColumnNames { virtual array<System::String^>^ get(); }


        /// Get the layer.
        virtual ::Minerva::Core::Layers::Layer*  layer() = 0;
        virtual System::IntPtr                   layerPtr();

        void clear();

        [
          System::ComponentModel::Category("Database")
        ]
        property bool CustomQuery
        {
          bool get() { return this->layer()->customQuery(); }
          void set( bool value ) { this->layer()->customQuery( value ); }
        }


        /// Get the default query.
        [
          System::ComponentModel::Category("Database")
        ]
        property System::String^ DefaultQuery
        {
          System::String^ get()
          {
            return gcnew System::String ( this->layer()->defaultQuery().c_str() );
          }
        }


        /// Get/Set the primary key column.
        [
          System::ComponentModel::Category("Database")
        ]
        PROPERTY_GET_SET(PrimaryKeyColumn, System::String^);


        /// Set the layer properties.
        void setLayerProperties()
        {
          if (!this->CustomQuery)
            this->Query = this->DefaultQuery;

          this->ColorFunctor = this->_createColorFunctor();
        }


        /// <summary>
        /// Update the layer.
        /// </summary>
        virtual void modify()
        {
          this->setLayerProperties();
        }


        /// <summary>
        /// Hide the layer.
        /// </summary>
        virtual void hide()
        {
          this->ShowLayer = false;
        }


        /// <summary>
        /// Show the layer.
        /// </summary>
        virtual void show()
        {
          this->ShowLayer = true;
          this->setLayerProperties();
        }


        [
          System::ComponentModel::Category("Color"),
          System::ComponentModel::TypeConverter( DT::Minerva::Layers::Colors::ColorTypeConverter::typeid),
          System::ComponentModel::Editor( DT::Minerva::Layers::Colors::ColorUITypeEditor::typeid, System::Drawing::Design::UITypeEditor::typeid)
        ]
        property DT::Minerva::Layers::Colors::ColorProperties^ ColorProperties
        {
          DT::Minerva::Layers::Colors::ColorProperties^ get() { return _properties; }
          void set ( DT::Minerva::Layers::Colors::ColorProperties^ value ) { _properties = value; }
        }


        /// Get/Set the name.
        property System::String^ Name
        {
          virtual System::String^ get();
          virtual void set ( System::String^ value );
        }


        /// <summary>
        /// Get/Set the shown state.
        /// </summary>
        property bool Shown
        {
          virtual bool get() { return this->ShowLayer; }
          virtual void set ( bool value ) { this->ShowLayer = value; }
        }


        /// <summary>
        /// Get the property grid object.
        /// </summary>
        [
          System::ComponentModel::Browsable(false)
        ]
        property System::Object^ PropertyGridObject
        {
          virtual System::Object^ get() { return this; }
        }

        /// Get the color properties.
        [
          System::ComponentModel::Browsable(false)
        ]
        property System::Object^ ColorPropertiesAsObject
        {
          virtual System::Object^ get() { return _properties; }
        }

  
        property System::String^ ColorMode
        { 
          virtual System::String^ get() { return _properties->ColorMode.ToString(); }
        }

      protected:
        DT::Minerva::Glue::BaseColorFunctor^ _createColorFunctor();

        //DT::Minerva::Layers::Colors::ColorProperties^ _getColorProperties();

      private:
        DT::Minerva::Glue::Connection ^_connection;
        DT::Minerva::Glue::BaseColorFunctor          ^_colorFunctor;
        DT::Minerva::Layers::Colors::ColorProperties^ _properties;
	    };
    }
  }
}
