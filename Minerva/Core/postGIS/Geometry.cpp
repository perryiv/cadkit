
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/postGIS/Geometry.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IProjectCoordinates.h"

using namespace Minerva::Core::postGIS;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( Geometry, Geometry::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Geometry::Geometry(  ) : BaseClass(),
_connection ( 0x0 ),
_tableName ( ),
_id ( -1 ),
_srid ( 0 ) ,
_offset( 0.0, 0.0, 0.0 ),
_dirty ( false )
{
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

//template < class Vertices >
//void Geometry::_convertToLatLong ( const Vertices& vertices, std::vector< ossimGpt >& latLongPoints )
//{
//  Usul::Interfaces::IProjectCoordinates::QueryPtr project ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IProjectCoordinates::IID ) );
//
//  if ( project.valid() )
//  {
//    for( typename Vertices::const_iterator iter = vertices.begin(); iter != vertices.end(); ++iter )
//    {
//      Usul::Math::Vec3d orginal ( (*iter)[0], (*iter)[1], 0.0 );
//      Usul::Math::Vec3d point;
//      project->projectToSpherical( orginal, _srid, point );
//
//      ossimGpt gpt ( point[1], point[0], point[2] ); // Lat is first agrument, long is second.
//      
//      latLongPoints.push_back( gpt );
//    }
//  }
//}


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

  osg::Vec3f center ( 0.0, 0.0, 0.0 );

  if( 0x0 != _connection.get() )
  {
    pqxx::result r ( _connection->executeQuery( os.str() ) );

    if( !r.empty() && !r[0][0].is_null() && !r[0][1].is_null() )
    {
      Usul::Types::Float64 x ( r[0][0].as< float > () );
      Usul::Types::Float64 y ( r[0][1].as< float > () );

      center.set( offset.x() + x, offset.y() + y, offset.z() );
    }
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
  return ( _srid > 0 );
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
