
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_VOLUME_GPU_RAY_CASTING_H__
#define __OSGTOOLS_VOLUME_GPU_RAY_CASTING_H__

#include "OsgTools/Export.h"
#include "OsgTools/Configure/OSG.h"
#include "OsgTools/Volume/TransferFunction.h"

#include "osg/Image"
#include "osg/Geometry"
#include "osg/Billboard"
#include "osg/Shader"
#include "osg/Uniform"

namespace OsgTools {
namespace Volume {

class OSG_TOOLS_EXPORT GPURayCasting : public osg::Geode
{
public:
  /// Typedefs.
  typedef osg::Geode                             BaseClass;
  typedef osg::Geometry                          Geometry;
  typedef osg::ref_ptr < osg::Image >            ImagePtr;
  typedef unsigned int                           TextureUnit;
  typedef std::pair < ImagePtr, TextureUnit >    TexutreInfo;
  typedef OsgTools::Volume::TransferFunction     TransferFunction;

  /// Construction.
  GPURayCasting();
  GPURayCasting( osg::Program * );
  
  static osg::Program*             createProgram ( bool useTransferFunction = true );

  /// Get/Set the image.
  osg::Image*                      image ();
  const osg::Image*                image () const;
  void                             image ( osg::Image* image, TextureUnit unit = 0 );

  /// Get/Set the sampling rate.
  float                            samplingRate () const;
  void                             samplingRate ( float rate );

  /// Get/Set the bounding box.
  void                             boundingBox ( const osg::BoundingBox& bb );
  const osg::BoundingBox&          boundingBox () const;

  /// Traverse this node.
  void                             traverse ( osg::NodeVisitor &nv );
  
  /// Get/Set the transfer function as an image.
  void                             transferFunction ( TransferFunction* tf, TextureUnit unit = 1 );
  TransferFunction*                transferFunction () const;

  /// Set the camera.
  void                             camera ( const osg::Vec3& camera );

protected:
  virtual ~GPURayCasting();

  void                             _construct();
  static osg::Shader*              _buildVertexShader ();
  static osg::Shader*              _buildFragmentShader ();

private:

  osg::ref_ptr<osg::Program>    _program;
  TexutreInfo                   _volume;
  osg::ref_ptr < Geometry >     _geometry;
  TransferFunction::RefPtr      _transferFunction;
  unsigned int                  _tfTextureUnit;
  osg::BoundingBox              _bb;
  float                         _samplingRate;
  osg::Vec3                     _camera;
  osg::ref_ptr < osg::Uniform > _cameraUniform;
  osg::ref_ptr < osg::Uniform > _minUniform;
  osg::ref_ptr < osg::Uniform > _maxUniform;
  osg::ref_ptr < osg::Uniform > _volumeUniform;
  osg::ref_ptr < osg::Uniform > _tfUniform;
  osg::ref_ptr < osg::Uniform > _rateUniform;
};

}
}

#endif // __OSGTOOLS_VOLUME_GPU_RAY_CASTING_H__
