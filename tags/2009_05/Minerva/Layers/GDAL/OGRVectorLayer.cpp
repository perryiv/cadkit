
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/GDAL/OGRVectorLayer.h"

#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Point.h"
#include "Minerva/Core/Data/Line.h"
#include "Minerva/Core/Data/Polygon.h"
#include "Minerva/Core/Factory/Readers.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
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
  _filename(),
  _lineStyle ( new Minerva::Core::Data::LineStyle )
{
  _lineStyle->width ( 2.0f );
  _lineStyle->color ( Usul::Math::Vec4f ( 1.0, 1.0, 0.0, 1.0 ) );
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
    this->_addLayer ( dataSource->GetLayer ( i ), progress );
    
    // Notify any listeners that the data has changed.
    this->_notifyDataChangedListeners();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void OGRVectorLayer::_addLayer ( OGRLayer* layer, Usul::Interfaces::IUnknown *unknown )
{
  if ( 0x0 == layer )
    return;
  
  // Query for a progress bar.
  Usul::Interfaces::IProgressBar::UpdateProgressBar progress ( 0.0, 1.0, unknown );

  // Get the spatial reference.
  OGRSpatialReference *src ( layer->GetSpatialRef() );

  // We are going to wgs 84.
  OGRSpatialReference dst;
  dst.SetWellKnownGeogCS ( "WGS84" );
  OGRCoordinateTransformation *transform ( ::OGRCreateCoordinateTransformation( src, &dst ) );

  // Make sure the transformation is destroyed.
  Usul::Scope::Caller::RefPtr destroyTransform ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( transform, ::OCTDestroyCoordinateTransformation ) ) );
  
  // Get the number of features.
  const unsigned int numFeatures ( layer->GetFeatureCount() );

  layer->ResetReading();

  OGRFeature *feature ( 0x0 );
  
  unsigned int i ( 0 );

  // Get the features.
  while ( 0x0 != ( feature = layer->GetNextFeature() ) )
  {
    // Update the progress.
    progress ( ++i, numFeatures );
    
    // Get the geometry.
    OGRGeometry *geometry ( feature->GetGeometryRef() );

    if ( 0x0 != geometry )
    {
      // Make a data object.
      Minerva::Core::Data::DataObject::RefPtr dataObject ( new Minerva::Core::Data::DataObject );

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
      this->add ( Usul::Interfaces::IUnknown::QueryPtr ( dataObject ), false );
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

OGRVectorLayer::Geometry* OGRVectorLayer::_createGeoemtry ( OGRGeometry* geometry, OGRCoordinateTransformation *transform ) const
{
  if ( 0x0 == geometry )
    return 0x0;

  switch ( geometry->getGeometryType() )
  {
  case wkbPoint:
    return this->_createPoint ( static_cast<OGRPoint*> ( geometry ), transform );
  case wkbLineString:
  case wkbLinearRing:
    return this->_createLine ( static_cast<OGRLineString*> ( geometry ), transform );
  case wkbPolygon:
    return this->_createPolygon ( static_cast<OGRPolygon*> ( geometry ), transform );
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert and transform a point.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline Usul::Math::Vec3d convertAndTransform ( const OGRPoint& point, OGRCoordinateTransformation* transform )
  {
    // Declare the point.
    Usul::Math::Vec3d p ( point.getX(), point.getY(), point.getZ() );

    // Transform the point.
    if ( 0x0 != transform )
    {
      transform->Transform ( 1, &p[0], &p[1], &p[2] );
    }

    return p;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert and transform a line string.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template <class Vertices>
  inline void convertAndTransform ( Vertices &vertices, const OGRLineString& line, OGRCoordinateTransformation* transform )
  {
    OGRPoint point;

    const int numPoints ( line.getNumPoints() );
    vertices.reserve ( numPoints );
    for ( int i = 0; i < numPoints; ++i )
    {
      line.getPoint ( i, &point );
      vertices.push_back ( Helper::convertAndTransform ( point, transform ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a point.
//
///////////////////////////////////////////////////////////////////////////////

OGRVectorLayer::Geometry* OGRVectorLayer::_createPoint ( OGRPoint* geometry, OGRCoordinateTransformation *transform ) const
{
  Minerva::Core::Data::Point::RefPtr point ( new Minerva::Core::Data::Point );
  
  if ( 0x0 != geometry )
  {
    // Set the point.
    point->point ( Helper::convertAndTransform ( *geometry, transform ) );

    // Set reasonable defaults.
    point->size ( 5 );
    point->color ( Usul::Math::Vec4f ( 1.0, 0.0, 0.0, 1.0 ) );
  }

  return point.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a line.
//
///////////////////////////////////////////////////////////////////////////////

OGRVectorLayer::Geometry* OGRVectorLayer::_createLine ( OGRLineString* geometry, OGRCoordinateTransformation *transform ) const
{
  typedef Minerva::Core::Data::Line Line;
  typedef Minerva::Core::Data::LineStyle LineStyle;
  typedef Line::Vertices Vertices;

  Line::RefPtr line ( new Line );

  if ( 0x0 != geometry )
  {
    Vertices vertices;
    Helper::convertAndTransform ( vertices, *geometry, transform );

    line->line ( vertices );
  }

  line->lineStyle ( _lineStyle.get() );

  return line.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a polygon.
//
///////////////////////////////////////////////////////////////////////////////

OGRVectorLayer::Geometry* OGRVectorLayer::_createPolygon ( OGRPolygon* geometry, OGRCoordinateTransformation *transform ) const
{
  typedef Minerva::Core::Data::Polygon Polygon;
  typedef Polygon::Vertices Vertices;
  typedef Minerva::Core::Data::LineStyle LineStyle;
  typedef Minerva::Core::Data::PolyStyle PolyStyle;

  Polygon::RefPtr polygon ( new Polygon );

  if ( 0x0 != geometry )
  {
    // Add the outer rings.
    OGRLinearRing *outer ( geometry->getExteriorRing() );
    if ( 0x0 != outer )
    {
      Vertices vertices;
      Helper::convertAndTransform ( vertices, *outer, transform );
      polygon->outerBoundary ( vertices );
    }

    // Add the inner rings.
    const int numRings ( geometry->getNumInteriorRings() );
    for ( int i = 0; i < numRings; ++i )
    {
      OGRLinearRing* ring ( geometry->getInteriorRing ( i ) );
      if ( 0x0 != ring )
      {
        Vertices vertices;
        Helper::convertAndTransform ( vertices, *outer, transform );
        polygon->addInnerBoundary ( vertices );
      }
    }
  }
  
  LineStyle::RefPtr lineStyle ( new LineStyle );
  lineStyle->width ( 1.0f );
  lineStyle->color ( Usul::Math::Vec4f ( 1.0f, 1.0f, 0.0f, 1.0f ) );
  
  PolyStyle::RefPtr polyStyle ( new PolyStyle );
  polyStyle->fill ( true );
  polyStyle->outline ( true );
  polyStyle->color ( Usul::Math::Vec4f ( 0.8f, 0.8f, 0.8f, 1.0f ) );
  
  polygon->lineStyle ( lineStyle.get() );
  polygon->polyStyle ( polyStyle.get() );

  return polygon.release();
}
