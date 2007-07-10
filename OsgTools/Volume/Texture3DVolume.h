
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_VOLUME_3D_TEXTURE_VOLUME_H__
#define __OSGTOOLS_VOLUME_3D_TEXTURE_VOLUME_H__

#include "OsgTools/Export.h"
#include "OsgTools/Volume/PlanarProxyGeometry.h"

#include "osg/Image"
#include "osg/Geode"

namespace OsgTools {
namespace Volume {

class OSG_TOOLS_EXPORT Texture3DVolume : public osg::Geode
{
public:
  /// Typedefs.
  typedef osg::Geode BaseClass;
  typedef OsgTools::Volume::PlanarProxyGeometry Geometry;

  /// Construction.
  Texture3DVolume();

  /// Get/Set the image.
  osg::Image*                      image ();
  const osg::Image*                image () const;
  void                             image ( osg::Image* image );

  /// Get/Set the number of planes.
  unsigned int                     numPlanes () const;
  void                             numPlanes ( unsigned int num );

  // Set/Get the bounding box.
  void                        boundingBox ( const osg::BoundingBox& bb );
  const osg::BoundingBox&     boundingBox () const;

protected:
  virtual ~Texture3DVolume();

  void                             _createShaders ();

private:

  osg::ref_ptr < osg::Image > _image;
  osg::ref_ptr < Geometry >   _geometry;
};

}
}

#endif // __OSGTOOLS_VOLUME_3D_TEXTURE_VOLUME_H__
