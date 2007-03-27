
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  An interactive viewer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_VIEWER_H_
#define _GENERIC_SCENE_GRAPH_CORE_VIEWER_H_

#include "GSG/Core/Node.h"
#include "GSG/Core/Renderer.h"
#include "GSG/Core/Camera.h"
#include "GSG/Core/Keys.h"


namespace GSG {

class Path;


class GSG_CORE_EXPORT Viewer : public Referenced
{
public:

  GSG_DECLARE_REFERENCED ( Viewer );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Viewer, Referenced );

  // Viewer messages.
  enum Message
  {
    SPIN_START,
    SPIN_MOVE,
    SPIN_FINISH,
    ROTATING,
    TRANSLATING,
    SCALING,
    VIEWING_MODE,
    PICKING_MODE,
    SEEKING_MODE,
    SEEK_MISS,
    SEEK_HIT,
  };

  // Mouse/control buttons.
  enum
  {
    MOUSE_LEFT   = 0x00000001,
    MOUSE_MIDDLE = 0x00000002,
    MOUSE_RIGHT  = 0x00000004,
    MOUSE_WHEEL  = 0x00000008
  };

  // Callback class.
  struct GSG_CORE_EXPORT Callback : public Referenced
  {
    GSG_DECLARE_LOCAL_TYPEDEFS ( Callback, Referenced );
    virtual void operator () ( Message id, Path *, Viewer & ) = 0;
  protected:
    explicit Callback() : Referenced(){}
    Callback ( const Callback &cb ) : Referenced ( cb ){}
    virtual ~Callback();
  };

  // Constructors.
  explicit Viewer();
  Viewer ( const Viewer &v );

  // Get/set the message callback.
  const Callback *        callback() const;
  Callback *              callback();
  void                    callback ( Callback *c );

  // Get/set the camera.
  const Camera *          camera() const;
  Camera *                camera();
  void                    camera ( Camera *c );

  // Get/set the renderer.
  const Renderer *        renderer() const;
  Renderer *              renderer();
  void                    renderer ( Renderer *s );

  // Get/set the scene.
  const Node *            scene() const;
  Node *                  scene();
  void                    scene ( Node *n );

  // Call this when a key is pressed/released.
  void                    keyDown ( UnsignedInteger c );
  void                    keyUp   ( UnsignedInteger c );

  // Call this function when the mouse moves.
  void                    mouseMoved ( SignedInteger x, SignedInteger y, 
                                       UnsignedInteger buttonsDown );

  // Call this function when the mouse button is pressed/released.
  void                    mouseButtonDown ( SignedInteger x, SignedInteger y, 
                                            UnsignedInteger whichButtonDown, 
                                            UnsignedInteger allButtonsDown );
  void                    mouseButtonUp   ( SignedInteger x, SignedInteger y, 
                                            UnsignedInteger whichButtonUp,
                                            UnsignedInteger buttonsDown );

  // Tell the window to render now.
  void                    render();

  // Resize the window.
  void                    resize ( UnsignedInteger width, UnsignedInteger height );

  // Tell the viewer that is should spin the geometry. Typically this 
  // function is called from within a timer callback.
  void                    spinNotify();

  // Move the camera so that the whole scene is visible.
  void                    viewAll();

protected:

  UnsignedInteger _flags;
  Pointer<Node> _scene;
  Pointer<Renderer> _renderer;
  Pointer<Camera> _camera;
  Pointer<Keys> _keys;
  MousePoint _windowMouse;
  MousePoint _lastMouse;
  Pointer<Callback> _callback;
  Matrix _dR;

  virtual ~Viewer();
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_VIEWER_H_
