
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenGL canvas.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_FOX_BINDING_CANVAS_H_
#define _OPEN_SCENE_GRAPH_FOX_BINDING_CANVAS_H_

#include "Helios/Fox/Views/Export.h"
#include "Helios/Fox/Views/FoxCanvas.h"

#include "Usul/Interfaces/IDocument.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Variable.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Devices/IDevice.h"

#include "Usul/Interfaces/IFoxWindow.h"
#include "Usul/Interfaces/IBackground.h"
#include "Usul/Interfaces/IHandleMessage.h"
#include "Usul/Devices/IKeyListener.h"
#include "Usul/Devices/IMotionListener.h"
#include "Usul/Devices/IButtonListener.h"
#include "Usul/Devices/IMouseWheelListener.h"

#include "OsgTools/Render/Viewer.h"

#include <list>
#include <string>

namespace FX { class FXMenuPane; }


namespace OsgFox {
namespace Views {

class OSG_FOX_VIEWS_EXPORT Canvas : public FoxCanvas,
                                    public Usul::Interfaces::IFoxWindow,
                                    public Usul::Interfaces::IBackground,
                                    public Usul::Interfaces::IHandleMessage,
                                    public Usul::Devices::IKeyListener,
                                    public Usul::Devices::IMotionListener,
                                    public Usul::Devices::IButtonListener,
                                    public Usul::Devices::IMouseWheelListener
{
public:

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Useful typedefs.
  typedef FoxCanvas BaseClass;
  typedef Usul::Errors::Stack ErrorStack;
  typedef std::list<FX::FXMenuPane *> MenuPanes;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef OsgTools::Render::Viewer   Viewer;
  typedef Usul::Interfaces::IDocument Document;

  // Message ids.
  enum 
  {
    ID_RENDER = BaseClass::ID_LAST,
    ID_NEW,
    ID_CAMERA_FIT,
    ID_CAMERA_RESET,
    ID_CAMERA_FRONT,
    ID_CAMERA_BACK,
    ID_CAMERA_LEFT,
    ID_CAMERA_RIGHT,
    ID_CAMERA_TOP,
    ID_CAMERA_BOTTOM,
    ID_CAMERA_COPY,
    ID_CAMERA_PASTE,
    ID_POLYGONS_FILLED,
    ID_POLYGONS_WIRE,
    ID_POLYGONS_HIDDEN,
    ID_POLYGONS_POINTS,
    ID_POLYGONS_AS_IS,
    ID_SHADING_SMOOTH,
    ID_SHADING_FLAT,
    ID_SHADING_AS_IS,
    ID_EDIT_BACKGROUND,
    ID_BACKGROUND_CHANGED,
    ID_DEFAULT_BACKGROUND,
    ID_EDIT_COPY_CAMERA,
    ID_BOUNDING_BOX,
    ID_BOUNDING_SPHERE,
    ID_NUM_RENDER_PASSES,
    ID_SORT_BACK_TO_FRONT,
    ID_SEMI_TRANSPARENT,
    ID_FOV,
    ID_LAST
  };

  // Construction/destruction.
  Canvas ( Document *doc, FX::FXComposite *parent, FX::FXGLVisual *visual );
  virtual ~Canvas();

  // Add a chore.
  void                  addChoreRedraw();

  // Add a chore to reset the camera.
  void                  addChoreCameraFit();
  void                  addChoreCameraReset();

    // Edit the background color.
  void                  editBackground();

  // Edit the background color.
  void                  setBackground ( const osg::Vec4 &);

  // Get/Set the background color.
  void                  backgroundColor ( const osg::Vec4 &color );
  const osg::Vec4 &     backgroundColor() const;

  // Clear the devices.
  void                  clearDevices();

  // Create.
  virtual void          create();

  // Set the default background.
  void                  defaultBackground();

  // Message maps.
  long                  onResize            ( FX::FXObject *, FX::FXSelector, void * );
  long                  onPaint             ( FX::FXObject *, FX::FXSelector, void * );
  long                  onRender            ( FX::FXObject *, FX::FXSelector, void * );
  long                  onFocusIn           ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCameraReset       ( FX::FXObject *, FX::FXSelector, void * );
  long                  onDisplayListSet    ( FX::FXObject *, FX::FXSelector, void * );

  // Message maps for commands.
  long                  onCommandCameraFit         ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandCameraReset       ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandCameraCopy        ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandCameraPaste       ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandPolygonsFilled    ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandPolygonsWire      ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandPolygonsHidden    ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandPolygonsPoints    ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandPolygonsAsIs      ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandShadingSmooth     ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandShadingFlat       ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandShadingAsIs       ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandEditBackground    ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandDefaultBackground ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandBoundingBox       ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandBoundingSphere    ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandNumRenderPasses   ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandSortBackToFront   ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandSemiTransparent   ( FX::FXObject *, FX::FXSelector, void * );
  long                  onCommandFOV               ( FX::FXObject *, FX::FXSelector, void * );

  // Message maps for the color dialog.
  long                  onBackgroundChanged ( FXObject*, FX::FXSelector, void * );

  // Message maps for GUI update.
  long                  onUpdateCameraPaste        ( FX::FXObject *, FX::FXSelector, void * );
  long                  onUpdatePolygonsFilled     ( FX::FXObject *, FX::FXSelector, void * );
  long                  onUpdatePolygonsWire       ( FX::FXObject *, FX::FXSelector, void * );
  long                  onUpdatePolygonsHidden     ( FX::FXObject *, FX::FXSelector, void * );
  long                  onUpdatePolygonsPoints     ( FX::FXObject *, FX::FXSelector, void * );
  long                  onUpdatePolygonsAsIs       ( FX::FXObject *, FX::FXSelector, void * );
  long                  onUpdateShadingSmooth      ( FX::FXObject *, FX::FXSelector, void * );
  long                  onUpdateShadingFlat        ( FX::FXObject *, FX::FXSelector, void * );
  long                  onUpdateShadingAsIs        ( FX::FXObject *, FX::FXSelector, void * );
  long                  onUpdateBoundingBox        ( FX::FXObject *, FX::FXSelector, void * );
  long                  onUpdateBoundingSphere     ( FX::FXObject *, FX::FXSelector, void * );
  long                  onUpdateNumRenderPasses    ( FX::FXObject *, FX::FXSelector, void * );
  long                  onUpdateSortBackToFront    ( FX::FXObject *, FX::FXSelector, void * );
  long                  onUpdateFOV                ( FX::FXObject *, FX::FXSelector, void * );

  // Run the menu in a modal loop.
  void                  runModalMenu ( FX::FXMenuPane *menu, int x, int y );

  void                  handleMenu ( float x, float y );
  void                  buildDefaultMenu ( FX::FXWindow *owner, MenuPanes &menus );

  virtual void          buttonPressed  ( unsigned int button, const Usul::Devices::State& state );
  virtual void          buttonReleased ( unsigned int button, const Usul::Devices::State& state );

  virtual void          keyPressed ( unsigned int code ) ;
  virtual void          keyReleased ( unsigned int code );

  virtual void          onMotion( const Usul::Devices::State& state );

  virtual void          onMouseWheel ( double delta );

protected:

  // Needed for a fox-macro. Do not use.
  Canvas();

  void                  _buttonZeroPressed ( const Usul::Devices::State& state );
  void                  _buttonOnePressed ( const Usul::Devices::State& state );
  void                  _buttonTwoPressed ( const Usul::Devices::State& state );

  void                  _buttonZeroReleased ( const Usul::Devices::State& state );
  void                  _buttonOneReleased ( const Usul::Devices::State& state );
  void                  _buttonTwoReleased ( const Usul::Devices::State& state );

  void                  _initDevices();

  void                  _onCommandPolygonMode ( osg::PolygonMode::Face face, osg::PolygonMode::Mode mode );
  void                  _onCommandShadeModel  ( osg::ShadeModel::Mode mode );
  void                  _onUpdatePolygonMode  ( FX::FXObject *sender, osg::PolygonMode::Face face, osg::PolygonMode::Mode mode ) const;
  void                  _onUpdateShadeModel   ( FX::FXObject *sender, osg::ShadeModel::Mode mode ) const;

  ///  Usul::Interfaces::IGetDocument
  virtual IUnknown*          getDocument();

  virtual void               handleMessage ( unsigned short message );


  /// Usul::Interfaces::IFoxWindow
  virtual FX::FXWindow*      getFoxWindow() { return this; }

private:

  // Typedef(s).
  typedef std::vector< Usul::Devices::IDevice::ValidRefPtr > DeviceList;

  // Do not copy.
  Canvas ( const Canvas & );
  Canvas &operator = ( const Canvas & );

  DeviceList      _devices;
  unsigned int    _refCount;

  FXDECLARE ( Canvas );
};

} // namespace Views
} // namespace OsgFox


#endif // _OPEN_SCENE_GRAPH_FOX_BINDING_CANVAS_H_
