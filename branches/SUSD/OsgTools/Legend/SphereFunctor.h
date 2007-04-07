
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_LEGEND_SPHERE_FUNCTOR_H__
#define __OSG_TOOLS_LEGEND_SPHERE_FUNCTOR_H__

#include "osg/ref_ptr"
#include "osg/Geometry"

#include "OsgTools/ShapeFactory.h"
#include "OsgTools/State/StateSet.h"

namespace OsgTools {
namespace Legend {

struct SphereFunctor
{
  osg::Geometry* operator() ( unsigned int width, unsigned int height ) const
  {
    OsgTools::ShapeFactory::MeshSize meshSize ( 20, 20 );
    OsgTools::ShapeFactory::LatitudeRange  latRange  ( 89.9f, -89.9f );
    OsgTools::ShapeFactory::LongitudeRange longRange (  0.0f, 360.0f );

    osg::ref_ptr < osg::Geometry > geom ( OsgTools::ShapeFactorySingleton::instance().sphere ( osg::Vec3( width / 2, height/2, 0.0 ), height / 2, meshSize, latRange, longRange  ) );
    //OsgTools::State::StateSet::setLighting ( geom->getOrCreateStateSet(), true );
    //OsgTools::State::StateSet::setNormalize ( geom->getOrCreateStateSet(), true );

    osg::ref_ptr< osg::StateSet > ss ( geom->getOrCreateStateSet() );
    //ss->setMode ( GL_NORMALIZE, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
    ss->setMode ( GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
    ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

    return geom.release();
  }
};

}
}


#endif // __OSG_TOOLS_LEGEND_SPHERE_FUNCTOR_H__
