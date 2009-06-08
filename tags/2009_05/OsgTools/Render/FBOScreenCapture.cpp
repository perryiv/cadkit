
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Render/FBOScreenCapture.h"

#ifndef USE_DEPRECATED_API
#define USE_DEPRECATED_API
#endif

#include "osg/Texture2D"
#include "osg/CameraNode"
#include "osg/Image"
#include "osg/Version"

#include "osgUtil/SceneView"

using namespace OsgTools::Render;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FBOScreenCapture::FBOScreenCapture () :
  _size (),
  _color (),
  _viewMatrix ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FBOScreenCapture::~FBOScreenCapture ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void FBOScreenCapture::size ( unsigned int width, unsigned int height )
{
  _size.set ( width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void FBOScreenCapture::size ( const Usul::Math::Vec2ui& size )
{
  _size = size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Math::Vec2ui& FBOScreenCapture::size () const
{
  return _size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the clear color.
//
///////////////////////////////////////////////////////////////////////////////

void FBOScreenCapture::clearColor ( const osg::Vec4& color )
{
  _color = color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the clear color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4& FBOScreenCapture::clearColor () const
{
  return _color;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

void FBOScreenCapture::viewMatrix ( const osg::Matrix& matrix )
{
  _viewMatrix = matrix;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Matrix& FBOScreenCapture::viewMatrix () const
{
  return _viewMatrix;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the image.
//
///////////////////////////////////////////////////////////////////////////////

void FBOScreenCapture::operator () ( osg::Image& image, osgUtil::SceneView& sceneView, const osg::Matrix& projection )
{
  unsigned int width  ( _size [ 0 ] );
  unsigned int height ( _size [ 1 ] );

  // Set up the texture.
  osg::ref_ptr< osg::Texture2D > tex ( new osg::Texture2D );
  tex->setTextureSize(width, height);
  tex->setInternalFormat( GL_RGBA );
  tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
  tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
  tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
  tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
  tex->setResizeNonPowerOfTwoHint ( false );
  
  // Make the fbo.
  osg::ref_ptr< osg::FrameBufferObject > fbo ( new osg::FrameBufferObject );
#if OPENSCENEGRAPH_MAJOR_VERSION <= 2 && OPENSCENEGRAPH_MINOR_VERSION <= 4
  fbo->setAttachment(GL_COLOR_ATTACHMENT0_EXT, osg::FrameBufferAttachment(tex.get()));
  fbo->setAttachment(GL_DEPTH_ATTACHMENT_EXT, osg::FrameBufferAttachment(new osg::RenderBuffer(width, height, GL_DEPTH_COMPONENT24)));
#else
  fbo->setAttachment(osg::Camera::COLOR_BUFFER0, osg::FrameBufferAttachment(tex.get()));
  fbo->setAttachment(osg::Camera::DEPTH_BUFFER, osg::FrameBufferAttachment(new osg::RenderBuffer(width, height, GL_DEPTH_COMPONENT24)));
#endif

  // Make the camera buffer.
  osg::ref_ptr< osg::CameraNode > camera ( new osg::CameraNode );
  camera->setClearColor( this->clearColor() );
  camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  camera->setViewport(0, 0, width, height);

  // Set the camera to render before the main camera.
  camera->setRenderOrder( osg::CameraNode::PRE_RENDER );

  // Set the projection matrix.
  camera->setProjectionMatrix ( projection );

  // Set view.
  camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  camera->setViewMatrix ( this->viewMatrix() );

  // Tell the camera to use OpenGL frame buffer object where supported.
  camera->setRenderTargetImplementation( osg::CameraNode::FRAME_BUFFER_OBJECT );

  // Attach the texture and use it as the color buffer.
  camera->attach ( osg::CameraNode::COLOR_BUFFER, &image );

  // Save the old root.
  osg::ref_ptr< osg::Node > node ( sceneView.getSceneData() );

  // Add the scene to the camera.
  camera->addChild ( node.get() );

  // Make the camera file the scene data.
  sceneView.setSceneData ( camera.get() );

  // Cull.
  sceneView.cull();

  // Render to the image.
  sceneView.draw();

  // Set the old root back to the scene data.
  sceneView.setSceneData ( node.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* FBOScreenCapture::operator () ( osgUtil::SceneView& sceneView, const osg::Matrix& projection )
{
  osg::ref_ptr < osg::Image > image ( new osg::Image );

  // Make enough space
  image->allocateImage ( _size [ 0 ], _size [ 1 ], 1, GL_RGB, GL_UNSIGNED_BYTE );

  (*this ) ( *image, sceneView, projection );

  return image.release();
}
