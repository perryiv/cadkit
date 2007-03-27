
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "PolygonLayerGLue.h"

#include "Usul/Pointers/Pointers.h"

#include "Minerva/Core/DB/Info.h"

using namespace DT::Minerva::Glue;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayerGlue::PolygonLayerGlue() : _polygonLayer ( 0x0 )
{
  _polygonLayer = new ::Minerva::Core::Layers::PolygonLayer;
  Usul::Pointers::reference( _polygonLayer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayerGlue::PolygonLayerGlue( ::Minerva::Core::Layers::PolygonLayer* layer ) : _polygonLayer ( layer )
{
  Usul::Pointers::reference( _polygonLayer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayerGlue::~PolygonLayerGlue()
{
  Usul::Pointers::unreference( _polygonLayer );
  _polygonLayer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalizer.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayerGlue::!PolygonLayerGlue()
{
  Usul::Pointers::unreference( _polygonLayer );
  _polygonLayer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Layers::Layer* PolygonLayerGlue::layer()
{
  return _polygonLayer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all the polygon data tables.
//
///////////////////////////////////////////////////////////////////////////////

array<System::String^>^ PolygonLayerGlue::DataTables::get()
{
  typedef ::Minerva::Core::DB::Info DbInfo;
  typedef DbInfo::Strings Strings;

  DbInfo::RefPtr info ( new DbInfo ( this->layer()->connection() ) );

  Strings strings ( info->polygonTables() );

  array<System::String^>^ tables = gcnew array< System::String^ > ( strings.size() );

  for( unsigned int i = 0; i < strings.size(); ++i )
    tables->SetValue( gcnew System::String( strings[i].c_str() ), static_cast < int > ( i ) );

  return tables;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show border flag.
//
///////////////////////////////////////////////////////////////////////////////

bool PolygonLayerGlue::ShowBorder::get()
{
  return _polygonLayer->border();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show border flag.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayerGlue::ShowBorder::set( bool value )
{
  _polygonLayer->border( value );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the color for the border.
//
///////////////////////////////////////////////////////////////////////////////

System::Drawing::Color PolygonLayerGlue::BorderColor::get()
{
  osg::Vec4 color ( _polygonLayer->borderColor() );
  int a ( static_cast < int > ( color.w() * 255 ) );
  int r ( static_cast < int > ( color.x() * 255 ) );
  int g ( static_cast < int > ( color.y() * 255 ) );
  int b ( static_cast < int > ( color.z() * 255 ) );
  return System::Drawing::Color::FromArgb( a, r, g, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color for the border.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayerGlue::BorderColor::set( System::Drawing::Color color )
{
  float r ( ( static_cast < float > ( color.R ) ) / 255.0 );
  float g ( ( static_cast < float > ( color.G ) ) / 255.0 );
  float b ( ( static_cast < float > ( color.B ) ) / 255.0 );
  float a ( ( static_cast < float > ( color.A ) ) / 255.0 );
  _polygonLayer->borderColor( osg::Vec4 ( r, g, b, a ) );
}
