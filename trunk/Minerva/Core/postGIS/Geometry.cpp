
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

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IProjectCoordinates.h"

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
_tableName ( tableName ),
_id ( id ),
_srid ( srid ),
_projection ( 0x0 ),
_offset( 0.0, 0.0, 0.0 ),
_dirty ( false ),
_orginalVertices()
{
  ossimKeywordlist kwl;

  std::ostringstream os;
  os << _srid;

  kwl.add( ossimKeywordNames::PCS_CODE_KW, os.str().c_str() );

  _projection = ossimProjectionFactoryRegistry::instance()->createProjection( kwl );

  BinaryParser::RefPtr parser ( new BinaryParser( F ) );
  _orginalVertices = parser->getVertices();
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
  Usul::Interfaces::IProjectCoordinates::QueryPtr project ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IProjectCoordinates::IID ) );

  if ( project.valid() )
  {
    for( Vertices::const_iterator iter = vertices.begin(); iter != vertices.end(); ++iter )
    {
      Usul::Math::Vec3d orginal ( (*iter)[0], (*iter)[1], 0.0 );
      Usul::Math::Vec3d point;
      project->projectToSpherical( orginal, _srid, point );

      ossimGpt gpt ( point[0], point[1], point[2] ); // Lat is first agrument, long is second.
      
      latLongPoints.push_back( gpt );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get center of geometry at given id.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3f Geometry::geometryCenter ( unsigned int& srid )
{
  return this->geometryCenter ( _offset, srid );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get center of geometry at given id.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3f  Geometry::geometryCenter ( const osg::Vec3f& offset, unsigned int& srid )
{
  // Set the srid.
  srid = _srid;

  std::ostringstream os;
  os << "SELECT x(centroid(" << _tableName << ".geom)) as x_c, y(centroid(" << _tableName << ".geom)) as y_c, srid(geom) as srid FROM " << _tableName << " WHERE id = " << _id;

  pqxx::result r ( _connection->executeQuery( os.str() ) );

  osg::Vec3f center ( 0.0, 0.0, 0.0 );

  if( !r.empty() && !r[0][0].is_null() && !r[0][1].is_null() )
  {
    Usul::Types::Float64 x ( r[0][0].as< float > () );
    Usul::Types::Float64 y ( r[0][1].as< float > () );

    center.set( offset.x() + x, offset.y() + y, offset.z() );
  }

  /*ossimMapProjection *mapProj = dynamic_cast < ossimMapProjection * > ( _projection.get() );

  if( this->_isSridSphereical( _srid ) )
  {
    Magrathea::convertToEarthCoordinates( center, offset.z() );
  }
  else if( _projection.valid() && 0x0 != mapProj )
  {
    Magrathea::convertToEarthCoordinates( center, mapProj, offset.z() );
  }*/

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
  return ( !_orginalVertices.empty() && _srid > 0 );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Return the orginal vertices from the parser.
//
///////////////////////////////////////////////////////////////////////////////

const Geometry::VertexList& Geometry::_vertices() const
{
  return _orginalVertices;
}
