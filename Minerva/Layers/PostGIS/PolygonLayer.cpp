
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/PostGIS/PolygonLayer.h"
#include "Minerva/Layers/PostGIS/BinaryParser.h"

#include "Minerva/Core/DataObjects/Polygon.h"
#include "Minerva/Core/Visitor.h"

#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Trace/Trace.h"

#include "osg/Group"

#include "pqxx/pqxx"

#include <algorithm>

USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_4 ( osg::Vec4 );
USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_4 ( osg::Vec4 );
SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER ( osg::Vec4 );

using namespace Minerva::Layers::PostGIS;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( PolygonLayer, PolygonLayer::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( PolygonLayer );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayer::PolygonLayer() : BaseClass(), 
  _showInterior( true ),
  _showBorder( false ),
  _borderColor( 0.0, 0.0, 0.0, 1.0 ),
  _borderWidth ( 1.0f )
{
  USUL_TRACE_SCOPE;
  this->name( "PolygonLayer" );
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayer::PolygonLayer ( const PolygonLayer& layer ) : BaseClass ( layer ),
  _showInterior( layer._showInterior ),
  _showBorder ( layer._showBorder ),
  _borderColor ( layer._borderColor ),
  _borderWidth ( layer._borderWidth )
{
  USUL_TRACE_SCOPE;
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::_registerMembers()
{
  USUL_TRACE_SCOPE;
  SERIALIZE_XML_ADD_MEMBER ( _showInterior );
  SERIALIZE_XML_ADD_MEMBER ( _showBorder );
  SERIALIZE_XML_ADD_MEMBER ( _borderColor );
  SERIALIZE_XML_ADD_MEMBER ( _borderWidth );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::accept ( Minerva::Core::Visitor& visitor )
{
  USUL_TRACE_SCOPE;
  visitor.visit ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone this layer.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* PolygonLayer::clone() const
{
  USUL_TRACE_SCOPE;
  Usul::Interfaces::IUnknown::QueryPtr copy  ( new PolygonLayer( *this ) );
  return copy.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayer::~PolygonLayer()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::buildDataObjects( Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *p )
{
  USUL_TRACE_SCOPE;
  Connection::ScopedConnection scopedConnection ( *this->connection() );

  Usul::Interfaces::IProgressBar::QueryPtr progress ( p );

  std::string dataTable ( this->tablename() );

  // Execute the query.  This will return data to draw.
  pqxx::result geometryResult ( this->connection()->executeQuery ( this->query() ) );

  if( progress.valid() )
    progress->setTotalProgressBar( geometryResult.size() );

  for ( pqxx::result::const_iterator iter = geometryResult.begin(); iter != geometryResult.end(); ++ iter )
  {
    try
    {
      // Id for row
      int id ( iter["id"].as < int > () );
      int srid ( iter["srid"].as < int > () );

      // Create the geometry.
      pqxx::binarystring buffer ( iter["geom"] );
      BinaryParser parser;
      BinaryParser::Geometries geometries ( parser ( &buffer.front() ) );

      for ( BinaryParser::Geometries::iterator geom = geometries.begin(); geom != geometries.end(); ++geom )
      {
        (*geom)->srid( srid );
        Usul::Interfaces::IUnknown::QueryPtr unknown ( *geom );
        Minerva::Interfaces::IOffset::QueryPtr offset ( unknown );

        if( offset.valid() )
          offset->spatialOffset( osg::Vec3f ( 0.0, 0.0, this->zOffset() ) );

        // Create the Data Object.
        Minerva::Core::DataObjects::Polygon::RefPtr data ( new Minerva::Core::DataObjects::Polygon );

        data->width ( this->borderWidth() );
        data->showBorder( this->showBorder() );
        data->showInterior ( this->showInterior() );
        data->geometry ( unknown.get() );
        data->color( this->_color ( iter ) );
        data->borderColor ( this->borderColor() );
        data->objectId ( Usul::Strings::format ( id ) );

        // Set tessellate to true for backwards compatabilty.  Need to make this an option.
        data->tessellate ( true );

        /// Set the label.
        this->_setDataObjectMembers( data.get(), caller );

        // Pre build the scene.
        data->preBuildScene( caller );

        this->add ( Usul::Interfaces::IUnknown::QueryPtr ( data.get() ) );
      }
    }
    catch ( const std::exception& e )
    {
      std::cout << "Error 3149586695: " << e.what() << std::endl;
    }
    catch ( ... )
    {
      std::cout << "Error 3420474749: Exception caught while adding data to layer." << std::endl;
    }

    // Update the progess.
    if( progress.valid() )
    {
      unsigned int num ( iter - geometryResult.begin() );
      progress->updateProgressBar( num );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::modify( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  
  // For now get what we have, clear and then rebuild.
  // Need a way to tell if the query has changed.  Then I think this can be handled better.
  this->clear ();
  this->buildDataObjects ( caller, 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show border flag.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::showBorder( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _showBorder = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show border flag.
//
///////////////////////////////////////////////////////////////////////////////

bool PolygonLayer::showBorder() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _showBorder;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the border color.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::borderColor( const osg::Vec4& color )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _borderColor = color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the border color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4& PolygonLayer::borderColor() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _borderColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show interior flag.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::showInterior( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _showInterior = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show interior flag.
//
///////////////////////////////////////////////////////////////////////////////

bool PolygonLayer::showInterior() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _showInterior;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the border width flag.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::borderWidth( float width )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _borderWidth = width;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the border width flag.
//
///////////////////////////////////////////////////////////////////////////////

float PolygonLayer::borderWidth() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _borderWidth;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* PolygonLayer::queryInterface( unsigned long iid )
{
  USUL_TRACE_SCOPE;
  
  switch ( iid )
  {
  case Minerva::Interfaces::IPolygonLayer::IID:
    return static_cast < Minerva::Interfaces::IPolygonLayer* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}
