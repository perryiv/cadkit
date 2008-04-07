
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "LineLayerGlue.h"

#include "Usul/Pointers/Pointers.h"

#include "Minerva/Core/DB/Info.h"

using namespace DT::Minerva::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineLayerGlue::LineLayerGlue() : _lineLayer ( 0x0 )
{
  _lineLayer = new ::Minerva::Core::Layers::LineLayer;
  Usul::Pointers::reference ( _lineLayer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineLayerGlue::LineLayerGlue( ::Minerva::Core::Layers::LineLayer *lineLayer ) : _lineLayer ( lineLayer )
{
  Usul::Pointers::reference ( _lineLayer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LineLayerGlue::~LineLayerGlue()
{
  Usul::Pointers::unreference( _lineLayer );
  _lineLayer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalizer.
//
///////////////////////////////////////////////////////////////////////////////

LineLayerGlue::!LineLayerGlue()
{
  Usul::Pointers::unreference( _lineLayer );
  _lineLayer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the layer.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Layers::Layer* LineLayerGlue::layer()
{
  return _lineLayer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line width.
//
///////////////////////////////////////////////////////////////////////////////

float LineLayerGlue::LineWidth::get()
{
  return _lineLayer->lineWidth();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the line width.
//
///////////////////////////////////////////////////////////////////////////////

void LineLayerGlue::LineWidth::set ( float f )
{
  _lineLayer->lineWidth ( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all the tables with line data.
//
///////////////////////////////////////////////////////////////////////////////

array<System::String^>^ LineLayerGlue::DataTables::get()
{
  typedef ::Minerva::Core::DB::Info DbInfo;
  typedef DbInfo::Strings Strings;

  DbInfo::RefPtr info ( new DbInfo ( this->layer()->connection() ) );

  Strings strings ( info->lineTables() );

  array<System::String^>^ tables = gcnew array< System::String^ > ( strings.size() );

  for( unsigned int i = 0; i < strings.size(); ++i )
    tables->SetValue( gcnew System::String( strings[i].c_str() ), static_cast < int > ( i ) );

  return tables;
}