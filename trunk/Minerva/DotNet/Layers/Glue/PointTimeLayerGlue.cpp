
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "PointTimeLayerGlue.h"

#include "Minerva/Core/DataObjects/Point.h"
#include "Minerva/Core/DB/Info.h"

#include "Usul/Pointers/Pointers.h"
#include "Usul/Strings/ManagedToNative.h"

using namespace DT::Minerva::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointTimeLayerGlue::PointTimeLayerGlue() : 
_pointTimeLayer ( 0x0 )
{
  _pointTimeLayer = new ::Minerva::Core::Layers::PointTimeLayer;
  Usul::Pointers::reference( _pointTimeLayer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointTimeLayerGlue::PointTimeLayerGlue ( ::Minerva::Core::Layers::PointTimeLayer* pointTimeLayer ) :
_pointTimeLayer ( pointTimeLayer )
{
  Usul::Pointers::reference( _pointTimeLayer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PointTimeLayerGlue::~PointTimeLayerGlue()
{
  Usul::Pointers::unreference( _pointTimeLayer );
  _pointTimeLayer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalizer.
//
///////////////////////////////////////////////////////////////////////////////

PointTimeLayerGlue::!PointTimeLayerGlue()
{
  Usul::Pointers::unreference( _pointTimeLayer );
  _pointTimeLayer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Layers::Layer* PointTimeLayerGlue::layer()
{
  return _pointTimeLayer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive type.
//
///////////////////////////////////////////////////////////////////////////////

System::String^ PointTimeLayerGlue::PrimitiveType::get()
{
  unsigned int primitiveID( _pointTimeLayer->primitiveID() );

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
  case ::Minerva::Core::DataObjects::Point::CYLINDER:
    return gcnew System::String ( "Cylinder" );
  }

  return gcnew System::String( "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primitive type.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayerGlue::PrimitiveType::set(System::String^ s )
{
  if( s->Equals( "Point" ) )
    _pointTimeLayer->primitiveID( ::Minerva::Core::DataObjects::Point::POINT );
    
  else if ( s->Equals( "Sphere" ) )
    _pointTimeLayer->primitiveID( ::Minerva::Core::DataObjects::Point::SPHERE );
  
  else if ( s->Equals( "Cone" ) )
    _pointTimeLayer->primitiveID( ::Minerva::Core::DataObjects::Point::CONE );
  
  else if ( s->Equals ( "Disk" ) )
    _pointTimeLayer->primitiveID( ::Minerva::Core::DataObjects::Point::DISK );
  
  else if ( s->Equals ( "Cube" ) )
    _pointTimeLayer->primitiveID( ::Minerva::Core::DataObjects::Point::CUBE );
  
  else if ( s->Equals ( "Inverted Cone" ) )
    _pointTimeLayer->primitiveID( ::Minerva::Core::DataObjects::Point::INVERTED_CONE );

  else if ( s->Equals ( "Cylinder" ) )
    _pointTimeLayer->primitiveID( ::Minerva::Core::DataObjects::Point::CYLINDER );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Get the first date column.
//
///////////////////////////////////////////////////////////////////////////////

System::String^ PointTimeLayerGlue::FirstDateColumn::get()
{
  return gcnew System::String ( _pointTimeLayer->firstDateColumn().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the first date column.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayerGlue::FirstDateColumn::set( System::String^ s )
{
  _pointTimeLayer->firstDateColumn ( Usul::Strings::convert( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last date column.
//
///////////////////////////////////////////////////////////////////////////////

System::String^ PointTimeLayerGlue::LastDateColumn::get()
{
  return gcnew System::String ( _pointTimeLayer->lastDateColumn().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the last date column.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayerGlue::LastDateColumn::set( System::String^ s )
{
  _pointTimeLayer->lastDateColumn ( Usul::Strings::convert( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data tables for this layer type.
//
///////////////////////////////////////////////////////////////////////////////

array< System::String ^ >^ PointTimeLayerGlue::DataTables::get()
{
  typedef ::Minerva::Core::DB::Info DbInfo;
  typedef DbInfo::Strings Strings;

  DbInfo::RefPtr info ( new DbInfo ( this->layer()->connection() ) );

  Strings strings ( info->pointTimeTables() );

  array<System::String^>^ tables = gcnew array< System::String^ > ( strings.size() );

  for( unsigned int i = 0; i < strings.size(); ++i )
    tables->SetValue( gcnew System::String( strings[i].c_str() ), static_cast < int > ( i ) );

  return tables;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the first date to show.
//
///////////////////////////////////////////////////////////////////////////////

System::DateTime PointTimeLayerGlue::FirstDate::get() 
{
  OsgTools::Animate::Date date ( _pointTimeLayer->minDate() );
  System::DateTime dateTime ( date.year(), date.month(), date.day() );
  return dateTime;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the first date to show.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayerGlue::FirstDate::set ( System::DateTime value )
{
  _pointTimeLayer->minDate( value.Day, value.Month, value.Year );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the lastt date to show.
//
///////////////////////////////////////////////////////////////////////////////

System::DateTime PointTimeLayerGlue::LastDate::get ()
{
  OsgTools::Animate::Date date ( _pointTimeLayer->maxDate() );
  System::DateTime dateTime ( date.year(), date.month(), date.day() );
  return dateTime;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the last date to show.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayerGlue::LastDate::set ( System::DateTime value )
{
  _pointTimeLayer->maxDate( value.Day, value.Month, value.Year );
}
