
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// 
//  SgViewer.cpp: An interactive view class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgViewer.h"
#include "SgPerspectiveCamera.h"
#include "SgOrthographicCamera.h"
#include "SgGetRayIntersection.h"
#include "SgRenderer.h"
#include "SgMessage.h"
#include "SgDefine.h"
#include "SgMessageIds.h"
#include "SgText.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
# include "Standard/SlTrace.h"
# include "Standard/SlTrackball.h"
# include "Standard/SlBitmask.h"
# ifdef __GNUC__
#  include <vector>
# else
#  include <map>
# endif
#endif

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS(SgViewer,SlRefBase);


///////////////////////////////////////////////////////////////////////////////
//
//  I don't want these in a header file.
//
///////////////////////////////////////////////////////////////////////////////

enum // Bits for "_internalFlags".
{
  FIRST_TIME_IN_MOUSEMOVED    = 0x00000001,
  MOUSE_IS_MOVING             = 0x00000002,
  VIEWING_MODE                = 0x00000004,
  SEEKING_MODE                = 0x00000008,
  SPINNING                    = 0x00000010,
  DISPLAY_MODELVIEW_MATRIX    = 0x00000020,
  DISPLAY_FRAME_RATE          = 0x00000040,
  DISPLAY_POLYGON_COUNT       = 0x00000080,
  DISPLAY_LINE_SEGMENT_COUNT  = 0x00000100,
  DISPLAY_VERTEX_COUNT        = 0x00000200,
  DISPLAY_NUM_STATE_CHANGES   = 0x00000400,
};

enum // Dragger state.
{
  DRAGGER_START  = 0,
  DRAGGER_MOVE   = 1,
  DRAGGER_FINISH = 2
};

