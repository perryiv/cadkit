
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Geometry/Point.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IProjectCoordinates.h"

using namespace Minerva::Core::Geometry;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Point, Point::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Point::Point() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Point::~Point()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the point
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec3d Point::pointData( )
{
  Usul::Math::Vec3d point;

  Usul::Interfaces::IProjectCoordinates::QueryPtr project ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IProjectCoordinates::IID ) );

  if( project.valid() )
  {
    osg::Vec3f offset ( this->spatialOffset() );
    int srid ( this->srid() );
    
    point.set( _point[0] + offset[0], _point[1] + offset[1], _point[2] + offset[2] );

    Usul::Math::Vec3d latLongPoint;
    project->projectToSpherical( point, srid, latLongPoint );
    return latLongPoint;
  }

  return _point;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Point::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Minerva::Interfaces::IPointData::IID:
    return static_cast < Minerva::Interfaces::IPointData* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the point.
//
///////////////////////////////////////////////////////////////////////////////

void Point::point( const Usul::Math::Vec3d& p )
{
  _point = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the point.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Math::Vec3d Point::point() const
{
  return _point;
}
