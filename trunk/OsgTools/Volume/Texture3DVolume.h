
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
  typedef osg::Geode                             BaseClass;
  typedef OsgTools::Volume::PlanarProxyGeometry  Geometry;
  typedef osg::ref_ptr < osg::Image >            ImagePtr;
  typedef unsigned int                           TextureUnit;
  typedef std::pair < ImagePtr, TextureUnit >    TexutreInfo;

  /// Construction.
  Texture3DVolume();

  /// Get/Set the image.
  osg::Image*                      image ();
  const osg::Image*                image () const;
  void                             image ( osg::Image* image, TextureUnit unit = 0 );

  /// Get/Set the number of planes.
  unsigned int                     numPlanes () const;
  void                             numPlanes ( unsigned int num );

  /// Get/Set the bounding box.
  void                             boundingBox ( const osg::BoundingBox& bb );
  const osg::BoundingBox&          boundingBox () const;

  /// Get/Set the transfer function flag.
  void                             useTransferFunction ( bool b );
  bool                             useTransferFunction () const;
  
  /// Get/Set the transfer function as an image.
  void                             transferFunction ( osg::Image* tf, TextureUnit unit = 1 );
  osg::Image*                      transferFunction () const;

protected:
  virtual ~Texture3DVolume();

  void                             _createShaders ();

private:

  enum RenderFlags
  {
    _USE_TRANSFER_FUNCTION = 0x00000001
  };

  TexutreInfo                  _volume;
  osg::ref_ptr < Geometry >    _geometry;
  unsigned int                 _flags;
  TexutreInfo                  _transferFunction;
};

}
}

#endif // __OSGTOOLS_VOLUME_3D_TEXTURE_VOLUME_H__
