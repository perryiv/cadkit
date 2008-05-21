
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/GDAL/OGRVectorLayer.h"

#include "Minerva/Core/DataObjects/DataObject.h"
#include "Minerva/Core/Geometry/Point.h"
#include "Minerva/Core/Factory/Readers.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Scope/Caller.h"

#include "ogr_api.h"
#include "ogrsf_frmts.h"

using namespace Minerva::Layers::GDAL;

///////////////////////////////////////////////////////////////////////////////
//
//  Register readers with the factory.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  Minerva::Core::Factory::RegisterReader < Minerva::Core::Factory::TypeCreator < OGRVectorLayer > > _creator_for_Shp ( "ESRI Shape File (*.shp)", "*.shp" );
}

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( OGRVectorLayer, OGRVectorLayer::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( OGRVectorLayer );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OGRVectorLayer::OGRVectorLayer() : BaseClass(),
  _filename()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OGRVectorLayer::~OGRVectorLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query Interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* OGRVectorLayer::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IRead::IID:
    return static_cast < Usul::Interfaces::IRead* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void OGRVectorLayer::read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  OGRDataSource *dataSource ( OGRSFDriverRegistrar::Open( filename.c_str(), FALSE ) );
  if( 0x0 == dataSource )
  {
    throw std::runtime_error ( "Error 5296869660: Could not open file: " + filename );
  }

  // Get the number of layers.
  const int layers ( dataSource->GetLayerCount() );

  // Check to make sure we have layers.
  if ( layers <= 0 )
    throw std::runtime_error ( "Error 2518191347: " + filename + " contains no layers." );

  // Loop over the layers.
  for ( int i = 0; i < layers; ++i )
  {
    this->_addLayer ( dataSource->GetLayer ( i ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void OGRVectorLayer::_addLayer ( OGRLayer* layer )
{
  if ( 0x0 == layer )
    return;

  // Get the spatial reference.
  OGRSpatialReference *src ( layer->GetSpatialRef() );

  // We are going to wgs 84.
  OGRSpatialReference dst;
  dst.SetWellKnownGeogCS ( "WGS84" );
  OGRCoordinateTransformation *transform ( ::OGRCreateCoordinateTransformation( src, &dst ) );

  // Make sure the transformation is destroyed.
  Usul::Scope::Caller::RefPtr destroyTransform ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( transform, ::OCTDestroyCoordinateTransformation ) ) );

  layer->ResetReading();

  OGRFeature *feature ( 0x0 );

  while ( 0x0 != ( feature = layer->GetNextFeature() ) )
  {
    // Get the geometry.
    OGRGeometry *geometry ( feature->GetGeometryRef() );

    if ( 0x0 != geometry )
    {
      // Make a data object.
      Minerva::Core::DataObjects::DataObject::RefPtr dataObject ( new Minerva::Core::DataObjects::DataObject );

      switch ( geometry->getGeometryType() )
      {
      case wkbPoint:
      case wkbLineString:
      case wkbLinearRing:
      case wkbPolygon:
        dataObject->addGeometry ( this->_createGeoemtry ( geometry, transform ) );
        break;
      case wkbMultiPoint:
      case wkbMultiLineString:
      case wkbMultiPolygon:
        {
          OGRGeometryCollection* collection ( static_cast<OGRGeometryCollection*> ( geometry ) );
          for ( int i = 0; i < collection->getNumGeometries(); ++i )
            dataObject->addGeometry ( this->_createGeoemtry ( collection->getGeometryRef ( i ), transform ) );
        }
        break;
      }

      // Add the data object.
      this->add ( Usul::Interfaces::IUnknown::QueryPtr ( dataObject ) );
    }

    // Destroy the feature.
    OGRFeature::DestroyFeature ( feature );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a point.
//
///////////////////////////////////////////////////////////////////////////////

OGRVectorLayer::Geometry* OGRVectorLayer::_createGeoemtry ( OGRGeometry* geometry, OGRCoordinateTransformation *transform )
{
  if ( 0x0 == geometry )
    return 0x0;

  switch ( geometry->getGeometryType() )
  {
  case wkbPoint:
    return this->_createPoint ( static_cast<OGRPoint*> ( geometry ), transform );
    break;
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a point.
//
///////////////////////////////////////////////////////////////////////////////

OGRVectorLayer::Geometry* OGRVectorLayer::_createPoint ( OGRPoint* geometry, OGRCoordinateTransformation *transform )
{
  Minerva::Core::Geometry::Point::RefPtr point ( new Minerva::Core::Geometry::Point );
  
  if ( 0x0 != geometry )
  {
    // Declare the point.
    Usul::Math::Vec3d p ( geometry->getX(), geometry->getY(), geometry->getZ() );

    // Transform the point.
    if ( 0x0 != transform )
    {
      transform->Transform ( 1, &p[0], &p[1] );
    }

    // Set the point.
    point->point ( p );
  }

  return point.release();
}
