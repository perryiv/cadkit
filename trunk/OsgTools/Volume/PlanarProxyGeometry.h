
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
#include "osg/Version"

#if OSG_VERSION_MAJOR <= 1 && OSG_VERSION_MINOR <= 2
#define DrawArgs osg::State& state
#else
#define DrawArgs osg::RenderInfo& info
#endif

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
  virtual void                drawImplementation( DrawArgs ) const;

  // Set/Get the bounding box.
  void                        boundingBox ( const osg::BoundingBox& bb );
  const osg::BoundingBox&     boundingBox () const;

  // Set/get the number of planes used.
  unsigned int                numPlanes() const { return _numPlanes; }
  void                        numPlanes ( unsigned int num ) { _numPlanes = num; }

protected:

  // Use reference counting.
  virtual ~PlanarProxyGeometry();

  virtual osg::BoundingBox    computeBound() const;

  void                        _drawImplementation( osg::State& state ) const;
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
