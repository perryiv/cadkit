
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "RLayerGlue.h"

#include "Usul/Pointers/Pointers.h"

#include "Minerva/Core/DB/Info.h"

using namespace DT::Minerva::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RLayerGlue::RLayerGlue() : BaseClass(),
_layer ( 0x0 )
{
  _layer = new ::Minerva::Core::Layers::RLayer;
  Usul::Pointers::reference ( _layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RLayerGlue::RLayerGlue( ::Minerva::Core::Layers::RLayer* rLayer ) : 
_layer ( rLayer )
{
  Usul::Pointers::reference ( _layer );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RLayerGlue::~RLayerGlue()
{
  Usul::Pointers::unreference( _layer );
  _layer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalizer.
//
///////////////////////////////////////////////////////////////////////////////

RLayerGlue::!RLayerGlue()
{
  Usul::Pointers::unreference( _layer );
  _layer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer.
//
///////////////////////////////////////////////////////////////////////////////

::Minerva::Core::Layers::Layer* RLayerGlue::layer()
{
  return _layer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the draw mode.
//
///////////////////////////////////////////////////////////////////////////////

DT::Minerva::Glue::RLayerGlue::Mode RLayerGlue::DrawMode::get()
{
  return static_cast< DT::Minerva::Glue::RLayerGlue::Mode > ( _layer->mode() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the draw mode.
//
///////////////////////////////////////////////////////////////////////////////

void RLayerGlue::DrawMode::set( DT::Minerva::Glue::RLayerGlue::Mode mode )
{
  _layer->mode( static_cast < ::Minerva::Core::Layers::RLayer::Mode > ( mode ) );
}


array<System::String^>^ RLayerGlue::DataTables::get ()
{
 typedef ::Minerva::Core::DB::Info DbInfo;
  typedef DbInfo::Strings Strings;

  DbInfo::RefPtr info ( new DbInfo ( this->layer()->connection() ) );

  Strings strings ( info->tables() );

  array<System::String^>^ tables = gcnew array< System::String^ > ( strings.size() );

  for( unsigned int i = 0; i < strings.size(); ++i )
    tables->SetValue( gcnew System::String( strings[i].c_str() ), static_cast < int > ( i ) );

  return tables;
}