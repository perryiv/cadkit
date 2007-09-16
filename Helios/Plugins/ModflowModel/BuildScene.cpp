
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Utility functions for helping build the scene.
//
///////////////////////////////////////////////////////////////////////////////

#include "BuildScene.h"
#include "Constants.h"

#include "Usul/Bits/Bits.h"
#include "Usul/Trace/Trace.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Add the quad.
//
///////////////////////////////////////////////////////////////////////////////

void BuildScene::addQuads ( unsigned int flags, double x, double y, double top, double bottom, 
                            const osg::Vec2d &half, osg::Vec3Array *vertices, osg::Vec3Array *normals )
{
  USUL_TRACE_SCOPE_STATIC;

  if ( 0x0 == vertices || 0x0 == normals )
    return;

  if ( Usul::Bits::has ( flags, Modflow::Flags::TOP ) )
  {
    vertices->push_back ( osg::Vec3f ( x + half[0], y - half[1], top ) );
    vertices->push_back ( osg::Vec3f ( x + half[0], y + half[1], top ) );
    vertices->push_back ( osg::Vec3f ( x - half[0], y + half[1], top ) );
    vertices->push_back ( osg::Vec3f ( x - half[0], y - half[1], top ) );

    const osg::Vec3f n ( 0, 0, 1 );
    normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n );
  }

  if ( Usul::Bits::has ( flags, Modflow::Flags::BOTTOM ) )
  {
    vertices->push_back ( osg::Vec3f ( x - half[0], y - half[1], bottom ) );
    vertices->push_back ( osg::Vec3f ( x - half[0], y + half[1], bottom ) );
    vertices->push_back ( osg::Vec3f ( x + half[0], y + half[1], bottom ) );
    vertices->push_back ( osg::Vec3f ( x + half[0], y - half[1], bottom ) );

    const osg::Vec3f n ( 0, 0, -1 );
    normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n );
  }

  if ( Usul::Bits::has ( flags, Modflow::Flags::EAST ) )
  {
    vertices->push_back ( osg::Vec3f ( x + half[0], y - half[1], bottom ) );
    vertices->push_back ( osg::Vec3f ( x + half[0], y + half[1], bottom ) );
    vertices->push_back ( osg::Vec3f ( x + half[0], y + half[1], top ) );
    vertices->push_back ( osg::Vec3f ( x + half[0], y - half[1], top ) );

    const osg::Vec3f n ( 1, 0, 0 );
    normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n );
  }

  if ( Usul::Bits::has ( flags, Modflow::Flags::WEST ) )
  {
    vertices->push_back ( osg::Vec3f ( x - half[0], y + half[1], bottom ) );
    vertices->push_back ( osg::Vec3f ( x - half[0], y - half[1], bottom ) );
    vertices->push_back ( osg::Vec3f ( x - half[0], y - half[1], top ) );
    vertices->push_back ( osg::Vec3f ( x - half[0], y + half[1], top ) );

    const osg::Vec3f n ( -1, 0, 0 );
    normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n );
  }

  if ( Usul::Bits::has ( flags, Modflow::Flags::NORTH ) )
  {
    vertices->push_back ( osg::Vec3f ( x + half[0], y + half[1], bottom ) );
    vertices->push_back ( osg::Vec3f ( x - half[0], y + half[1], bottom ) );
    vertices->push_back ( osg::Vec3f ( x - half[0], y + half[1], top ) );
    vertices->push_back ( osg::Vec3f ( x + half[0], y + half[1], top ) );

    const osg::Vec3f n ( 0, 1, 0 );
    normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n );
  }

  if ( Usul::Bits::has ( flags, Modflow::Flags::SOUTH ) )
  {
    vertices->push_back ( osg::Vec3f ( x - half[0], y - half[1], bottom ) );
    vertices->push_back ( osg::Vec3f ( x + half[0], y - half[1], bottom ) );
    vertices->push_back ( osg::Vec3f ( x + half[0], y - half[1], top ) );
    vertices->push_back ( osg::Vec3f ( x - half[0], y - half[1], top ) );

    const osg::Vec3f n ( 0, -1, 0 );
    normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n ); normals->push_back ( n );
  }
}
