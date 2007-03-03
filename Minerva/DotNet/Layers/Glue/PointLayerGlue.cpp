
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "PointLayerGlue.h"

#include "Minerva/Core/DataObjects/Point.h"

#include "Threads/OpenThreads/Mutex.h"
#include "Usul/Threads/Mutex.h"

using namespace DT::Minerva::Glue;

namespace Detail
{
  System::String^ intToString ( int i )
  {
    std::ostringstream os;
    os << i;
    return gcnew System::String ( os.str().c_str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointLayerGlue::PointLayerGlue() : _pointLayer ( 0x0 )
{
  Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );
  _pointLayer = new ::Minerva::Core::Layers::PointLayer;
  Usul::Pointers::reference ( _pointLayer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointLayerGlue::PointLayerGlue( PointLayerGlue ^layer ) : _pointLayer ( 0x0 )
{
  _pointLayer = new ::Minerva::Core::Layers::PointLayer;
  Usul::Pointers::reference ( _pointLayer );

  this->_setProperties( layer );

  this->PrimitiveID = layer->PrimitiveID;
  this->Size = layer->Size;
  this->PrimitiveType = layer->PrimitiveType;
  this->StackPoints = layer->StackPoints;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PointLayerGlue::~PointLayerGlue()
{
  Usul::Pointers::unreference( _pointLayer );
  _pointLayer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalizer.
//
///////////////////////////////////////////////////////////////////////////////

PointLayerGlue::!PointLayerGlue()
{
  Usul::Pointers::unreference( _pointLayer );
  _pointLayer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive id.
//
///////////////////////////////////////////////////////////////////////////////

int PointLayerGlue::PrimitiveID::get()
{
  return _pointLayer->primitiveID();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primitive id.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayerGlue::PrimitiveID::set ( int i )
{
  _pointLayer->primitiveID( i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

float PointLayerGlue::Size::get()
{
  return _pointLayer->size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayerGlue::Size::set ( float f )
{
  _pointLayer->size ( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the available primitive types.
//
///////////////////////////////////////////////////////////////////////////////

PointLayerGlue::Strings^ PointLayerGlue::getPointPrimitiveTypes()
{
  Strings ^strings = gcnew Strings();

  strings->Add( gcnew System::String ( "Point" ) );
  strings->Add( gcnew System::String ( "Sphere" ) );
  strings->Add( gcnew System::String ( "Cone" ) );
  strings->Add( gcnew System::String ( "Disk" ) );
  strings->Add( gcnew System::String ( "Cube" ) );
  strings->Add( gcnew System::String ( "Inverted Cone" ) );

  return strings;
}


System::String^ PointLayerGlue::PrimitiveType::get()
{
  unsigned int primitiveID( _pointLayer->primitiveID() );

  switch( primitiveID )
  {
  case ::Minerva::Core::DataObjects::Point::POINT:
    return gcnew System::String ( "Point" );
  case ::Minerva::Core::DataObjects::Point::SPHERE:
    return gcnew System::String ( "Sphere" );
  case ::Minerva::Core::DataObjects::Point::CONE:
    return gcnew System::String ( "Cone" );
  case ::Minerva::Core::DataObjects::Point::DISK:
    return gcnew System::String ( "Disk" );
  case ::Minerva::Core::DataObjects::Point::CUBE:
    return gcnew System::String ( "Cube" );
  case ::Minerva::Core::DataObjects::Point::INVERTED_CONE:
    return gcnew System::String ( "Inverted Cone" );
  }

  return gcnew System::String( "" );
}

void PointLayerGlue::PrimitiveType::set(System::String^ s )
{
  if( s->Equals( "Point" ) )
    _pointLayer->primitiveID( ::Minerva::Core::DataObjects::Point::POINT );
    
  else if ( s->Equals( "Sphere" ) )
    _pointLayer->primitiveID( ::Minerva::Core::DataObjects::Point::SPHERE );
  
  else if ( s->Equals( "Cone" ) )
    _pointLayer->primitiveID( ::Minerva::Core::DataObjects::Point::CONE );
  
  else if ( s->Equals ( "Disk" ) )
    _pointLayer->primitiveID( ::Minerva::Core::DataObjects::Point::DISK );
  
  else if ( s->Equals ( "Cube" ) )
    _pointLayer->primitiveID( ::Minerva::Core::DataObjects::Point::CUBE );
  
  else if ( s->Equals ( "Inverted Cone" ) )
    _pointLayer->primitiveID( ::Minerva::Core::DataObjects::Point::INVERTED_CONE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Layers::Layer* PointLayerGlue::layer()
{
  return _pointLayer;
}


bool PointLayerGlue::StackPoints::get()
{
  return _pointLayer->stackPoints();
}

void PointLayerGlue::StackPoints::set ( bool b )
{
  _pointLayer->stackPoints ( b );
}
