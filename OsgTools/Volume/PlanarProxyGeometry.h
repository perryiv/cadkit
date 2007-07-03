
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_VOLUME_PLANAR_PROXY_GEOMETRY_H__
#define __OSGTOOLS_VOLUME_PLANAR_PROXY_GEOMETRY_H__

#include "OsgTools/Export.h"

#include "osg/Drawable"

namespace OsgTools {
namespace Volume {

class OSG_TOOLS_EXPORT PlanarProxyGeometry : public osg::Drawable
{
public:
  // Typedefs.
  typedef osg::Drawable                            BaseClass;
  typedef std::pair< unsigned int, unsigned int >  Edge;
  typedef std::vector < Edge >                     Edges;
  typedef std::vector < osg::Vec3f >               Corners;

  // Construction.
  PlanarProxyGeometry();
  PlanarProxyGeometry ( const PlanarProxyGeometry &d, const osg::CopyOp &options = osg::CopyOp::SHALLOW_COPY );

  // Assignment.
  PlanarProxyGeometry &operator = ( const PlanarProxyGeometry & );

  // Implementation of osg::Object's cloning functions.
  virtual osg::Object         *clone ( const osg::CopyOp &options ) const;
  virtual osg::Object         *cloneType() const;

  // Draw.
  virtual void                drawImplementation( osg::RenderInfo& renderInfo ) const;

  // Set/get the number of planes used.
  unsigned int                numPlanes() const { return _numPlanes; }
  void                        numPlanes ( unsigned int num ) { _numPlanes = num; }

protected:

  // Use reference counting.
  virtual ~PlanarProxyGeometry();

  virtual osg::BoundingBox    computeBound() const;

  void                        _drawImplementation( osg::RenderInfo& renderInfo ) const;
  void                        _initCornersAndEdges();

private:

  unsigned int      _numPlanes;
  osg::BoundingBox  _bbox;
  Corners           _corners;
  Edges             _edges;
};

}
}

#endif // __OSGTOOLS_VOLUME_PLANAR_PROXY_GEOMETRY_H__