namespace CadKit
{
  enum KeyState { KEY_UP, KEY_DOWN };
#ifdef __GNUC__
  typedef std::vector<KeyState> KeyStates;
#else
  typedef std::map<unsigned long,KeyState> KeyStates;
#endif
  KeyStates _keyStates;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgViewer::SgViewer() : SlRefBase ( 0 ), 
  _scene ( NULL ), 
  _display ( new SgGroup ),
  _modelviewText ( new SgText ),
  _renderer ( NULL ),
  _camera ( NULL ),
  _internalFlags ( FIRST_TIME_IN_MOUSEMOVED | VIEWING_MODE ),
  _rotationMethod ( TRACKBALL ),
  _windowMouse ( 0, 0 ),
  _lastWindowMouse ( 0, 0 ),
  _dragger ( NULL ),
  _rotateMouseButtons ( MOUSE_LEFT ),
  _translateMouseButtons ( MOUSE_RIGHT ),
  _scaleMouseButtons ( MOUSE_LEFT | MOUSE_RIGHT ),
  _seekMouseButtons ( MOUSE_LEFT ),
  _viewingKey ( 118 ),        // FOX key code for 'v'
  _seekingKey ( 115 ),        // FOX key code for 's'
  _rotateKey ( 114 ),         // FOX key code for 'r'
  _translateKey ( 116 ),      // FOX key code for 't'
  _scaleKey ( 122 ),          // FOX key code for 'z'
  _displayMatrixKey ( 109 ),  // FOX key code for 'm'
  _callback ( NULL, NULL, NULL ),
  _spinTolerance ( 2 ), // 25
  _spinTimerInterval ( 1 ), // 10
  _dR ( SL_MATRIX_44_IDENTITY_F )
{
  SL_PRINT2 ( "SgViewer::SgViewer(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgViewer::~SgViewer()
{
  SL_PRINT2 ( "SgViewer::~SgViewer(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the message callback.
//
///////////////////////////////////////////////////////////////////////////////

void SgViewer::setCallback ( Callback *callback, const void *data ) 
{
  SL_ASSERT ( this );
  _callback.setValue ( callback, data, NULL );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Change the background color.
//
///////////////////////////////////////////////////////////////////////////////

void SgViewer::setCamera ( SgCamera *camera ) 
{
  SL_ASSERT ( this );
  _camera = camera; // Automatically handles references.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Map mouse and control-key bit flags to SgViewer bit flags.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long SgViewer::mapMouseEventBitFlags ( const unsigned long &wmSpecificBits ) 
{
  // Initialize.
  unsigned long viewerBits = 0;

  // Determine what buttons are down.
  #ifdef _WIN32

    if ( wmSpecificBits & MK_LBUTTON )
      viewerBits |= SgViewer::MOUSE_LEFT;

    if ( wmSpecificBits & MK_MBUTTON ) 
      viewerBits |= SgViewer::MOUSE_MIDDLE;

    if ( wmSpecificBits & MK_RBUTTON ) 
      viewerBits |= SgViewer::MOUSE_RIGHT;

    if ( wmSpecificBits & MK_CONTROL ) 
      viewerBits |= SgViewer::KEY_CONTROL;

    if ( wmSpecificBits & MK_SHIFT ) 
      viewerBits |= SgViewer::KEY_SHIFT;

  #else
    // TODO
    SL_ASSERT ( 0 );
  #endif

  // Return the flags.
  return viewerBits;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call this function when the mouse moves.
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::mouseMoved ( const long &x, const long &y, const unsigned long &buttonsDown )
{
  SL_ASSERT ( this );

  // The first time in here we have to do this because _windowMouse 
  // is initialized to zero and point may not be, so you'll get a
  // rotation because sometimes the left mouse click is still active,
  // like if you double clicked on a file browser to open a file, and 
  // this view pops up underneith it (which is when I noticed it).
  if ( _internalFlags & FIRST_TIME_IN_MOUSEMOVED )
  {
    _windowMouse.setValue ( x, y );
    _internalFlags = CadKit::removeBits ( _internalFlags, (unsigned long) FIRST_TIME_IN_MOUSEMOVED );
  }

  // Save the new mouse point.
  _lastWindowMouse = _windowMouse;
  _windowMouse.setValue ( x, y );

  // If the current and last mouse position are the same then return.
  if ( _windowMouse == _lastWindowMouse ) 
    return true;

  // The mouse is really moving if we get to here.
  _internalFlags = CadKit::addBits ( _internalFlags, (unsigned long) MOUSE_IS_MOVING );

  // Initialize.
  bool results = true;

  // If we have a renderer and a scene...
  if ( _renderer.isValid() && _scene.isValid() )
  {
    // If we have a camera and we are in viewing mode...
    if ( _camera.isValid() && this->isViewing() ) 
    {
      // Process the viewing.
      results = this->_processViewing ( buttonsDown );
    }

    // Otherwise, if we have a current dragger...
    else if ( _dragger ) 
    {
      // Process the dragging.
      results = this->_processDragging ( _windowMouse, DRAGGER_MOVE );
    }
  }

  // The mouse is no longer moving.
  _internalFlags = CadKit::removeBits ( _internalFlags, (unsigned long) MOUSE_IS_MOVING );

  // Return the results.
  return results;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call this function when the mouse button is pressed.
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::mouseButtonDown ( const long &x, const long &y, const unsigned long &whichButtonDown, const unsigned long &buttonsDown )
{
  SL_ASSERT ( this );

  // If we are spinning.
  if ( this->isSpinning() )
  {
    // Send a message to stop any spin.
    if ( false == this->sendMessage ( SG_VIEWER_SPIN_FINISH ) )
      return false;

    // Set the flag.
    _internalFlags = CadKit::removeBits ( _internalFlags, (unsigned long) SPINNING );
  }

  // If we are seeking and the proper mouse buttons are down, and we 
  // have all the necessary pointers...
  if ( this->isSeeking() && buttonsDown == _seekMouseButtons && _renderer.isValid() && _camera.isValid() && _scene.isValid() )
  {
    // Process the seek request.
    if ( false == this->_processSeek ( x, y ) ) 
      return false;

    // Now make sure seeking is off.
    this->setSeeking ( false );

    // Set the message id. If we're not viewing then we're picking.
    SlUint64 id = ( this->isViewing() ) ? SG_VIEWER_VIEWING_MODE : SG_VIEWER_PICKING_MODE;

    // Send the message.
    if ( false == this->sendMessage ( id ) )
      return false;
  }

  // We did good.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call this function when the mouse button is released.
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::mouseButtonUp ( const long &x, const long &y, const unsigned long &whichButtonUp, const unsigned long &buttonsDown )
{
  SL_ASSERT ( this );

  // If we are viewing and the mouse buttons released indicate rotation...
  if ( this->isViewing() && whichButtonUp == _rotateMouseButtons )
  {
    // Get the absolute difference between the current point and the last one.
    SlVec2i diff ( _windowMouse[0] - _lastWindowMouse[0], _windowMouse[1] - _lastWindowMouse[1] );
    diff.absolute();

    // If the difference in x or y is greater than the tolerance...
    if ( diff[0] > _spinTolerance || diff[1] > _spinTolerance )
    {
      // Set the flag.
      _internalFlags = CadKit::addBits ( _internalFlags, (unsigned long) SPINNING );

      // Send a message to start a spin.
      if ( false == this->sendMessage ( SG_VIEWER_SPIN_START ) )
        return false;
    }
  }

  // If there are no more mouse buttons down...
  if ( buttonsDown == 0 )
  {
    // Send a message indicating what mode we are in.
    if ( false == this->sendMessage ( this->_getCurrentModeMessageId() ) )
      return false;
  }

  // We did good.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the message id of the current mode.
//
///////////////////////////////////////////////////////////////////////////////

SlUint64 SgViewer::_getCurrentModeMessageId() const
{
  SL_ASSERT ( this );

  // See what mode we're in.
  if      ( this->isSeeking() ) return SG_VIEWER_SEEKING_MODE;
  else if ( this->isViewing() ) return SG_VIEWER_VIEWING_MODE;

  // Should be true, unless we added another mode.
  SL_ASSERT ( this->isPicking() );
  return SG_VIEWER_PICKING_MODE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send the message.
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::sendMessage ( const SlUint64 &id ) const
{
  SL_ASSERT ( this );

  // Declare and set the message.
  Message message;
  message.SlMessage<SG_VMTA>::setValue ( 
    id,
    "Sent from SgViewer::sendMessage()", 
    _callback.getClientData(), 
    const_cast<SgViewer *> ( this ) );

  // Call the other one.
  return this->sendMessage ( message );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send the message (if there is a callback).
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::sendMessage ( Message &message ) const
{
  SL_ASSERT ( this );

  // If there is a callback...
  if ( _callback.getFunction() )
  {
    // Then call it.
    return (_callback.getFunction()) ( message );
  }

  // Otherwise, we did our job.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we displaying the modelview matrix?
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::isDisplayingModelviewMatrix() const
{
  SL_ASSERT ( this );
  return CadKit::hasBits ( _internalFlags, (unsigned long) DISPLAY_MODELVIEW_MATRIX );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we in picking mode?
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::isPicking() const
{
  SL_ASSERT ( this );

  // To be in picking mode simply means that we are not in viewing mode.
  return ( false == this->isViewing() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we in viewing mode?
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::isViewing() const
{
  SL_ASSERT ( this );
  return CadKit::hasBits ( _internalFlags, (unsigned long) VIEWING_MODE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we spinning?
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::isSpinning() const
{
  SL_ASSERT ( this );
  return CadKit::hasBits ( _internalFlags, (unsigned long) SPINNING );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we in seeking mode?
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::isSeeking() const
{
  SL_ASSERT ( this );
  return CadKit::hasBits ( _internalFlags, (unsigned long) SEEKING_MODE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the display of the modelview matrix.
//
///////////////////////////////////////////////////////////////////////////////

void SgViewer::setDisplayOfModelviewMatrix ( const bool &state )
{
  SL_ASSERT ( this );

  // If we are supposed to draw the modelview matrices values...
  if ( state )
  {
    // Add the flag.
    _internalFlags = CadKit::addBits ( _internalFlags, (unsigned long) DISPLAY_MODELVIEW_MATRIX );

    // Should be true.
    SL_ASSERT ( false == _display->removeChild ( _modelviewText ) );

    // Add the modelview text.
    _display->addChild ( _modelviewText );
  }

  // Otherwise...
  else
  {
    // Remove the flag.
    _internalFlags = CadKit::removeBits ( _internalFlags, (unsigned long) DISPLAY_MODELVIEW_MATRIX );

    // Remove the child.
    _display->removeChild ( _modelviewText );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the picking mode.
//
///////////////////////////////////////////////////////////////////////////////

void SgViewer::setPicking ( const bool &state )
{
  SL_ASSERT ( this );

  // To be in picking mode simply means that we are not in viewing mode.
  this->setViewing ( !state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewing mode.
//
///////////////////////////////////////////////////////////////////////////////

void SgViewer::setViewing ( const bool &state )
{
  SL_ASSERT ( this );
  _internalFlags = ( state ) ? 
    CadKit::addBits ( _internalFlags, (unsigned long) VIEWING_MODE ) : 
    CadKit::removeBits ( _internalFlags, (unsigned long) VIEWING_MODE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the seeking mode.
//
///////////////////////////////////////////////////////////////////////////////

void SgViewer::setSeeking ( const bool &state )
{
  SL_ASSERT ( this );
  _internalFlags = ( state ) ? 
    CadKit::addBits ( _internalFlags, (unsigned long) SEEKING_MODE ) : 
  CadKit::removeBits ( _internalFlags, (unsigned long) SEEKING_MODE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the mouse button(s) that rotate.
//
///////////////////////////////////////////////////////////////////////////////

void SgViewer::setMouseButtonsForRotating ( const unsigned long &rotateMouseButtons )
{
  SL_ASSERT ( this );
  _rotateMouseButtons = rotateMouseButtons;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the mouse button(s) that translate.
//
///////////////////////////////////////////////////////////////////////////////

void SgViewer::setMouseButtonsForTranslating ( const unsigned long &translateMouseButtons )
{
  SL_ASSERT ( this );
  _translateMouseButtons = translateMouseButtons;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the mouse button(s) that scale.
//
///////////////////////////////////////////////////////////////////////////////

void SgViewer::setMouseButtonsForScaling ( const unsigned long &scaleMouseButtons )
{
  SL_ASSERT ( this );
  _scaleMouseButtons = scaleMouseButtons;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the seek request.
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::_processSeek ( const long &x, const long &y )
{
  SL_ASSERT ( this );
  SL_ASSERT ( _camera.isValid() );

  // If there is no scene then return.
  if ( _scene.isNull() ) 
    return true;

  // Get the line (in absolute global space).
  SlLine3f line;
  if ( false == this->getRay ( x, y, line ) )
    return false;
  //if ( false == this->getRay ( x, y, line ) )return false;

  // Get the intersected node under the ray.
  SlVec3f global, local;
  SgNode *node = this->getIntersection ( line, global, local );

  // Did we miss?
  if ( NULL == node )
  {
    // Send a message.
    if ( false == this->sendMessage ( SG_VIEWER_SEEK_MISS ) )
      return false;

    // We did our job.
    return true;
  }

  // Set the camera's center with the intersection point (in global coordinates).
  if ( false == _camera->setCenter ( global, false ) ) 
    return false;

  // Send a message.
  if ( false == this->sendMessage ( SG_VIEWER_SEEK_HIT ) )
    return false;

  // Render the scene now.
  if ( false == this->render() ) 
  {
    SL_ASSERT ( 0 ); 
    return false;
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the viewing.
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::_processViewing ( const unsigned long &buttonsDown )
{
  SL_ASSERT ( this );
  SL_ASSERT ( _camera.isValid() );
  SL_ASSERT ( _renderer.isValid() );
  SL_TRACE2 ( "In SgViewer::_processViewing(), buttonsDown = %d\n", buttonsDown );

  // Get the viewport.
  SlViewporti viewport;
  if ( false == _renderer->getViewport ( viewport ) )
  {
    SL_ASSERT ( 0 );
    return false;
  }

  // If we are supposed to scale...
  if ( buttonsDown == _scaleMouseButtons )
  {
    // Send a message.
    if ( false == this->sendMessage ( SG_VIEWER_SCALING ) )
      return false;
    
    // Set the camera's scale.
    float scaleFactor = 1.0f + ( ( (float) (_windowMouse[1] - _lastWindowMouse[1]) ) / (float) viewport.getHeight() );
    _camera->scale ( scaleFactor );

    // Render now.
    if ( false == this->render() )
    {
      SL_ASSERT ( 0 );
      return false;
    }
  }

  // If we are supposed to rotate...
  else if ( buttonsDown == _rotateMouseButtons )
  {
    // Send a message.
    if ( false == this->sendMessage ( SG_VIEWER_ROTATING ) )
      return false;

    if ( TRACKBALL == _rotationMethod )
    {
      // Calculate the current and last mouse position in the range [-1,1].
      // It will exceed this range when the mouse is outside of the window, 
      // but that is ok, the math can handle it.
      float magicNumber = 2.0f;
      const long &width = viewport.getWidth();
      const long &height = viewport.getHeight();
      SlVec2f pt1 ( ( magicNumber * _lastWindowMouse[0] - width )  / width,
                    ( height - magicNumber * _lastWindowMouse[1] ) / height );
      SlVec2f pt2 ( ( magicNumber * _windowMouse[0] - width )      / width,
                    ( height - magicNumber * _windowMouse[1] )     / height );

      // Get the rotation matrix.
      SlTrackballf trackball;
      trackball.getMatrix ( pt1, pt2, _dR );

      // Rotate the camera by this matrix.
      _camera->rotate ( _dR );
    }

    else if ( SCREEN_AXES == _rotationMethod )
    {
      SL_ASSERT ( 0 ); // TODO.
    }

    else if ( BODY_AXES == _rotationMethod )
    {
      SL_ASSERT ( 0 ); // TODO.
    }

    else 
    {
      // Should never happen.
      SL_ASSERT ( 0 );
    }

    // Render now.
    if ( false == this->render() ) 
    {
      SL_ASSERT ( 0 );
      return false;
    }
  }

  // If we are supposed to translate...
  else if ( buttonsDown == _translateMouseButtons )
  {
    // Send a message.
    if ( false == this->sendMessage ( SG_VIEWER_TRANSLATING ) )
      return false;

    // Reset this if we have a perspective camera.
    float transFactor = 0.01f;

    if ( _camera->isOfType ( SgPerspectiveCamera::getClassType() ) )
    {
      SgPerspectiveCamera::Ptr camera = static_cast<SgPerspectiveCamera *> ( _camera.getValue() );
      float angle = camera->getFieldOfView ( SgPerspectiveCamera::FOV_IN_X ); // This works!
      //float angle = CadKit::fixAngleRad ( camera->getFieldOfView ( SgPerspectiveCamera::FOV_IN_X ) ); // This works!
      //float angle = camera->fieldOfView; // This works too!
      //float angle = SL_TO_RAD_D * camera->fieldOfView; // Why doesn't this work? It seems right!
      //transFactor = ( (float) ::sin ( angle ) * _camera->getAspectRatio() ) / ( (float) viewport.getWidth() * _camera->getScale() );
      transFactor = ( (float) ::cos ( angle ) * _camera->getAspectRatio() ) / ( (float) viewport.getWidth() * _camera->getScale() );
      transFactor = SL_ABS ( transFactor );
    }

    // Translate the camera.
    _camera->translate ( transFactor * float ( _windowMouse[0] - _lastWindowMouse[0] ),
                         transFactor * float ( _lastWindowMouse[1] - _windowMouse[1] ), 0.0f );

    // Render now.
    if ( false == this->render() )
    {
      SL_ASSERT ( 0 );
      return false;
    }
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the dragging.
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::_processDragging ( const SlVec2i &mousePoint, const unsigned long &dragState )
{
  SL_ASSERT ( this );
  SL_ASSERT ( dragState == DRAGGER_START || dragState == DRAGGER_MOVE || dragState == DRAGGER_FINISH );
  return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  This gets called when the user resized the window.
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::resize ( const long &width, const long &height ) 
{
  SL_ASSERT ( this );

  // If we have a renderer...
  if ( _renderer.isValid() )
  {
    // Set the viewport.
    if ( false == _renderer->setViewport ( SlViewporti ( 0, 0, width, height ) ) )
      return false;
  }

  // If we have a camera...
  if ( _camera.isValid() )
  {
    // Set the aspect ratio.
    _camera->setAspectRatio ( width, height );
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  This will draw everything connected to the scene.
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::render()
{
  SL_ASSERT ( this );

  try
  {
    // If there's a renderer and a scene...
    if ( _renderer.isValid() && _scene.isValid() )
    {
      // Pre-render the scene.
      if ( false == _renderer->preRender ( *_scene ) )
        return false;

      // If there's a camera...
      if ( _camera.isValid() )
      {
        // Render the camera.
        if ( false == _renderer->render ( *_camera ) ) 
          return false;
      }

      // Render the scene.
      if ( false == _renderer->render ( *_scene ) ) 
        return false;

      // Render our displays.
      if ( false == _renderer->render ( *_display ) )
        return false;

      // Post-render the scene.
      return _renderer->postRender ( *_scene );
    }

    // If not...
    return false;
  }

  catch ( ... )
  {
    // Should never get here.
    SL_ASSERT ( 0 );
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move the camera so that the whole scene is visible.
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::viewAll()
{
  SL_ASSERT ( this );

  // If there's a scene and a camera...
  if ( _scene.isValid() && _camera.isValid() )
  {
    // Let the camera do the work.
    return ( _camera->viewAll ( *_scene ) ) ? true : false;
  }

  // If not...
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call this when a key is pressed when the mouse is over the window.
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::keyDown ( const unsigned long &theChar )
{
  SL_ASSERT ( this );

#ifdef __GNUC__
  // Make sure your usage of std::vector is ok.
  // See if you can figure out how to use std::map.
  // This is just a heads-up for the first time I run this on linux.
  SL_ASSERT ( 0 ); 
#endif

  // Set the state.
  CadKit::_keyStates[theChar] = CadKit::KEY_DOWN;

  // If the given char is for viewing...
  if ( theChar == _viewingKey )
  {
    // Make sure seeking is off.
    this->setSeeking ( false );

    // Toggle the viewing mode.
    this->setViewing ( !this->isViewing() );

    // Set the message id. If we're not viewing then we're picking.
    SlUint64 id = ( this->isViewing() ) ? SG_VIEWER_VIEWING_MODE : SG_VIEWER_PICKING_MODE;

    // Send the message.
    if ( false == this->sendMessage ( id ) )
      return false;
  }

  // If the given char is for seeking...
  else if ( theChar == _seekingKey )
  {
    // Toggle the seeking mode.
    this->setSeeking ( !this->isSeeking() );

    // Set the message id. If we're not seeking then we see if we're viewing or picking.
    SlUint64 id = ( this->isSeeking() ) ? SG_VIEWER_SEEKING_MODE : ( ( this->isViewing() ) ? SG_VIEWER_VIEWING_MODE : SG_VIEWER_PICKING_MODE );

    // Send the message.
    if ( false == this->sendMessage ( id ) )
      return false;
  }

  // Display the current transformation matrix.
  else if ( theChar == _displayMatrixKey )
  {
    // Toggle the display of the current modelview matrix.
    this->setDisplayOfModelviewMatrix ( !this->isDisplayingModelviewMatrix() );
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call this when a key is released when the mouse is over the window.
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::keyUp ( const unsigned long &theChar ) 
{
  SL_ASSERT ( this );

  // Set the state.
  CadKit::_keyStates[theChar] = CadKit::KEY_UP;

  // All is good.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene graph and ref it.
//
///////////////////////////////////////////////////////////////////////////////

void SgViewer::setScene ( SgNode *scene ) 
{
  SL_ASSERT ( this );
  _scene = scene; // Automatically handles references.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the renderer and ref it.
//
///////////////////////////////////////////////////////////////////////////////

void SgViewer::setRenderer ( SgRenderer *renderer ) 
{
  SL_ASSERT ( this );
  _renderer = renderer; // Automatically handles references.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell the viewer that is should spin the geometry. Typically this 
//  function is called from within a timer callback.
//
///////////////////////////////////////////////////////////////////////////////

void SgViewer::spinNotify()
{
  SL_ASSERT ( this );

  // If there is a camera then rotate it.
  if ( _camera.isValid() ) 
    _camera->rotate ( _dR );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the ray into the screen from the window point.
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::getRay ( long x, long y, SlLine3f &line ) const
{
  SL_ASSERT ( this );
  SL_ASSERT ( _renderer.isValid() );
  SL_ASSERT ( _camera.isValid() );

  // Get the viewport.
  SlViewporti viewport;
  if ( false == _renderer->getViewport ( viewport ) ) 
    return false;

  // Move the origin to the middle of the window and flip the y-direction.
  x = x - ( viewport.getWidth() / 2 );
  y = ( viewport.getHeight() / 2 ) - y;

  // If the camera is perspective...
  if ( _camera->isOfType ( SgPerspectiveCamera::getClassType() ) )
  {
    // Get a pointer to the perspective camera.
    SgPerspectiveCamera::Ptr camera = static_cast<SgPerspectiveCamera *> ( _camera.getValue() );

    // Calculate the z-coordinate using the fact that the x and y coordinates
    // are on a plane parallel to the clipping planes.
    float angle = (float) camera->getFieldOfView() * 0.5f * SL_DEG_TO_RAD_F;
    float z = float ( (float) ( viewport.getHeight() / 2 ) / tanf ( angle ) );

    // This is the point on the screen (in global space).
    SlVec3f pointOnWindow ( (float) x, (float) y, -z );

    // The vrp is on the near clipping plane.
    SlVec3f vrp;
    vrp[0] = ( (float) _camera->getZNear() * pointOnWindow[0] ) / z;
    vrp[1] = ( (float) _camera->getZNear() * pointOnWindow[1] ) / z;
    vrp[2] = - (float) _camera->getZNear();

    // This is a vector through the global origin, normalize it.
    pointOnWindow.normalize();

    // Make the line (in absolute global space).
    line.setValue ( vrp, pointOnWindow );

    SL_TRACE7 ( "In SgViewer::getRay(),  line.getPoint() = %f,%f,%f, line.getDirection() = %f,%f,%f\n", line.getPoint()[0], line.getPoint()[1], line.getPoint()[2], line.getDirection()[0], line.getDirection()[1], line.getDirection()[2] );

    // It worked.
    return true;
  }

  // If the camera is orthographic...
  else if ( _camera->isOfType ( SgOrthographicCamera::getClassType() ) )
  {
    SL_ASSERT ( 0 ); // TODO.
  }

  // If we get to here then something went wrong.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the ray into the screen from the window point.
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::getRay2 ( const long &winX, const long &winY, SlLine3f &line ) const
{
  SL_ASSERT ( this );
  SL_PRINT3 ( "In SgViewer::getRay2(), winX = %4d, winY = %4d\n", winX, winY );
  SL_ASSERT ( _renderer.isValid() );
  SL_ASSERT ( _camera.isValid() );

  // Get the viewport.
  SlViewporti vp;
  if ( false == _renderer->getViewport ( vp ) ) 
    return false;

  // Get the matrices.
  //const SlMatrix44f &M = _camera->getModelviewMatrix();
  const SlMatrix44f M ( SL_MATRIX_44_IDENTITY_F );
  SlMatrix44f P;
  _camera->getProjectionMatrix ( P );

  // Get the 3D point on the near plane.
  SlVec3f nearPoint;
  if ( false == this->unProject ( winX, winY, 0.0f, M, P, vp, nearPoint[0], nearPoint[1], nearPoint[2] ) )
    return false;

  // Get the 3D point on the far plane.
  SlVec3f farPoint;
  if ( false == this->unProject ( winX, winY, 1.0f, M, P, vp, farPoint[0], farPoint[1], farPoint[2] ) )
    return false;

  // Make the line through them.
  line.setValue ( nearPoint, farPoint - nearPoint );
  line.getDirection().normalize();

  SL_TRACE7 ( "In SgViewer::getRay2(), line.getPoint() = %f,%f,%f, line.getDirection() = %f,%f,%f\n", line.getPoint()[0], line.getPoint()[1], line.getPoint()[2], line.getDirection()[0], line.getDirection()[1], line.getDirection()[2] );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the node and the intersection point under the ray.
//
///////////////////////////////////////////////////////////////////////////////

SgNode *SgViewer::getIntersection ( const SlLine3f &line, SlVec3f &global, SlVec3f &local )
{
  SL_ASSERT ( this );
  SL_ASSERT ( _scene.isValid() );

  // Declare the ray-intersection visitor, and set its ray.
  SgGetRayIntersection visitor;
  visitor.setRay ( line );

  // Give the visitor to the camera (if there is one). This will modify the 
  // visitor's internal matrices to account for the camera position.
  if ( _camera.isValid() && false == _camera->accept ( visitor ) ) 
    return NULL;

  // Now give the visitor to the scene.
  if ( _scene.isValid() && false == _scene->accept ( visitor ) ) 
    return NULL;

  // Get the node we intersected with.
  SgNode *node = visitor.getIntersectedNode();

  // If there is a node...
  if ( node )
  {
    // Set the intersection point (in global and local coordinates).
    global = visitor.getGlobalIntersectionPoint();
    local = visitor.getLocalIntersectionPoint();
  }

  // Return the node (which may be NULL).
  return node;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the window coordinate to object coordinates. If winZ = 0 then 
//  the object coordinate will be on the near plane. If winZ = 1 then 
//  the object coordinate will be on the far plane.
//
//  From: http://www.rs6000.ibm.com/idd500/usr/share/man/info/en_US/a_doc_lib/libs/openglrf/gluUnProject.htm
//  How to use: http://www.opengl.org.ru/docs/pg/0309.html
//
//                  |  2(winX - vp.x)       |
//                  |  --------------- - 1  |
//                  |       vp.width        |
//                  |                       |
//  | wx |          |  2(winY - vp.y)       |
//  | wy | = INV(PM)|  --------------- - 1  |
//  | wz |          |       vp.height       |
//  |  w |          |                       |
//                  |      2(winZ) - 1      |
//                  |                       |
//                  |           1           |
//
///////////////////////////////////////////////////////////////////////////////

bool SgViewer::unProject ( const long &winX, 
                           const long &winY, 
                           const float &winZ, 
                           const SlMatrix44f &M, 
                           const SlMatrix44f &P, 
                           const SlViewporti &vp, 
                           float &x, 
                           float &y, 
                           float &z )
{
  // Get the inverse of PM.
  SlMatrix44f invPM = P * M;
  if ( false == invPM.invert() ) 
    return false;

  // Adjust the window y-value (puts the origin in the lower left).
  // Got this from http://www.opengl.org.ru/docs/pg/0309.html.
  // Not sure why the -1 is there.
  long wY = vp.getHeight() - winY - 1;

  // Form the vector on the right side of the equation.
  SlVec4f b ( ( ( 2.0f * ( (float) ( winX - vp.getX() ) ) ) / ( (float) vp.getWidth()  ) ) - 1.0f,
              ( ( 2.0f * ( (float) (   wY - vp.getY() ) ) ) / ( (float) vp.getHeight() ) ) - 1.0f,
              2.0f * ( (float) winZ ) - 1.0f,
              1.0f );

  // Calculate the answer.
  SlVec4f v = invPM * b;

  // Check the weight.
  if ( v[3] == 0.0f ) 
    return false;

  // Set the answers.
  float invW = 1.0f / v[3];
  x = v[0] * invW;
  y = v[1] * invW;
  z = v[2] * invW;

  // It worked.
  return true;
}
