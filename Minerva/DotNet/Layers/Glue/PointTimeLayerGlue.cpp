
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "PointTimeLayerGlue.h"

#include "Minerva/DataObjects/Point.h"
#include "Usul/Pointers/Pointers.h"
#include "Threads/OpenThreads/Mutex.h"
#include "Usul/Threads/Mutex.h"

using namespace DT::Minerva::Glue;

PointTimeLayerGlue::PointTimeLayerGlue() : _pointTimeLayer ( 0x0 )
{
  Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );
  _pointTimeLayer = new ::Minerva::Layers::PointTimeLayer;
  Usul::Pointers::reference( _pointTimeLayer );
}

PointTimeLayerGlue::PointTimeLayerGlue( PointTimeLayerGlue ^ layer )
{
  _pointTimeLayer = new ::Minerva::Layers::PointTimeLayer;
  Usul::Pointers::reference( _pointTimeLayer );

  this->_setProperties( layer );

  this->PrimitiveID = layer->PrimitiveID;
  this->PrimitiveType = layer->PrimitiveType;
  this->DateColumn = layer->DateColumn;
  this->Size = layer->Size;
  this->Format = layer->Format;
}

PointTimeLayerGlue::~PointTimeLayerGlue()
{
  Usul::Pointers::unreference( _pointTimeLayer );
  _pointTimeLayer = 0x0;
}

PointTimeLayerGlue::!PointTimeLayerGlue()
{
  Usul::Pointers::unreference( _pointTimeLayer );
  _pointTimeLayer = 0x0;
}

Minerva::Layers::Layer* PointTimeLayerGlue::layer()
{
  return _pointTimeLayer;
}

System::String^ PointTimeLayerGlue::PrimitiveType::get()
{
  unsigned int primitiveID( _pointTimeLayer->primitiveID() );

  switch( primitiveID )
  {
  case ::Minerva::DataObjects::Point::POINT:
    return gcnew System::String ( "Point" );
  case ::Minerva::DataObjects::Point::SPHERE:
    return gcnew System::String ( "Sphere" );
  case ::Minerva::DataObjects::Point::CONE:
    return gcnew System::String ( "Cone" );
  case ::Minerva::DataObjects::Point::DISK:
    return gcnew System::String ( "Disk" );
  case ::Minerva::DataObjects::Point::CUBE:
    return gcnew System::String ( "Cube" );
  case ::Minerva::DataObjects::Point::INVERTED_CONE:
    return gcnew System::String ( "Inverted Cone" );
  }

  return gcnew System::String( "" );
}

void PointTimeLayerGlue::PrimitiveType::set(System::String^ s )
{
  if( s->Equals( "Point" ) )
    _pointTimeLayer->primitiveID( ::Minerva::DataObjects::Point::POINT );
    
  else if ( s->Equals( "Sphere" ) )
    _pointTimeLayer->primitiveID( ::Minerva::DataObjects::Point::SPHERE );
  
  else if ( s->Equals( "Cone" ) )
    _pointTimeLayer->primitiveID( ::Minerva::DataObjects::Point::CONE );
  
  else if ( s->Equals ( "Disk" ) )
    _pointTimeLayer->primitiveID( ::Minerva::DataObjects::Point::DISK );
  
  else if ( s->Equals ( "Cube" ) )
    _pointTimeLayer->primitiveID( ::Minerva::DataObjects::Point::CUBE );
  
  else if ( s->Equals ( "Inverted Cone" ) )
    _pointTimeLayer->primitiveID( ::Minerva::DataObjects::Point::INVERTED_CONE );
}
