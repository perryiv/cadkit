
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


Minerva::Core::Layers::Layer* PolygonLayerGlue::layer()
{
  return _polygonLayer;
}

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
