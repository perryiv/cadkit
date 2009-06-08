
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_LEGEND_LINE_FUNCTOR_H__
#define __OSG_TOOLS_LEGEND_LINE_FUNCTOR_H__

#include "OsgTools/State/StateSet.h"

#include "osg/ref_ptr"
#include "osg/Geometry"

namespace OsgTools {
namespace Widgets {

struct LineFunctor
{
  LineFunctor() : _lineWidth ( 1.0f )
  {
  }

  /// Get/Set the line width.
  void lineWidth( float f ) { _lineWidth = f; }
  float lineWidth() const { return _lineWidth; }

  /// Build the geometry.
  osg::Geometry* operator() ( unsigned int width, unsigned int height ) const
  {
    osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );

    osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array ( 2 ) );
    (*vertices)[0] = osg::Vec3 ( 0.0,   height / 2, 0.0 );
    (*vertices)[1] = osg::Vec3 ( width, height / 2, 0.0 );

    geometry->setVertexArray( vertices.get() );
    geometry->addPrimitiveSet( new osg::DrawArrays ( osg::PrimitiveSet::LINES, 0, vertices->size() ) );

    OsgTools::State::StateSet::setLineWidth ( geometry->getOrCreateStateSet(), _lineWidth );
    OsgTools::State::StateSet::setLighting ( geometry->getOrCreateStateSet(), false );

    return geometry.release();
  }

private:
  float _lineWidth;
};

}
}


#endif // __OSG_TOOLS_LEGEND_LINE_FUNCTOR_H__
