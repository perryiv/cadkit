
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions for creating common bodies.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Functions/MakeBody.h"

#include "Minerva/Core/TileEngine/Body.h"
#include "Minerva/Core/TileEngine/LandModelEllipsoid.h"

#include "osg/CoordinateSystemNode"

///////////////////////////////////////////////////////////////////////////////
//
//  Make a body for the earth.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::TileEngine::Body* Minerva::Core::Functions::makeEarth ( Usul::Jobs::Manager *manager )
{
  // Local typedefs to shorten lines.
  typedef Minerva::Core::TileEngine::Body Body;
  typedef Body::Extents Extents;
  typedef Minerva::Core::TileEngine::LandModelEllipsoid Land;

  // Make the land model.
  Land::Vec2d radii ( osg::WGS_84_RADIUS_EQUATOR, osg::WGS_84_RADIUS_POLAR );
  Land::RefPtr land ( new Land ( radii ) );

  // Make a good split distance.
  const double splitDistance ( osg::WGS_84_RADIUS_EQUATOR * 3.0 ); // Was 2.5

  // Size of the mesh.
  Body::MeshSize meshSize ( 17, 17 );

  // Add the body.
  Body::RefPtr body ( new Body ( land, manager, meshSize, splitDistance ) );
  body->name ( "Earth" );
  body->useSkirts ( true );

  // Add tiles to the body.
  body->addTile ( Extents ( -180, -90,    0,   90 ) );
  body->addTile ( Extents (    0, -90,  180,   90 ) );

  return body.release();
}
