
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

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( Geometry, Geometry::BaseClass );

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
_offset( 0.0, 0.0, 0.0 ),
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
      // Index 0 contains longitude, index 1 contains latitude.
      ossimGpt gpt ( (*iter)[1], (*iter)[0] ); // Lat is first agrument, long is second.
      
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

osg::Vec3f Geometry::geometryCenter ( )
{
  return this->geometryCenter ( _offset );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get center of geometry at given id.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3f  Geometry::geometryCenter ( const osg::Vec3f& offset )
{
  std::ostringstream os;
  os << "SELECT x(centroid(" << _tableName << ".geom)) as x_c, y(centroid(" << _tableName << ".geom)) as y_c, srid(geom) as srid FROM " << _tableName << " WHERE id = " << _id;

  pqxx::result r ( _connection->executeQuery( os.str() ) );

  osg::Vec3f center ( 0.0, 0.0, 0.0 );

  if( !r.empty() && !r[0][0].is_null() && !r[0][1].is_null() )
  {
    Usul::Types::Float64 x ( r[0][0].as< float > () );
    Usul::Types::Float64 y ( r[0][1].as< float > () );

    center.set( offset.x() + x, offset.y() + y, 0.0 );
  }

  ossimMapProjection *mapProj = dynamic_cast < ossimMapProjection * > ( _projection.get() );

  if( this->_isSridSphereical( _srid ) )
  {
    Magrathea::convertToEarthCoordinates( center, offset.z() );
  }
  else if( _projection.valid() && 0x0 != mapProj )
  {
    Magrathea::convertToEarthCoordinates( center, mapProj, offset.z() );
  }

  return center;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the offset.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::spatialOffset( const osg::Vec3f& value )
{
  if( _offset != value )
  {
    _offset = value;
    this->dirty( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the offset.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f& Geometry::spatialOffset( ) const
{
  return _offset;
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


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Geometry::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IGeometryCenter::IID:
    return static_cast < Usul::Interfaces::IGeometryCenter* > ( this );
  case Usul::Interfaces::IOffset::IID:
    return static_cast < Usul::Interfaces::IOffset* > ( this );
  default:
    return 0x0;
  }
}
