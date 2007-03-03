
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
#include "Threads/OpenThreads/Mutex.h"
#include "Usul/Threads/Mutex.h"

using namespace DT::Minerva::Glue;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayerGlue::PolygonLayerGlue() : _polygonLayer ( 0x0 )
{
  Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );
  _polygonLayer = new ::Minerva::Core::Layers::PolygonLayer;
  Usul::Pointers::reference( _polygonLayer );
}


PolygonLayerGlue::PolygonLayerGlue(DT::Minerva::Glue::PolygonLayerGlue ^layer)
{
  _polygonLayer = new ::Minerva::Core::Layers::PolygonLayer;
  Usul::Pointers::reference( _polygonLayer );

  this->_setProperties( layer );

  this->Format = layer->Format;
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


System::String^ PolygonLayerGlue::Format::get()
{
  return this->toManagedString( _polygonLayer->format() );
}
void PolygonLayerGlue::Format::set( System::String^ s )
{
  _polygonLayer->format ( this->toNativeString( s ) );
}
