
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/postGIS/Geometry.h"

#include "Magrathea/Project.h"

#pragma warning ( disable : 4561 )
#include "boost/algorithm/string/find.hpp"

#include "osg/Node"

#include "ossim/base/ossimKeywordNames.h"
#include "ossim/base/ossimKeywordlist.h"
#include "ossim/projection/ossimProjectionFactoryRegistry.h"
#include "ossim/projection/ossimMapProjection.h"

using namespace Minerva::Core::postGIS;

Geometry::VerticesCache Geometry::_verticesCache;
Geometry::TriangleCache Geometry::_triangleCache;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Geometry::Geometry( Minerva::Core::DB::Connection *connection, const std::string &tableName, int id, int srid, const pqxx::result::field &F ) : BaseClass(),
_connection ( connection ),
_parser ( new BinaryParser ( F ) ),
_tableName ( tableName ),
_id ( id ),
_srid ( srid ),
_projection ( 0x0 ),
_xOffset( 0.0 ),
_yOffset( 0.0 ),
_zOffset( 0.0 ),
_dirty ( false )
{
  ossimKeywordlist kwl;

  std::ostringstream os;
  os << _srid;

  kwl.add( ossimKeywordNames::PCS_CODE_KW, os.str().c_str() );

  _projection = ossimProjectionFactoryRegistry::instance()->createProjection( kwl );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Geometry::~Geometry()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert to lat long coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::_convertToLatLong ( const Vertices& vertices, std::vector< ossimGpt >& latLongPoints )
{
  ossimMapProjection *mapProj = dynamic_cast < ossimMapProjection * > ( _projection.get() );

  if( this->_isSridSphereical( _srid ) )
  {
    for( Vertices::const_iterator iter = vertices.begin(); iter != vertices.end(); ++iter )
    {
      ossimGpt gpt ( (*iter)[0], (*iter)[1] );
      
      double deltaH = ossimElevManager::instance()->getHeightAboveMSL( gpt );
      if(deltaH == OSSIM_DBL_NAN)
      {
         deltaH = 0.0;
      }

      gpt.height( deltaH + ossimGeoidManager::instance()->offsetFromEllipsoid(gpt) );

      latLongPoints.push_back( gpt );
    }
  }
  else if( _projection.valid() && 0x0 != mapProj )
  {
    Magrathea::convertToLatLong( vertices, latLongPoints, mapProj );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get center of geometry at given id.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3 Geometry::getCenter ( )
{
  return this->getCenter ( this->xOffset(), this->yOffset(), this->zOffset() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get center of geometry at given id.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3  Geometry::getCenter ( float xOffset, float yOffset, float zOffset )
{
  std::ostringstream os;
  os << "SELECT x(centroid(" << _tableName << ".geom)) as x_c, y(centroid(" << _tableName << ".geom)) as y_c, srid(geom) as srid FROM " << _tableName << " WHERE id = " << _id;

  pqxx::result r ( _connection->executeQuery( os.str() ) );

  osg::Vec3f center ( 0.0, 0.0, 0.0 );

  if( !r.empty() && !r[0][0].is_null() && !r[0][1].is_null() )
  {
    Usul::Types::Float64 x ( r[0][0].as< float > () );
    Usul::Types::Float64 y ( r[0][1].as< float > () );

    center.set( x, y, 0.0 );
  }

  ossimMapProjection *mapProj = dynamic_cast < ossimMapProjection * > ( _projection.get() );

  if( this->_isSridSphereical( _srid ) )
  {
    Magrathea::convertToEarthCoordinates( center, zOffset );
  }
  else if( _projection.valid() && 0x0 != mapProj )
  {
    Magrathea::convertToEarthCoordinates( center, mapProj, zOffset );
  }

  return center;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the x offset.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::xOffset( float f )
{
  if( _xOffset != f )
  {
    _xOffset = f;
    this->dirty( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the x offset.
//
///////////////////////////////////////////////////////////////////////////////

float Geometry::xOffset( ) const
{
  return _xOffset;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the y offset.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::yOffset( float f )
{
  if( _yOffset != f )
  {
    _yOffset = f;
    this->dirty( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the y offset.
//
///////////////////////////////////////////////////////////////////////////////

float Geometry::yOffset( ) const
{
  return _yOffset;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the z offset.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::zOffset( float f )
{
  if( _zOffset != f )
  {
    _zOffset = f;
    this->dirty( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the z offset.
//
///////////////////////////////////////////////////////////////////////////////

float Geometry::zOffset( ) const
{
  return _zOffset;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is it valid?
//
///////////////////////////////////////////////////////////////////////////////

bool Geometry::valid() const
{
  return ( _parser.valid() && _srid > 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::dirty ( bool b )
{
  _dirty = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Geometry::dirty () const
{
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given srid spherical?.
//
///////////////////////////////////////////////////////////////////////////////

bool Geometry::_isSridSphereical( int id )
{
  // For now, but I think there is an sql statement that can answer this question.
  return id == 4326 || id == 4269;
}
