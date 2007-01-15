
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "PolygonTimeLayerGlue.h"

#include "Usul/Pointers/Pointers.h"
#include "Threads/OpenThreads/Mutex.h"
#include "Usul/Threads/Mutex.h"

using namespace DT::Minerva::Glue;



PolygonTimeLayerGlue::PolygonTimeLayerGlue() :
_polygonTimeLayer ( 0x0 )
{
  Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );
  _polygonTimeLayer = new ::Minerva::Layers::PolygonTimeLayer;
  Usul::Pointers::reference ( _polygonTimeLayer );
}



PolygonTimeLayerGlue::PolygonTimeLayerGlue(DT::Minerva::Glue::PolygonTimeLayerGlue ^layer) : BaseClass( layer )
{
  _polygonTimeLayer = new ::Minerva::Layers::PolygonTimeLayer;
  Usul::Pointers::reference ( _polygonTimeLayer );

  this->_setProperties( layer );

  this->StepMode = layer->StepMode;
  this->StepColumn = layer->StepColumn;
  this->FieldName = layer->FieldName;
  this->ZScaleFactor = layer->ZScaleFactor;
  this->XOffset = layer->XOffset;
  this->YOffset = layer->YOffset;
}


PolygonTimeLayerGlue::~PolygonTimeLayerGlue()
{
  if( 0x0 != _polygonTimeLayer )
  {
    _polygonTimeLayer->unref();
    _polygonTimeLayer = 0x0;
  }
}


PolygonTimeLayerGlue::!PolygonTimeLayerGlue()
{
  if( 0x0 != _polygonTimeLayer )
  {
    _polygonTimeLayer->unref();
    _polygonTimeLayer = 0x0;
  }
}


Minerva::Layers::Layer* PolygonTimeLayerGlue::layer()
{
  return _polygonTimeLayer;
}


float PolygonTimeLayerGlue::ZScaleFactor::get()
{
  return _polygonTimeLayer->zScaleFactor();
}

void PolygonTimeLayerGlue::ZScaleFactor::set( float f )
{
  _polygonTimeLayer->zScaleFactor( f );
}


float PolygonTimeLayerGlue::XOffset::get()
{
  return _polygonTimeLayer->xOffset();
}

void PolygonTimeLayerGlue::XOffset::set( float f )
{
  _polygonTimeLayer->xOffset( f );
}


float PolygonTimeLayerGlue::YOffset::get()
{
  return _polygonTimeLayer->yOffset();
}

void PolygonTimeLayerGlue::YOffset::set( float f )
{
  _polygonTimeLayer->yOffset( f );
}