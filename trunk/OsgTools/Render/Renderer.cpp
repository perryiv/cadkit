
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  Copyright (c) 2005, Perry L. Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Renderer.h"

#include "OsgTools/Jitter.h"
#include "OsgTools/Render/RecordTime.h"
#include "OsgTools/ScopedProjection.h"

#include "Usul/Errors/Checker.h"
#include "Usul/Bits/Bits.h"
#include "Usul/System/Clock.h"
#include "Usul/Errors/Stack.h"

#include "osgUtil/UpdateVisitor"

using namespace OsgTools::Render;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Renderer::Renderer() : BaseClass(),
_sceneView ( new osgUtil::SceneView ),
_times(),
_numPasses ( 1 ),
_hasAccumulationBuffer ( false )
{
  // Set the update-visitor.
  _sceneView->setUpdateVisitor ( new osgUtil::UpdateVisitor );

  // Set the display settings.
  _sceneView->setDisplaySettings ( new osg::DisplaySettings() );

  // Set the viewer's defaults.
  _sceneView->setDefaults();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Renderer::~Renderer()
{
  _sceneView = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::init()
{
  GLint red ( 0 ), green ( 0 ), blue ( 0 ), alpha ( 0 );
  ::glGetIntegerv ( GL_ACCUM_RED_BITS,   &red   );
  ::glGetIntegerv ( GL_ACCUM_GREEN_BITS, &green );
  ::glGetIntegerv ( GL_ACCUM_BLUE_BITS,  &blue  );
  ::glGetIntegerv ( GL_ACCUM_ALPHA_BITS, &alpha );
  const bool hasAccum ( red && green && blue && alpha );
  _hasAccumulationBuffer = hasAccum;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::clear()
{
  _sceneView->releaseAllGLObjects();
  _sceneView->flushAllDeletedGLObjects();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is there an accumulation buffer?
//
///////////////////////////////////////////////////////////////////////////////

bool Renderer::hasAccumBuffer() const
{
  return _hasAccumulationBuffer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the clipping plane distances.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::nearFar ( double &n, double &f ) const
{
  // If we have a valid viewer...
  if ( _sceneView.valid() )
  {
    // Get the projection matrix.
    osg::Matrix P ( _sceneView->getProjectionMatrix() );

    // Try to get perspective parameters.
    double left, right, bottom, top;
    if ( P.getFrustum ( left, right, bottom, top, n, f ) )
      return;

    // Try to get orthographic parameters.
    if ( P.getOrtho ( left, right, bottom, top, n, f ) )
      return;
  }

  // If we get to here then it did not work.
  throw ( std::runtime_error ( "Failed to calculate far clipping distance" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::render()
{
  // Handle no viewer.
  if ( !_sceneView.valid() )
    return;

  // If there is no scene then clear the screen. 
  // Otherwise, weird artifacts may show up.
  if ( !_sceneView->getSceneData() )
  {
    const osg::Vec4 &color = this->backgroundColor();
    ::glClearColor ( color[0], color[1], color[2], color[3] );
    ::glClear ( _sceneView->getRenderStage()->getClearMask() );
  }

  // Handle particles and osg-animations.
  osg::ref_ptr<osg::FrameStamp> fs ( new osg::FrameStamp );
  fs->setFrameNumber ( fs->getFrameNumber() + 1 );
  fs->setReferenceTime ( ::time ( 0x0 )  );
  _sceneView->setFrameStamp ( fs.get() );

  // Draw
  // See if we are supposed to use multiple passes.
  if ( this->numRenderPasses() > 1 )
    this->_multiPassRender();
  else
    this->_singlePassRender();

  // Check for errors.
  USUL_ERROR_CHECKER ( GL_NO_ERROR == ::glGetError() );

}



///////////////////////////////////////////////////////////////////////////////
//
//  Handle single-pass rendering.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::_singlePassRender()
{
  // Handle no viewer.
  if ( !_sceneView.valid() )
    return;

  // Cull and draw.
  this->_cullAndDraw();  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle multi-pass rendering.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::_multiPassRender()
{
  // Handle no viewer.
  if ( !_sceneView.valid() )
    return;

  // Safely...
  try
  {
    // Save original projection matrix.
    OsgTools::ScopedProjection sp ( _sceneView.get() );

    // Clear the accumulation buffer.
    ::glClearAccum ( 0.0f, 0.0f, 0.0f, 0.0f );
    ::glClear ( GL_ACCUM_BUFFER_BIT );

    // Check for errors.
    USUL_ERROR_CHECKER ( GL_NO_ERROR == ::glGetError() );

    // Needed in the loop.
    osg::Matrixd matrix;
    osg::ref_ptr<osg::Viewport> vp ( this->viewport() );
    const osg::Matrixd &proj = _sceneView->getProjectionMatrix();

    // Loop through the passes...
    for ( unsigned int i = 0; i < this->numRenderPasses(); ++i )
    {
      // Set the proper projection matrix.
      OsgTools::Jitter::instance().perspective ( _numPasses, i, *vp, proj, matrix );
      _sceneView->setProjectionMatrix ( matrix );

      // Render a single pass.
      this->_singlePassRender();

      // Check for errors.
      USUL_ERROR_CHECKER ( GL_NO_ERROR == ::glGetError() );

      // Accumulate the pixels from the frame buffer.
      float value ( 1.0f / static_cast < float > ( this->numRenderPasses() ) );
      ::glAccum ( GL_ACCUM, value );

      // Check for errors.
      USUL_ERROR_CHECKER ( GL_NO_ERROR == ::glGetError() );
    }

    // Transfer the accumulation buffer into the frame buffer.
    ::glAccum ( GL_RETURN, 1.0f );

    // Check for errors.
    USUL_ERROR_CHECKER ( GL_NO_ERROR == ::glGetError() );
  }

  // Catch all exceptions and reset the number of passes. Otherwise, you 
  // can get stuck in a loop where the error dialog causes a render, and 
  // the render causes an error dialog, etc.

  // Standard exceptions.
  catch ( const std::exception &e )
  {
    Usul::Errors::Stack::instance().push ( e.what() );
    std::ostringstream message;
    message << "Error 2156758683: Standard exception caught when attempting to render with " << this->numRenderPasses() << " passes";
    this->numRenderPasses ( 1 );
    throw std::runtime_error ( message.str() );
  }

  // Unknown exceptions.
  catch ( ... )
  {
    std::ostringstream message;
    message << "Error 3880806891: Unknown exception caught when attempting to render with " << this->numRenderPasses() << " passes";
    this->numRenderPasses ( 1 );
    throw std::runtime_error ( message.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::backgroundColor ( const osg::Vec4 &color )
{
  // Set the color.
  if ( this->viewer() )
    this->viewer()->setClearColor ( color );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4 &Renderer::backgroundColor() const
{
  if ( !this->viewer() )
    throw ( std::runtime_error ( "Cannot get the background color because there is no viewer" ) );
  return this->viewer()->getClearColor();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewport.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::viewport ( osg::Viewport *vp )
{
  if ( _sceneView.valid() )
    _sceneView->setViewport ( vp );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewport.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::viewport ( int x, int y, unsigned int w, unsigned int h )
{
  if ( _sceneView.valid() )
    _sceneView->setViewport ( x, y, (int) w, (int) h );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewport.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Viewport *Renderer::viewport() const
{
  return ( _sceneView.valid() ) ? _sceneView->getViewport() : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewport.
//
///////////////////////////////////////////////////////////////////////////////

osg::Viewport *Renderer::viewport()
{
  return ( _sceneView.valid() ) ? _sceneView->getViewport() : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

Renderer::SceneView *Renderer::viewer()
{
  return _sceneView.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

const Renderer::SceneView *Renderer::viewer() const
{
  return _sceneView.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewer.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::viewer ( SceneView *viewer )
{
  _sceneView = viewer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle the cull and draw.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::_cullAndDraw()
{
  // Handle no viewer.
  if ( false == _sceneView.valid() )
    return;

  typedef RecordTime< Renderer > RecordTime;

  // Cull.
  {
    RecordTime ct ( *this, "cull" );
    _sceneView->cull();
  }

  // Draw.
  {
    RecordTime dt ( *this, "draw" );
    _sceneView->draw();
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Record the start-time.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::timeStart( const std::string& name )
{
  // Record the time.
  TimePair now ( (double) Usul::System::Clock::milliseconds() / (double) CLOCKS_PER_SEC, 0 );
  _times[name].push_back ( now );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Record the end-time.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::timeStop( const std::string &name )
{
  // Record the end-time.
  TimeHistory &h = _times[name];
  TimePair &p = h.back();
  p.second = (double) Usul::System::Clock::milliseconds() / (double) CLOCKS_PER_SEC;

  // Trim to a reasonable size.
  while ( h.size() > 100 )
    h.pop_front();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last time.
//
///////////////////////////////////////////////////////////////////////////////

double Renderer::timeLast ( const std::string &name ) const
{
  // Try to get the time-history.
  TimeHistories::const_iterator i = _times.find ( name );
  if ( _times.end() == i )
  {
    return 0.0;
  }

  // Return the most recent time.
  const TimeHistory &h = i->second;
  const TimePair &p = h.back();
  return ( p.second - p.first );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the average time.
//
///////////////////////////////////////////////////////////////////////////////

double Renderer::timeAverage ( const std::string &name ) const
{
  // Try to get the time-history.
  TimeHistories::const_iterator i = _times.find ( name );
  if ( _times.end() == i )
  {
    return 0.0;
  }

  // Sum the durations.
  const TimeHistory &h = i->second;
  double sum ( 0 );
  for ( TimeHistory::const_iterator j = h.begin(); j != h.end(); ++j )
  {
    const TimePair &p = *j;
    sum += ( p.second - p.first );
  }

  // Return the average.
  return sum / h.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of rendering passes. Unavailable numbers have no effect.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::numRenderPasses ( unsigned int num )
{
  if ( 1 == num || OsgTools::Jitter::instance().available ( num ) )
    _numPasses = num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set a unique ID for the viewer.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::uniqueID ( unsigned int id )
{
  // Counter for display-list id. OSG will handle using the correct display 
  // list for this context.
  _sceneView->getState()->setContextID ( id );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::scene ( osg::Node *node )
{
  _sceneView->setSceneData ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Node * Renderer::scene() const
{
  return _sceneView->getSceneData ( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * Renderer::scene()
{
  return _sceneView->getSceneData ( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::updateScene()
{
  if( !_sceneView.valid() )
    return;

  typedef RecordTime< Renderer > RecordTime;

  {
    RecordTime ut ( *this, "update" );
    _sceneView->update();
  }
}
