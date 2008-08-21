
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

#include <algorithm>

namespace OsgTools {
namespace Widgets {

struct SphereFunctor
{
  osg::Geometry* operator() ( unsigned int width, unsigned int height ) const
  {
    OsgTools::ShapeFactory::MeshSize meshSize ( 20, 20 );
    OsgTools::ShapeFactory::LatitudeRange  latRange  ( 89.9f, -89.9f );
    OsgTools::ShapeFactory::LongitudeRange longRange (  0.0f, 360.0f );

    osg::ref_ptr < osg::Geometry > geom ( OsgTools::ShapeFactory::instance().sphere ( osg::Vec3( width / 2, height/2, 0.0 ), height / 2, meshSize, latRange, longRange  ) );

    // I don't think this is needed any more.
#if 0
#ifdef _MSC_VER
    osg::Vec3Array *normals = dynamic_cast < osg::Vec3Array * > ( geom->getNormalArray() );
    if ( normals )
    {
      // Loop through the normals and negate them.
      std::transform ( normals->begin(), normals->end(), normals->begin(), std::negate<osg::Vec3>() );
    }
#endif
#endif
    
    // Make sure lighting is on.
    OsgTools::State::StateSet::setLighting ( geom->getOrCreateStateSet(), true );

    return geom.release();
  }
};

}
}


#endif // __OSG_TOOLS_LEGEND_SPHERE_FUNCTOR_H__
