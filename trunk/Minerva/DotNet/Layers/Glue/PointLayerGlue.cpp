
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "PointLayerGlue.h"

#include "Minerva/Core/DataObjects/Point.h"
#include "Minerva/Core/DB/Info.h"

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
  _pointLayer = new ::Minerva::Core::Layers::PointLayer;
  Usul::Pointers::reference ( _pointLayer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointLayerGlue::PointLayerGlue( ::Minerva::Core::Layers::PointLayer* pointLayer ) :
_pointLayer ( pointLayer )
{
  Usul::Pointers::reference ( _pointLayer );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primitive type.
//
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
//
//  Get the stack points flag.
//
///////////////////////////////////////////////////////////////////////////////

bool PointLayerGlue::StackPoints::get()
{
  return _pointLayer->stackPoints();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the stack points flag.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayerGlue::StackPoints::set ( bool b )
{
  _pointLayer->stackPoints ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive quality.
//
///////////////////////////////////////////////////////////////////////////////

float PointLayerGlue::Quality::get()
{
  return _pointLayer->quality() * 100;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primitive quality.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayerGlue::Quality::set ( float value )
{
  _pointLayer->quality ( value / 100.0f );
}


array<System::String^> ^ PointLayerGlue::DataTables::get()
{
  typedef ::Minerva::Core::DB::Info DbInfo;
  typedef DbInfo::Strings Strings;

  DbInfo::RefPtr info ( new DbInfo ( this->layer()->connection() ) );

  Strings strings ( info->pointTables() );

  array<System::String^>^ tables = gcnew array< System::String^ > ( strings.size() );

  for( unsigned int i = 0; i < strings.size(); ++i )
    tables->SetValue( gcnew System::String( strings[i].c_str() ), static_cast < int > ( i ) );

  return tables;
}
