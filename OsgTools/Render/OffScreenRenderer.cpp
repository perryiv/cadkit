
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to render to off screen target, without needed a window.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Render/OffScreenRenderer.h"
#include "OsgTools/Render/ActionAdapter.h"
#include "OsgTools/Render/EventAdapter.h"

#include "Usul/Threads/Safe.h"

#include "osg/GraphicsContext"
#include "osg/Image"
#include "osg/Texture2D"

#include "osgDB/WriteFile"

#include "osgGA/TrackballManipulator"

#include <iostream>

using namespace OsgTools::Render;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OffScreenRenderer::OffScreenRenderer() : BaseClass(),
  _viewer ( 0x0 ),
  _sceneManager ( 0x0 ),
  _frameDump(),
  _size ( 0, 0 ),
  _image ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OffScreenRenderer::~OffScreenRenderer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void OffScreenRenderer::init ( unsigned int width, unsigned int height )
{
  // Make the traits for the graphics context we want.
  osg::ref_ptr<osg::GraphicsContext::Traits> traits ( new osg::GraphicsContext::Traits );
  traits->x = 0;
  traits->y = 0;
  traits->width = width;
  traits->height = height;
  traits->red = 8;
  traits->green = 8;
  traits->blue = 8;
  traits->alpha = 8;
  traits->depth = 24;
  traits->windowDecoration = false;
  traits->pbuffer = true; // We want a pixel buffer.
  traits->doubleBuffer = true;
  traits->sharedContext = 0x0;

  // Make the graphics context.
  osg::ref_ptr<osg::GraphicsContext> gc ( osg::GraphicsContext::createGraphicsContext ( traits.get() ) );

  // Make the viewer.
  osg::ref_ptr<osgViewer::Viewer> viewer ( new osgViewer::Viewer );
  viewer->setThreadingModel ( osgViewer::Viewer::SingleThreaded );

  // Set the graphic convert.
  viewer->getCamera()->setGraphicsContext ( gc.get() );

  // Set the target.
  viewer->getCamera()->setRenderTargetImplementation( osg::Camera::PIXEL_BUFFER );

  // Set the size.
  const double fovy   ( 45.0 );
  const double zNear  ( 0.01f );
  const double zFar   ( 100.0 );
  const double w      ( width ), h ( height );
  const double aspect ( w / h );

  viewer->getCamera()->setViewport ( new osg::Viewport  ( 0 ,0, w, h ) );
  viewer->getCamera()->setProjectionMatrixAsPerspective ( fovy, aspect, zNear, zFar );

  // Make the scene manager.
  OsgTools::Render::SceneManager::RefPtr sm ( new OsgTools::Render::SceneManager );

  // Set the viewer's scene data.
  viewer->setSceneData ( sm->scene() );
  
  // Attach an image.
  osg::ref_ptr<osg::Image> image ( new osg::Image );
  image->allocateImage ( width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE );
#if 0
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
  fbo->setAttachment(GL_COLOR_ATTACHMENT0_EXT, osg::FrameBufferAttachment(tex.get()));
  fbo->setAttachment(GL_DEPTH_ATTACHMENT_EXT, osg::FrameBufferAttachment(new osg::RenderBuffer(width, height, GL_DEPTH_COMPONENT24)));
  
  viewer->getCamera()->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
#endif
  viewer->getCamera()->attach ( osg::Camera::COLOR_BUFFER, image.get() );

  // Make sure everything is set up.
  viewer->realize();
  
  viewer->setCameraManipulator( 0x0 );

  // Set the internal members.
  {
    Guard guard ( this->mutex() );
    _viewer = viewer;
    _sceneManager = sm;
    _size.set ( width, height );
    _image = image;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the model.
//
///////////////////////////////////////////////////////////////////////////////

void OffScreenRenderer::model ( osg::Node* node )
{
  OsgTools::Render::SceneManager::RefPtr sm ( Usul::Threads::Safe::get ( this->mutex(), _sceneManager ) );

  if ( sm.valid() )
    sm->model ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render.
//
///////////////////////////////////////////////////////////////////////////////

void OffScreenRenderer::render() const
{
  osg::ref_ptr<osgViewer::Viewer> viewer ( Usul::Threads::Safe::get ( this->mutex(), _viewer ) );

  if ( viewer.valid() )
  {
    // Render the frame.
    viewer->frame();

    osg::ref_ptr<osg::Image> image ( Usul::Threads::Safe::get ( this->mutex(), _image ) );

    FrameDumpPtr fd ( this->frameDump() );
    if ( 0x0 != fd && image.valid() )
      osgDB::writeImageFile ( *image, fd->file() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the frame dump class.
//
///////////////////////////////////////////////////////////////////////////////

void OffScreenRenderer::frameDump ( FrameDumpPtr frameDump )
{
  Guard guard ( this->mutex() );
  _frameDump = frameDump;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame dump class.
//
///////////////////////////////////////////////////////////////////////////////

OffScreenRenderer::FrameDumpPtr OffScreenRenderer::frameDump() const
{
  Guard guard ( this->mutex() );
  return _frameDump;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set look at.
//
///////////////////////////////////////////////////////////////////////////////

void OffScreenRenderer::lookAt ( double ex, double ey, double ez, double cx, double cy, double cz, double ux, double uy, double uz )
{
  osg::ref_ptr<osgViewer::Viewer> viewer ( Usul::Threads::Safe::get ( this->mutex(), _viewer ) );
  if ( viewer.valid() )
  {
    osg::Matrixd matrix;
    matrix.makeLookAt( osg::Vec3d ( ex, ey, ez ), osg::Vec3 ( cx, cy, cx ), osg::Vec3 ( ux, uy, uz ) );
    viewer->getCamera()->setViewMatrix ( matrix );
  }
}
