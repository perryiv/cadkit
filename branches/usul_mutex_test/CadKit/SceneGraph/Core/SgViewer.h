
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgViewer.h: An interactive view class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_VIEWER_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_VIEWER_H_


#include "SgMessage.h"
#include "SgRenderer.h"
#include "SgCamera.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlRefBase.h"
# include "Standard/SlVec2.h"
# include "Standard/SlVec3.h"
# include "Standard/SlMatrix44.h"
# include "Standard/SlLine3.h"
# include "Standard/SlCallback.h"
# include <string>
#endif


namespace CadKit
{
class SgNode;
class SgGroup;

class SG_API SgViewer : public SlRefBase
{
public:

  // Declare SgViewer::Ptr up here.
  SL_DECLARE_REFERENCE_POINTER ( SgViewer );

  #define SG_VMTA SlUint64, std::string, const void *, SlRefPtr<SgViewer>
  typedef SgMessage < SG_VMTA > Message;
  typedef bool Callback ( Message &message );

  // Mouse/control buttons.
  enum
  {
    MOUSE_LEFT   = 0x00000001,
    MOUSE_MIDDLE = 0x00000002,
    MOUSE_RIGHT  = 0x00000004,
    MOUSE_WHEEL  = 0x00000008,
    KEY_CONTROL  = 0x00000010,
    KEY_SHIFT    = 0x00000020
  };

  // The rotation method.
  enum RotationMethod
  {
    TRACKBALL,    // The default.
    SCREEN_AXES,
    BODY_AXES
  };

  SgViewer();

  // Get the camera.
  const SgCamera *       getCamera() const { return _camera.getValue(); }
  SgCamera *             getCamera()       { return _camera.getValue(); }

  // Get the intersected node under the given ray. Returns NULL when no intersection is found.
  SgNode *               getIntersection ( const SlLine3f &ray, SlVec3f &global, SlVec3f &local );

  // Get the ray into the window under the mouse point.
  bool                   getRay ( long winX, long winY, SlLine3f &line ) const;
  bool                   getRay2 ( const long &winX, const long &winY, SlLine3f &line ) const;

  // Get the renderer.
  const SgRenderer *     getRenderer() const { return _renderer.getValue(); }
  SgRenderer *           getRenderer()       { return _renderer.getValue(); }

  // Get the scene graph.
  const SgNode *         getScene() const { return _scene.getValue(); }
  SgNode *               getScene()       { return _scene.getValue(); }

  // Get the spin timer interval (in milliseconds).
  const long &           getSpinTimerInterval() const { return _spinTimerInterval; }

  // Are we in these modes?
  bool                   isDisplayingModelviewMatrix() const;
  bool                   isPicking() const;
  bool                   isSeeking() const;
  bool                   isSpinning() const;
  bool                   isViewing() const;

  // Call this when a key is pressed/released.
  bool                   keyDown ( const unsigned long &theChar );
  bool                   keyUp   ( const unsigned long &theChar );

  // Map the window-manager-specific mouse event bit flags to this class's 
  // equivalent flags. This functions is merely a convenience for the client.
  static unsigned long   mapMouseEventBitFlags ( const unsigned long &wmSpecificBits );

  // Call this function when the mouse moves.
  bool                   mouseMoved ( const long &x, const long &y, const unsigned long &buttonsDown );

  // Call this function when the mouse button is pressed/released.
  bool                   mouseButtonDown ( const long &x, const long &y, const unsigned long &whichButtonDown, const unsigned long &buttonsDown );
  bool                   mouseButtonUp ( const long &x, const long &y, const unsigned long &whichButtonUp, const unsigned long &buttonsDown );

  // Tell the window to render now.
  virtual bool           render();

  // Resize the window.
  bool                   resize ( const long &width, const long &height );

  // Send a message.
  bool                   sendMessage ( Message &message ) const;
  bool                   sendMessage ( const SlUint64 &id ) const;

  // Set the message callback.
  void                   setCallback ( Callback *callback, const void *data );

  // Set the camera.
  void                   setCamera ( SgCamera *camera );

  // Set the mouse button(s) that rotate, translate, and scale. Set to 0 to turn off.
  void                   setMouseButtonsForRotating ( const unsigned long &rotateMouseButtons );
  void                   setMouseButtonsForScaling ( const unsigned long &scaleMouseButtons );
  void                   setMouseButtonsForTranslating ( const unsigned long &translateMouseButtons );

  // Set the renderer (this will ref renderer). Pass NULL to unset the 
  // current renderer (this will also unref the current renderer).
  void                   setRenderer ( SgRenderer *renderer );

  // Set the scene graph (this will ref scene). Pass NULL to unset the 
  // current scene (this will also unref the current scene).
  void                   setScene ( SgNode *scene );

  // Set the mode.
  void                   setDisplayOfModelviewMatrix ( const bool &state );
  void                   setPicking ( const bool &state );
  void                   setSeeking ( const bool &state );
  void                   setViewing ( const bool &state );

  // Tell the viewer that is should spin the geometry. Typically this 
  // function is called from within a timer callback.
  void                   spinNotify();

  // Convert the window coordinate to object coordinates. If winZ = 0 then the object coordinate 
  // will be on the near plane. If winZ = 1 then the object coordinate will be on the far plane.
  static bool            unProject ( const long &winX, 
                                     const long &winY, 
                                     const float &winZ, 
                                     const SlMatrix44f &M, 
                                     const SlMatrix44f &P, 
                                     const SlViewporti &vp, 
                                     float &objX, 
                                     float &objY, 
                                     float &objZ );
  // Move the camera so that the whole scene is visible.
  virtual bool           viewAll();

protected:

  SlRefPtr<SgNode> _scene;
  SlRefPtr<SgGroup> _display;
  SlRefPtr<SgText> _modelviewText;
  SlRefPtr<SgRenderer> _renderer;
  SlRefPtr<SgCamera> _camera;
  unsigned long _internalFlags;
  unsigned long _displayFlags;
  RotationMethod _rotationMethod;
  SlVec2i _windowMouse;
  SlVec2i _lastWindowMouse;
  void *_dragger;
  unsigned long _rotateMouseButtons;
  unsigned long _translateMouseButtons;
  unsigned long _scaleMouseButtons;
  unsigned long _seekMouseButtons;
  unsigned long _viewingKey;
  unsigned long _seekingKey;
  unsigned long _rotateKey;
  unsigned long _translateKey;
  unsigned long _scaleKey;
  unsigned long _displayMatrixKey;
  SlCallback<Callback *, const void *, const void *> _callback;
  long _spinTolerance;
  long _spinTimerInterval;
  SlMatrix44f _dR;

  virtual ~SgViewer();

  SlUint64               _getCurrentModeMessageId() const;

  virtual bool           _processDragging ( const SlVec2i &mousePoint, const unsigned long &dragState );
  virtual bool           _processSeek ( const long &x, const long &y );
  virtual bool           _processViewing ( const unsigned long &buttonsDown );

  SL_DECLARE_DYNAMIC_CLASS ( SgViewer, 1032912045 );
};
};

#endif
