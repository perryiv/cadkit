
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/EsriShapeFile/EsriShapeFileDocument.h"

#include "Usul/Components/Manager.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Format.h"
#include "Usul/Trace/Trace.h"

#include "osg/Vec3d"

#include "ogr_api.h"
#include "ogrsf_frmts.h"

#include <vector>
#include <fstream>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//
//  Initialize Gdal.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct Init
  {
    Init()
    {
      OGRRegisterAll();
    }
    ~Init()
    {
      OGRCleanupAll();
    }
  } _init;
}


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( EsriShapeFileDocument, EsriShapeFileDocument::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

EsriShapeFileDocument::EsriShapeFileDocument() : 
  BaseClass ( "Esri Shape File Document" ),
	_driver ( 0x0 ),
  _positions( 0 ),
  _measurement( 0.0 )
{
  USUL_TRACE_SCOPE;

	const std::string driverName ( "ESRI Shapefile" );
	_driver = ::OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( driverName.c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

EsriShapeFileDocument::~EsriShapeFileDocument()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void EsriShapeFileDocument::clear ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *EsriShapeFileDocument::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IArcGenReaderWriter::IID:
    return static_cast < Usul::Interfaces::IArcGenReaderWriter* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool EsriShapeFileDocument::canExport ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool EsriShapeFileDocument::canInsert ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool EsriShapeFileDocument::canOpen ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool EsriShapeFileDocument::canSave ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "shp" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void EsriShapeFileDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
  USUL_TRACE_SCOPE;
  
	const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );

	if ( "shp" == ext )
		this->_writeShapeFile ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void EsriShapeFileDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

EsriShapeFileDocument::Filters EsriShapeFileDocument::filtersSave() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
	filters.push_back ( Filter ( "ESRI ShapeFile (*.shp)", "*.shp" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

EsriShapeFileDocument::Filters EsriShapeFileDocument::filtersExport() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

EsriShapeFileDocument::Filters EsriShapeFileDocument::filtersOpen() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

EsriShapeFileDocument::Filters EsriShapeFileDocument::filtersInsert() const
{
  USUL_TRACE_SCOPE;
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write a shape file.
//
///////////////////////////////////////////////////////////////////////////////

void EsriShapeFileDocument::_writeShapeFile ( const std::string& filename ) const
{
	USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

	// Get the driver.
	OGRSFDriver *driver ( this->driver() );

	if ( 0x0 == driver )
		throw std::runtime_error ( "Error 5496285230: Could not find driver for " + filename + "." );

	OGRDataSource *dataSource ( driver->CreateDataSource( filename.c_str(), 0x0 ) );

	if ( 0x0 == dataSource )
		throw std::runtime_error ( "Error 3162280902: Could not create file " + filename + "." );

	OGRLayer *layer ( dataSource->CreateLayer( "line_out", 0x0, wkbLineString, 0x0 ) );
  
	if( 0x0 == layer )
		throw std::runtime_error ( "Error 4257001710: Could not create layer in " + filename + "." );

	// Make a field for the length.
	OGRFieldDefn length ( "Length", OFTReal );

	if ( OGRERR_NONE != layer->CreateField ( &length ) )
		throw std::runtime_error ( "Error 3402584574: Could not create field in " + filename + "." );

	OGRFeature *feature ( OGRFeature::CreateFeature( layer->GetLayerDefn() ) );

	if ( 0x0 != feature )
	{
		feature->SetField( "Length", _measurement );

		OGRLineString line;
    
		for( Positions::const_iterator iter = _positions.begin(); iter < _positions.end(); ++iter )
		{
			// Get the position.
			Positions::value_type position ( *iter );

			// Make the point.
			OGRPoint pt ( position[0], position[1], position[2] );

			// Add the point.
			line.addPoint ( &pt );
		}

    feature->SetGeometry( &line ); 

    if( OGRERR_NONE != layer->CreateFeature( feature )  )
    {
			throw std::runtime_error ( "Error 9646379370: Failed to create feature in " + filename + "." );
    }

     OGRFeature::DestroyFeature( feature );
	}

	OGRDataSource::DestroyDataSource( dataSource );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the positions vector.
//
///////////////////////////////////////////////////////////////////////////////

void EsriShapeFileDocument::setPolyLineVertices ( Positions p )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _positions = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the recorded measurement value.
//
///////////////////////////////////////////////////////////////////////////////

void EsriShapeFileDocument::measurement( double m )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _measurement = m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the driver.
//
///////////////////////////////////////////////////////////////////////////////

OGRSFDriver* EsriShapeFileDocument::driver() const
{
	USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
	return _driver;
}
