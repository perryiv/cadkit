
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "LayerGlue.h"
#include "SingleColorFunctor.h"
#include "GradientColorFunctor.h"
#include "Factory.h"

#include "Usul/Strings/Managed.h"

#include "Minerva/Core/DB/Info.h"

using namespace DT::Minerva::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LayerGlue::LayerGlue() : 
_connection( nullptr ),
_colorFunctor ( nullptr ),
_properties ( nullptr )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LayerGlue::~LayerGlue()
{
  _connection = nullptr;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalizer.
//
///////////////////////////////////////////////////////////////////////////////

LayerGlue::!LayerGlue()
{
  _connection = nullptr;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the native pointer.
//
///////////////////////////////////////////////////////////////////////////////

System::IntPtr LayerGlue::nativeIntPtr()
{
  /// Make sure every thing is set properly before returning native pointer.
  this->setLayerProperties();

  return System::IntPtr ( this->layer()->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::clear()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data source.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::DataSource::set ( System::Object^ value )
{
  DT::Minerva::Glue::Connection^ connection = dynamic_cast < DT::Minerva::Glue::Connection^ > ( value );
  if( nullptr != connection )
  {
    _connection = connection;
    this->layer()->connection( _connection->nativeConnection() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data source.
//
///////////////////////////////////////////////////////////////////////////////

System::Object^ LayerGlue::DataSource::get()
{
  return _connection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the table name.
//
///////////////////////////////////////////////////////////////////////////////

System::String^ LayerGlue::Tablename::get()
{
  return gcnew System::String ( this->layer()->tablename().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the table name.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::Tablename::set( System::String^ s )
{
  this->layer()->tablename( Usul::Strings::convert ( s ) );
  this->ColorColumn = "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the column to use for the label.
//
///////////////////////////////////////////////////////////////////////////////

System::String^ LayerGlue::LabelColumn::get()
{
  return gcnew System::String ( this->layer()->labelColumn().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the column to use for the label.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::LabelColumn::set( System::String^ s )
{
  this->layer()->labelColumn( Usul::Strings::convert ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the render bin.
//
///////////////////////////////////////////////////////////////////////////////

int LayerGlue::RenderBin::get()
{
  return this->layer()->renderBin();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the render bin.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::RenderBin::set( int i )
{
  this->layer()->renderBin( i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the x offset.
//
///////////////////////////////////////////////////////////////////////////////

float LayerGlue::XOffset::get()
{
  return this->layer()->xOffset();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the z offset.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::XOffset::set( float f )
{
  this->layer()->xOffset( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the z offset.
//
///////////////////////////////////////////////////////////////////////////////

float LayerGlue::YOffset::get()
{
  return this->layer()->yOffset();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the z offset.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::YOffset::set( float f )
{
  this->layer()->yOffset( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the z offset.
//
///////////////////////////////////////////////////////////////////////////////

float LayerGlue::ZOffset::get()
{
  return this->layer()->zOffset();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the z offset.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::ZOffset::set( float f )
{
  this->layer()->zOffset( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the query.
//
///////////////////////////////////////////////////////////////////////////////

System::String^ LayerGlue::Query::get()
{
  return gcnew System::String ( this->layer()->query().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the query.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::Query::set( System::String^ s )
{
  this->layer()->query( Usul::Strings::convert ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the legend text.
//
///////////////////////////////////////////////////////////////////////////////

System::String^ LayerGlue::LegendText::get()
{
  return gcnew System::String ( this->layer()->legendText().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend text.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::LegendText::set( System::String^ s )
{
  this->layer()->legendText( Usul::Strings::convert ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the label be shown?
//
///////////////////////////////////////////////////////////////////////////////

bool LayerGlue::ShowLabel::get()
{
  return this->layer()->showLabel();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show label flag.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::ShowLabel::set( bool b )
{
  this->layer()->showLabel( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the layer be shown?
//
///////////////////////////////////////////////////////////////////////////////

bool LayerGlue::ShowLayer::get()
{
  return this->layer()->showLayer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show layer flag.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::ShowLayer::set( bool b )
{
  this->layer()->showLayer( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color for the label.
//
///////////////////////////////////////////////////////////////////////////////

System::Drawing::Color LayerGlue::LabelColor::get()
{
  osg::Vec4 color ( this->layer()->labelColor() );

  int a ( static_cast < int > ( color.w() * 255 ) );
  int r ( static_cast < int > ( color.x() * 255 ) );
  int g ( static_cast < int > ( color.y() * 255 ) );
  int b ( static_cast < int > ( color.z() * 255 ) );
  return System::Drawing::Color::FromArgb( a, r, g, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color for the label.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::LabelColor::set( System::Drawing::Color color )
{
  float r ( ( static_cast < float > ( color.R ) ) / 255.0 );
  float g ( ( static_cast < float > ( color.G ) ) / 255.0 );
  float b ( ( static_cast < float > ( color.B ) ) / 255.0 );
  float a ( ( static_cast < float > ( color.A ) ) / 255.0 );
  this->layer()->labelColor( osg::Vec4 ( r, g, b, a ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the label z offset.
//
///////////////////////////////////////////////////////////////////////////////

float LayerGlue::LabelZOffset::get()
{
  return this->layer()->labelZOffset();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the label z offset.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::LabelZOffset::set ( float f )
{
  this->layer()->labelZOffset ( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color column.
//
///////////////////////////////////////////////////////////////////////////////

System::String^ LayerGlue::ColorColumn::get()
{
  return gcnew System::String ( this->layer()->colorColumn().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color column.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::ColorColumn::set( System::String^ s )
{
  this->layer()->colorColumn( Usul::Strings::convert ( s ) );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of data objects.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int LayerGlue::Number::get()
{
  return this->layer()->number();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the label size.
//
///////////////////////////////////////////////////////////////////////////////

float LayerGlue::LabelSize::get()
{
  return this->layer()->labelSize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the label size.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::LabelSize::set ( float f )
{
  this->layer()->labelSize ( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all column names.
//
///////////////////////////////////////////////////////////////////////////////

array<System::String^>^ LayerGlue::ColumnNames::get()
{
  if( 0x0 != this->layer()->connection() && this->Tablename->Length > 0 )
  {
    typedef ::Minerva::Core::DB::Info DbInfo;
    typedef DbInfo::Strings Strings;

    DbInfo::RefPtr info ( new DbInfo ( this->layer()->connection() ) );

    Strings strings ( info->getColumnNames( Usul::Strings::convert ( this->Tablename ) ) );

    array<System::String^>^ columns = gcnew array< System::String^ > ( strings.size() );

    for( unsigned int i = 0; i < strings.size(); ++i )
      columns->SetValue( gcnew System::String( strings[i].c_str() ), static_cast < int > ( i ) );

    return columns;
  }
  return gcnew array<System::String^>( 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create color functor.
//
///////////////////////////////////////////////////////////////////////////////

DT::Minerva::Glue::BaseColorFunctor^ LayerGlue::_createColorFunctor( DT::Minerva::Layers::Colors::ColorProperties ^properties )
{
  if( nullptr != properties )
  {
    if (properties->ColorMode == DT::Minerva::Layers::Colors::ColorProperties::Mode::SINGLE_COLOR)
    {
      DT::Minerva::Glue::SingleColorFunctor ^functor = gcnew DT::Minerva::Glue::SingleColorFunctor();
      functor->Color = properties->Color;
      return functor;
    }
    else if (properties->ColorMode == DT::Minerva::Layers::Colors::ColorProperties::Mode::GRADIENT_COLOR)
    {
      DT::Minerva::Glue::GradientColorFunctor ^funtor = gcnew DT::Minerva::Glue::GradientColorFunctor();
      funtor->MinColor = properties->MinColor;
      funtor->MaxColor = properties->MaxColor;

      if (nullptr != _connection)
      {
        double min = 0.0, max = 0.0;
        ::Minerva::Core::DB::Info::RefPtr info ( new ::Minerva::Core::DB::Info ( this->layer()->connection() ) );
        info->getMinMaxValue( Usul::Strings::convert ( this->Query ), Usul::Strings::convert ( this->ColorColumn ), min, max);
        funtor->MinValue = min;
        funtor->MaxValue = max;
        return funtor;
      }
  }
  }

  return nullptr;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

System::String^ LayerGlue::Name::get()
{
  return gcnew System::String ( this->layer()->name().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::Name::set ( System::String^ value )
{
  this->layer()->name( Usul::Strings::convert( value ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primary key column.
//
///////////////////////////////////////////////////////////////////////////////

System::String^ LayerGlue::PrimaryKeyColumn::get()
{
  return gcnew System::String ( this->layer()->primaryKeyColumn().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primary key column.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::PrimaryKeyColumn::set( System::String^ value )
{
  this->layer()->primaryKeyColumn ( Usul::Strings::convert( value ) ); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the connection.
//
///////////////////////////////////////////////////////////////////////////////

DT::Minerva::Glue::Connection^ LayerGlue::Connection::get()
{
  return _connection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color properties.
//
///////////////////////////////////////////////////////////////////////////////

DT::Minerva::Layers::Colors::ColorProperties^ LayerGlue::ColorProperties::get()
{
  if( nullptr == _properties )
  {
    BaseColorFunctor ^functor = DT::Minerva::Glue::Factory::createColorFunctor ( this->layer()->colorFunctor() );
    if( nullptr != functor )
      _properties = functor->createColorProperties();
    else
      _properties = gcnew DT::Minerva::Layers::Colors::ColorProperties();
  }

  return _properties;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color properties.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::ColorProperties::set ( DT::Minerva::Layers::Colors::ColorProperties^ value )
{
  _properties = value;
  this->layer()->colorFunctor( this->_createColorFunctor( value )->colorFunctor() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show count in legend flag.
//
///////////////////////////////////////////////////////////////////////////////

bool LayerGlue::ShowCountLegend::get()
{
  return this->layer()->showCountLegend();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the show count in legend flag.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::ShowCountLegend::set ( bool value )
{
  this->layer()->showCountLegend( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show min in legend flag.
//
///////////////////////////////////////////////////////////////////////////////

bool LayerGlue::ShowMinLegend::get()
{
  return this->layer()->showMinLegend();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the show min in legend flag.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::ShowMinLegend::set ( bool value )
{
  this->layer()->showMinLegend( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show max in legend flag.
//
///////////////////////////////////////////////////////////////////////////////

bool LayerGlue::ShowMaxLegend::get()
{
  return this->layer()->showMaxLegend();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the show max in legend flag.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::ShowMaxLegend::set ( bool value )
{
  this->layer()->showMaxLegend( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Guid.
//
///////////////////////////////////////////////////////////////////////////////

System::String^ LayerGlue::Guid::get()
{
  return gcnew System::String ( this->layer()->guid().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the alpha.
//
///////////////////////////////////////////////////////////////////////////////

float LayerGlue::Alpha::get()
{
  return this->layer()->alpha();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the alpha.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::Alpha::set( float f )
{
  this->layer()->alpha( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get flag to show layer in legend.
//
///////////////////////////////////////////////////////////////////////////////

bool LayerGlue::ShowInLegend::get()
{
  return this->layer()->showInLegend();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set flag to show layer in legend.
//
///////////////////////////////////////////////////////////////////////////////

void LayerGlue::ShowInLegend::set ( bool value )
{
  this->layer()->showInLegend( value );
}
