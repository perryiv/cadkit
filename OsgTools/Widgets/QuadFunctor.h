
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_LEGEND_QUAD_FUNCTOR_H__
#define __OSG_TOOLS_LEGEND_QUAD_FUNCTOR_H__

#include "osg/ref_ptr"
#include "osg/Geometry"

namespace OsgTools {
namespace Widgets {

struct QuadFunctor
{
  osg::Geometry* operator() ( unsigned int width, unsigned int height ) const
  {
    osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );

    osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );
    vertices->push_back( osg::Vec3 ( 0.0, 0.0, 0.0 ) );
    vertices->push_back( osg::Vec3 ( 0.0, height, 0.0 ) );
    vertices->push_back( osg::Vec3 ( width, height, 0.0 ) );
    vertices->push_back( osg::Vec3 ( width, 0.0, 0.0 ) );

    geometry->setVertexArray ( vertices.get() );

    osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array );
    normals->push_back ( osg::Vec3 ( 0.0, 0.0, 1.0 ) );

    geometry->setNormalArray ( normals.get() );
    geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );

    geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_QUADS, 0, vertices->size() ) );

    return geometry.release();
  }
};

}
}


#endif // __OSG_TOOLS_LEGEND_QUAD_FUNCTOR_H__
