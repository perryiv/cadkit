
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

#include "Usul/Threads/Safe.h"

#include "osg/GraphicsContext"
#include "osg/Image"

#include "osgDB/WriteFile"

#include "osgGA/TrackballManipulator"

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
  _size ( 0, 0 )
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
  traits->windowDecoration = false;
  traits->pbuffer = true; // We want a pixel buffer.
  traits->doubleBuffer = true;
  traits->sharedContext = 0;

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

  // For now...
  viewer->setCameraManipulator( new osgGA::TrackballManipulator );

  // Make the scene manager.
  OsgTools::Render::SceneManager::RefPtr sm ( new OsgTools::Render::SceneManager );

  // Set the viewer's scene data.
  viewer->setSceneData ( sm->scene() );

  // Make sure everything is set up.
  viewer->realize();

  // Set the internal members.
  {
    Guard guard ( this->mutex() );
    _viewer = viewer;
    _sceneManager = sm;
    _size.set ( width, height );
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
    Usul::Math::Vec2ui size ( Usul::Threads::Safe::get ( this->mutex(), _size ) );

    // Make a new image.
    osg::ref_ptr<osg::Image> image ( new osg::Image );
    image->allocateImage ( size[0], size[1], 1, GL_RGB, GL_UNSIGNED_BYTE );

    viewer->getCamera()->attach ( osg::Camera::COLOR_BUFFER, image.get() );

    viewer->frame();

    viewer->getCamera()->detach ( osg::Camera::COLOR_BUFFER );

    FrameDumpPtr fd ( this->frameDump() );
    if ( 0x0 != fd )
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
