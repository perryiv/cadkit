
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

// Disable deprecated warning in Visual Studio 8 
#if defined ( _MSC_VER ) && _MSC_VER == 1400
#pragma warning ( disable : 4996 )
#endif

#include "OsgFox/Views/Precompiled.h"
#include "OsgFox/Views/Canvas.h"
#include "OsgFox/Views/Registry.h"
#include "OsgFox/Views/FoxContext.h"

#include "OsgTools/Draggers/Dragger.h"
#include "OsgTools/Render/Defaults.h"
#include "OsgTools/Render/EventAdapter.h"
#include "OsgTools/Jitter.h"

#include "FoxTools/Errors/ErrorChecker.h"
#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Headers/App.h"
#include "FoxTools/Headers/MainWindow.h"
#include "FoxTools/Functions/Check.h"
#include "FoxTools/Functions/Enable.h"

#include "Usul/Components/Manager.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Math/Constants.h"
#include "Usul/Math/UMath.h"
#include "Usul/Bits/Bits.h"
#include "Usul/System/LastError.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Interfaces/IUpdateTreeControls.h"
#include "Usul/Interfaces/IStatusBar.h"
#include "Usul/Interfaces/IReportErrors.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IMaterialEditor.h"
#include "Usul/Interfaces/ICleanUp.h"
#include "Usul/Interfaces/IGetBoundingBox.h"
#include "Usul/Resources/StatusBar.h"
#include "Usul/Resources/ReportErrors.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Shared/Preferences.h"
#include "Usul/System/Clock.h"
#include "Usul/Predicates/Tolerance.h"
#include "Usul/Functors/Delete.h"
#include "Usul/Scope/Reset.h"
#include "Usul/Devices/IKeyboardDevice.h"
#include "Usul/Devices/Factory.h"
#include "Usul/Devices/State.h"
#include "Usul/Cast/Cast.h"

#include <limits>

using namespace OsgFox::Views;


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( Canvas ) CanvasMap[] =
{
  // Message type,                     id,                                                      handler.
  FXMAPFUNC ( FX::SEL_FOCUSIN,         0,                                                       Canvas::onFocusIn                     ),
  FXMAPFUNC ( FX::SEL_CHORE,           Canvas::ID_RENDER,                                       Canvas::onRender                      ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Usul::Interfaces::ISendMessage::ID_RENDER_SCENE,         Canvas::onRender                      ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Usul::Interfaces::ISendMessage::ID_DISPLAY_LISTS_UPDATE, Canvas::onDisplayListSet              ),
  FXMAPFUNC ( FX::SEL_CHORE,           Canvas::ID_CAMERA_RESET,                                 Canvas::onCommandCameraReset          ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_CAMERA_RESET,                                 Canvas::onCommandCameraReset          ),
  FXMAPFUNC ( FX::SEL_CHORE,           Canvas::ID_CAMERA_FIT,                                   Canvas::onCommandCameraFit            ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_CAMERA_FIT,                                   Canvas::onCommandCameraFit            ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_CAMERA_COPY,                                  Canvas::onCommandCameraCopy           ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_CAMERA_PASTE,                                 Canvas::onCommandCameraPaste          ),
  FXMAPFUNC ( FX::SEL_UPDATE,          Canvas::ID_CAMERA_PASTE,                                 Canvas::onUpdateCameraPaste           ),
  FXMAPFUNC ( FX::SEL_UPDATE,          Canvas::ID_POLYGONS_FILLED,                              Canvas::onUpdatePolygonsFilled        ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_POLYGONS_FILLED,                              Canvas::onCommandPolygonsFilled       ),
  FXMAPFUNC ( FX::SEL_UPDATE,          Canvas::ID_POLYGONS_WIRE,                                Canvas::onUpdatePolygonsWire          ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_POLYGONS_WIRE,                                Canvas::onCommandPolygonsWire         ),
  FXMAPFUNC ( FX::SEL_UPDATE,          Canvas::ID_POLYGONS_HIDDEN,                              Canvas::onUpdatePolygonsHidden        ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_POLYGONS_HIDDEN,                              Canvas::onCommandPolygonsHidden       ),
  FXMAPFUNC ( FX::SEL_UPDATE,          Canvas::ID_POLYGONS_POINTS,                              Canvas::onUpdatePolygonsPoints        ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_POLYGONS_POINTS,                              Canvas::onCommandPolygonsPoints       ),
  FXMAPFUNC ( FX::SEL_UPDATE,          Canvas::ID_POLYGONS_AS_IS,                               Canvas::onUpdatePolygonsAsIs          ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_POLYGONS_AS_IS,                               Canvas::onCommandPolygonsAsIs         ),
  FXMAPFUNC ( FX::SEL_UPDATE,          Canvas::ID_SHADING_SMOOTH,                               Canvas::onUpdateShadingSmooth         ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_SHADING_SMOOTH,                               Canvas::onCommandShadingSmooth        ),
  FXMAPFUNC ( FX::SEL_UPDATE,          Canvas::ID_SHADING_FLAT,                                 Canvas::onUpdateShadingFlat           ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_SHADING_FLAT,                                 Canvas::onCommandShadingFlat          ),
  FXMAPFUNC ( FX::SEL_UPDATE,          Canvas::ID_SHADING_AS_IS,                                Canvas::onUpdateShadingAsIs           ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_SHADING_AS_IS,                                Canvas::onCommandShadingAsIs          ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_EDIT_BACKGROUND,                              Canvas::onCommandEditBackground       ),
  FXMAPFUNC ( FX::SEL_CHANGED,         Canvas::ID_BACKGROUND_CHANGED,                           Canvas::onBackgroundChanged           ), 
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_DEFAULT_BACKGROUND,                           Canvas::onCommandDefaultBackground    ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_BOUNDING_BOX,                                 Canvas::onCommandBoundingBox          ),
  FXMAPFUNC ( FX::SEL_UPDATE,          Canvas::ID_BOUNDING_BOX,                                 Canvas::onUpdateBoundingBox           ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_BOUNDING_SPHERE,                              Canvas::onCommandBoundingSphere       ),
  FXMAPFUNC ( FX::SEL_UPDATE,          Canvas::ID_BOUNDING_SPHERE,                              Canvas::onUpdateBoundingSphere        ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_NUM_RENDER_PASSES,                            Canvas::onCommandNumRenderPasses      ),
  FXMAPFUNC ( FX::SEL_UPDATE,          Canvas::ID_NUM_RENDER_PASSES,                            Canvas::onUpdateNumRenderPasses       ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_SORT_BACK_TO_FRONT,                           Canvas::onCommandSortBackToFront      ),
  FXMAPFUNC ( FX::SEL_UPDATE,          Canvas::ID_SORT_BACK_TO_FRONT,                           Canvas::onUpdateSortBackToFront       ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_SEMI_TRANSPARENT,                             Canvas::onCommandSemiTransparent      ),
  FXMAPFUNC ( FX::SEL_COMMAND,         Canvas::ID_FOV,                                          Canvas::onCommandFOV                  ),
  FXMAPFUNC ( FX::SEL_UPDATE,          Canvas::ID_FOV,                                          Canvas::onUpdateFOV                   ),
};

// This class, base class, array of message handlers, size of the array.
FOX_TOOLS_IMPLEMENT ( Canvas, BaseClass, CanvasMap, ARRAYNUMBER ( CanvasMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor (needed for FOX_TOOLS_IMPLEMENT macro).
//
///////////////////////////////////////////////////////////////////////////////

Canvas::Canvas() : BaseClass(),
  _devices         (),
  _refCount        ( 0 )
{
  // Do not use this constructor.
  FOX_TOOLS_ERROR_CHECKER ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Canvas::Canvas ( Document *doc, FX::FXComposite *parent, FX::FXGLVisual *visual ) :
BaseClass ( doc, parent, visual ),
  _devices         (),
  _refCount        ( 0 )
{
  // Initialize any devices.
  this->_initDevices();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Canvas::~Canvas()
{
  this->clearDevices();

  // Better be zero
  USUL_ASSERT ( 0 == _refCount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::create()
{
  // Call the base class's function first.
  BaseClass::create();

  // Set the cursor.
  this->updateCursor();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the default background.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::defaultBackground()
{
  if ( this->viewer() )
  {
    this->viewer()->backgroundColor ( OsgTools::Render::Defaults::CLEAR_COLOR );
    this->viewer()->render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  This render function is typically called from a chore.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onRender ( FX::FXObject *, FX::FXSelector, void * )
{
  if( this->viewer() )
    this->viewer()->render();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the window gains focus.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onFocusIn ( FX::FXObject *object, FX::FXSelector selector, void *callData )
{
#if 0
  // Update the tree control.
  MainWindow *main ( OsgFox::mainWindow ( this->getParent() ) );
  Usul::Interfaces::IUpdateTreeControls::QueryPtr update ( main->queryInterface( Usul::Interfaces::IUpdateTreeControls::IID ) );
  if( update.valid() )  
    update->updateTreeControls ( this->scene() );
#endif

  // Call the base class's function.
  return BaseClass::onFocusIn ( object, selector, callData );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::backgroundColor ( const osg::Vec4 &color )
{
  // Set the color.
  if ( this->viewer() )
    this->viewer()->backgroundColor ( color );

  // Write it to the registry.
  OsgFox::Registry::write ( Usul::Registry::Sections::OPEN_GL_CANVAS, Usul::Registry::Keys::CLEAR_COLOR, color ); // TODO, put this in preference class?
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4 &Canvas::backgroundColor() const
{
  if ( !this->viewer() )
    throw ( std::runtime_error ( "Cannot get the background color because there is no viewer" ) );
  return this->viewer()->backgroundColor();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a chore.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::addChoreRedraw()
{
  this->getApp()->removeChore ( this, Canvas::ID_RENDER );
  this->getApp()->addChore    ( this, Canvas::ID_RENDER );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Canvas::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IFoxWindow::IID:
    return static_cast < Usul::Interfaces::IFoxWindow* > ( this );
  case Usul::Interfaces::IBackground::IID:
    return static_cast < Usul::Interfaces::IBackground* > ( this );
  case Usul::Interfaces::ISetCursorType::IID:
    return static_cast < Usul::Interfaces::ISetCursorType* > ( this );
  case Usul::Interfaces::IHandleMessage::IID:
    return static_cast < Usul::Interfaces::IHandleMessage* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
  
}

///////////////////////////////////////////////////////////////////////////////
//
//  Reference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::ref()
{
  // Keep track for debugging purposes.
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::unref ( bool allowDeletion )
{
  // Keep track for debugging purposes.
  --_refCount;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IGetDocument::getDocument()
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Canvas::getDocument()
{
  return ( this->document() ) ? this->document()->queryInterface ( Usul::Interfaces::IUnknown::IID ) : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle given message
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::handleMessage ( unsigned short message )
{
  this->handle ( 0x0, FXSEL ( FX::SEL_COMMAND, message ), 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle the menu event.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::handleMenu ( float x, float y )
{
  // Make sure we are in picking mode.
  if ( !this->viewer() || !this->viewer()->picking() || this->viewer()->tool() )
    return;

  // Stop any spinning.
  this->viewer()->spin ( false );

  // Build and run the default menu.
  MenuPanes menus;
  this->buildDefaultMenu ( (FX::FXWindow*) this, menus );
  this->runModalMenu ( menus.front(), (int)x, (int)y );

  // Delete the menus.
  std::for_each ( menus.begin(), menus.end(), Usul::Functors::DeleteObject() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize devices with listeners.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::_initDevices()
{
  _devices = Usul::Devices::Factory::instance().devices( this->queryInterface( Usul::Interfaces::IUnknown::IID ) );

  for ( DeviceList::iterator iter = _devices.begin(); iter != _devices.end(); ++iter )
  {
    (*iter)->addButtonListener( 0, this );
    (*iter)->addButtonListener( 1, this );
    (*iter)->addButtonListener( 2, this );
    (*iter)->addMotionListener( this );
    (*iter)->addMouseWheelListener( this );

    Usul::Devices::IKeyboardDevice::QueryPtr keyboard ( (*iter).get() );

    if( keyboard.valid() )
      keyboard->addKeyListener ( this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the menu in a modal loop.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::runModalMenu ( FX::FXMenuPane *menu, int x, int y )
{
  // Handle bad input.
  if ( !menu )
    return;

  // Show the menu-pane.
  menu->popup ( 0x0, x, y );

  // Run a model loop until the menu is dismissed.
  this->getApp()->runModalWhileShown ( menu );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a polygon mode is selected.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::_onCommandPolygonMode ( osg::PolygonMode::Face face, osg::PolygonMode::Mode mode )
{
  this->viewer()->setPolygonMode ( face, mode );
  this->viewer()->removeHiddenLines();
  this->viewer()->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the menu button needs to be updated.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::_onUpdatePolygonMode ( FX::FXObject *sender, osg::PolygonMode::Face face, osg::PolygonMode::Mode mode ) const
{
  // Set the check.
  bool result ( this->viewer()->hasPolygonMode ( face, mode ) );
  FoxTools::Functions::check( result, sender );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a shading model is selected.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::_onCommandShadeModel ( osg::ShadeModel::Mode mode )
{
  this->viewer()->setShadeModel ( mode );
  this->viewer()->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the menu button needs to be updated.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::_onUpdateShadeModel ( FX::FXObject *sender, osg::ShadeModel::Mode mode ) const
{
  // Set the check.
  bool result ( this->viewer()->hasShadeModel ( mode ) );
  FoxTools::Functions::check( result, sender );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the default menu.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::buildDefaultMenu ( FX::FXWindow *owner, MenuPanes &menus )
{
  // Make a menu-pane.
  FX::FXMenuPane *menu = new FX::FXMenuPane ( owner );
  menus.push_back ( menu );

  // Add the buttons.
  new FX::FXMenuCommand ( menu, "&Fit\t\tMove the camera such that all the geometry is visible.", 0x0, this, Canvas::ID_CAMERA_FIT );
  new FX::FXMenuCommand ( menu, "&Reset\t\tReset the camera position and orientation.",           0x0, this, Canvas::ID_CAMERA_RESET );
  new FX::FXMenuSeparator ( menu );
  new FX::FXMenuCommand ( menu, "&Copy\t\tCopy the current camera properties into the buffer.", 0x0, this, Canvas::ID_CAMERA_COPY );
  new FX::FXMenuCommand ( menu, "&Paste\t\tPaste the camera properties from the buffer.",       0x0, this, Canvas::ID_CAMERA_PASTE );

  // Polygon submenu.
  new FX::FXMenuSeparator ( menu );
  FX::FXMenuPane *polygonMenu = new FX::FXMenuPane ( owner );
  new FX::FXMenuCascade ( menu, "Polygons", 0x0, polygonMenu );
  new FX::FXMenuRadio ( polygonMenu, "&Filled\t\tDraw polygons filled.",                this, Canvas::ID_POLYGONS_FILLED );
  new FX::FXMenuRadio ( polygonMenu, "&Wire Frame\t\tDraw polygons as wire-frame.",     this, Canvas::ID_POLYGONS_WIRE   );
  new FX::FXMenuRadio ( polygonMenu, "&Hidden Lines\t\tDraw polygons as hidden lines.", this, Canvas::ID_POLYGONS_HIDDEN );
  new FX::FXMenuRadio ( polygonMenu, "&Points\t\tDraw polygons as points.",             this, Canvas::ID_POLYGONS_POINTS );
  new FX::FXMenuRadio ( polygonMenu, "As &Is\t\tDraw polygons as they are.",            this, Canvas::ID_POLYGONS_AS_IS  );
  menus.push_back ( polygonMenu );

  // Shading submenu.
  FX::FXMenuPane *shadingMenu = new FX::FXMenuPane ( owner );
  new FX::FXMenuCascade ( menu, "Shading", 0x0, shadingMenu );
  new FX::FXMenuRadio ( shadingMenu, "&Smooth\t\tDraw polygons with smooth shading.",   this, Canvas::ID_SHADING_SMOOTH );
  new FX::FXMenuRadio ( shadingMenu, "&Flat\t\tDraw polygons with flat shading.",       this, Canvas::ID_SHADING_FLAT   );
  new FX::FXMenuRadio ( shadingMenu, "&As &Is\t\tDraw polygons with original shading.", this, Canvas::ID_SHADING_AS_IS  );
  menus.push_back ( shadingMenu );

  // Bounding-volumes submenu.
  FX::FXMenuPane *boundsMenu = new FX::FXMenuPane ( owner );
  new FX::FXMenuCascade ( menu, "Bounding", 0x0, boundsMenu );
  new FX::FXMenuCheck ( boundsMenu, "&Box\t\tShow/hide the bounding box.",       this, Canvas::ID_BOUNDING_BOX    );
  new FX::FXMenuCheck ( boundsMenu, "&Sphere\t\tShow/hide the bounding sphere.", this, Canvas::ID_BOUNDING_SPHERE );
  menus.push_back ( boundsMenu );

  // Background submenu.
  new FX::FXMenuSeparator ( menu );
  FX::FXMenuPane *backgroundMenu = new FX::FXMenuPane ( owner );
  new FX::FXMenuCascade ( menu, "Background", 0x0, backgroundMenu );
  new FX::FXMenuCommand ( backgroundMenu, "&Edit...\t\tEdit the background.",    0x0, this, Canvas::ID_EDIT_BACKGROUND );
  new FX::FXMenuCommand ( backgroundMenu, "&Default\t\tSet default background.", 0x0, this, Canvas::ID_DEFAULT_BACKGROUND );
  menus.push_back ( backgroundMenu );

  // Is there an accumulation buffer? It is needed for jitter-based anti-aliasing.
  if ( this->viewer()->hasAccumBuffer() )
  {
    // Rendering passes submenu. Each button's user-data points to an integer 
    // indicating how many passes.
    FX::FXMenuPane *renderPassesMenu = new FX::FXMenuPane ( owner );
    new FX::FXMenuCascade ( menu, "Rendering Passes", 0x0, renderPassesMenu );
    FX::FXMenuRadio *radio ( new FX::FXMenuRadio ( renderPassesMenu, "1-Pass", this, Canvas::ID_NUM_RENDER_PASSES ) );
    radio->setUserData ( USUL_UNSAFE_CAST ( void *, 1 ) );
    OsgTools::Jitter::Available passes ( OsgTools::Jitter::instance().available() );
    for ( OsgTools::Jitter::Available::const_iterator i = passes.begin(); i != passes.end(); ++i )
    {
      radio = new FX::FXMenuRadio ( renderPassesMenu, i->second.c_str(), this, Canvas::ID_NUM_RENDER_PASSES );
      radio->setUserData ( USUL_UNSAFE_CAST ( void *, i->first ) );
    }
    menus.push_back ( renderPassesMenu );
  }
  new FX::FXMenuCheck ( menu, "Sort Back to Front\t\tSet/Unset back to front sorting.", this, Canvas::ID_SORT_BACK_TO_FRONT );
  
  FX::FXMenuPane *transparencyMenu = new FX::FXMenuPane ( owner );
  new FX::FXMenuCascade ( menu, "Transparency", 0x0, transparencyMenu );
  new FX::FXMenuCommand ( transparencyMenu, "0.05\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.10\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.15\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.20\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.25\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.30\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.35\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.40\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.45\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.50\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.55\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.60\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.65\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.70\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.75\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.80\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.85\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.90\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "0.95\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  new FX::FXMenuCommand ( transparencyMenu, "1.0\t\t", 0x0, this, Canvas::ID_SEMI_TRANSPARENT );
  menus.push_back ( transparencyMenu );

  FX::FXMenuPane *fovMenu = new FX::FXMenuPane ( owner );
  new FX::FXMenuCascade ( menu, "FOV", 0x0, fovMenu );
  new FX::FXMenuRadio ( fovMenu, "20\t\tSet Field of View.",   this, Canvas::ID_FOV );
  new FX::FXMenuRadio ( fovMenu, "25\t\tSet Field of View.",   this, Canvas::ID_FOV );
  new FX::FXMenuRadio ( fovMenu, "30\t\tSet Field of View.",   this, Canvas::ID_FOV );
  new FX::FXMenuRadio ( fovMenu, "35\t\tSet Field of View.",   this, Canvas::ID_FOV );
  new FX::FXMenuRadio ( fovMenu, "40\t\tSet Field of View.",   this, Canvas::ID_FOV );
  new FX::FXMenuRadio ( fovMenu, "45\t\tSet Field of View.",   this, Canvas::ID_FOV );
  new FX::FXMenuRadio ( fovMenu, "50\t\tSet Field of View.",   this, Canvas::ID_FOV );
  new FX::FXMenuRadio ( fovMenu, "55\t\tSet Field of View.",   this, Canvas::ID_FOV );
  new FX::FXMenuRadio ( fovMenu, "60\t\tSet Field of View.",   this, Canvas::ID_FOV );
  new FX::FXMenuRadio ( fovMenu, "65\t\tSet Field of View.",   this, Canvas::ID_FOV );
  new FX::FXMenuRadio ( fovMenu, "70\t\tSet Field of View.",   this, Canvas::ID_FOV );
  new FX::FXMenuRadio ( fovMenu, "75\t\tSet Field of View.",   this, Canvas::ID_FOV );
  new FX::FXMenuRadio ( fovMenu, "80\t\tSet Field of View.",   this, Canvas::ID_FOV );
  new FX::FXMenuRadio ( fovMenu, "85\t\tSet Field of View.",   this, Canvas::ID_FOV );
  new FX::FXMenuRadio ( fovMenu, "90\t\tSet Field of View.",   this, Canvas::ID_FOV );
  menus.push_back ( fovMenu );

  // Create it.
  menu->create();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Fit" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandCameraFit ( FX::FXObject *, FX::FXSelector, void * )
{
  // Move the camera.
  this->viewer()->spin ( false ); // Stop any scale or translate.
  this->viewer()->camera ( OsgTools::Render::Viewer::FIT );
  this->viewer()->render();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Reset" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandCameraReset ( FX::FXObject *, FX::FXSelector, void * )
{
  // Move the camera.
  this->viewer()->spin ( false );
  this->viewer()->camera ( OsgTools::Render::Viewer::RESET );
  this->viewer()->render();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Copy" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandCameraCopy ( FX::FXObject *, FX::FXSelector, void * )
{
  this->viewer()->copyCamera();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Paste" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandCameraPaste ( FX::FXObject *, FX::FXSelector, void * )
{
  if ( this->viewer()->canPasteCamera() )
    this->viewer()->pasteCamera();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Paste" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onUpdateCameraPaste ( FX::FXObject *sender, FX::FXSelector, void * )
{
  // Enable or disable.
  FoxTools::Functions::enable ( this->viewer()->canPasteCamera(), sender );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons Filled" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandPolygonsFilled ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandPolygonMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons Wire Frame" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandPolygonsWire ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandPolygonMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons Hidden" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandPolygonsHidden ( FX::FXObject *, FX::FXSelector, void * )
{
  this->viewer()->toggleHiddenLines();
  this->viewer()->render();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons Points" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandPolygonsPoints ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandPolygonMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::POINT );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons As Is" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandPolygonsAsIs ( FX::FXObject *, FX::FXSelector, void * )
{
  this->viewer()->removePolygonMode();
  this->viewer()->removeHiddenLines();
  this->viewer()->render();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Shading Smooth" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandShadingSmooth ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandShadeModel ( osg::ShadeModel::SMOOTH );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Shading Flat" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandShadingFlat ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandShadeModel ( osg::ShadeModel::FLAT );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Shading As Is" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandShadingAsIs ( FX::FXObject *, FX::FXSelector, void * )
{
  this->viewer()->removeShadeModel();
  this->viewer()->render();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "View Filled" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onUpdatePolygonsFilled ( FX::FXObject *sender, FX::FXSelector, void * )
{
  this->_onUpdatePolygonMode ( sender, osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons Wire Frame" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onUpdatePolygonsWire ( FX::FXObject *sender, FX::FXSelector, void * )
{
  this->_onUpdatePolygonMode ( sender, osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons Hidden" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onUpdatePolygonsHidden ( FX::FXObject *sender, FX::FXSelector, void * )
{
  FoxTools::Functions::check ( this->viewer()->hasHiddenLines(), sender );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons Points" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onUpdatePolygonsPoints ( FX::FXObject *sender, FX::FXSelector, void * )
{
  this->_onUpdatePolygonMode ( sender, osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::POINT );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons As Is" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onUpdatePolygonsAsIs ( FX::FXObject *sender, FX::FXSelector, void * )
{
  // Set the check.
  bool result ( !this->viewer()->hasPolygonMode() && !this->viewer()->hasHiddenLines() );
  FoxTools::Functions::check ( result, sender );

  // Message handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Shading Smooth" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onUpdateShadingSmooth ( FX::FXObject *sender, FX::FXSelector, void * )
{
  this->_onUpdateShadeModel ( sender, osg::ShadeModel::SMOOTH );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Shading Flat" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onUpdateShadingFlat ( FX::FXObject *sender, FX::FXSelector, void * )
{
  this->_onUpdateShadeModel ( sender, osg::ShadeModel::FLAT );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Shading As Is" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onUpdateShadingAsIs ( FX::FXObject *sender, FX::FXSelector, void * )
{
  // Set the check.
  bool result ( this->viewer()->hasShadeModel() );
  FoxTools::Functions::check( result, sender );

  // Message handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the user wants to edit the background color.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandEditBackground ( FX::FXObject *, FX::FXSelector, void * )
{
  this->editBackground();

  // Message was handled.
  return 1;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Edit the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::editBackground()
{
  // Make a color dialog and set its properties.
  FX::FXColorDialog dialog ( (FX::FXWindow*) FoxTools::Functions::mainWindow(), "Color Dialog", DECOR_TITLE | DECOR_BORDER );
  dialog.setOpaqueOnly ( dialog.isOpaqueOnly() );
  osg::Vec4 c1 ( this->backgroundColor() );
  dialog.setRGBA ( FX::FXVec4f ( c1[0], c1[1], c1[2], c1[3] ) );
  dialog.setTarget ( this );
  dialog.setSelector ( Canvas::ID_BACKGROUND_CHANGED );

  // Run the dialog in a modal loop.
  if ( dialog.execute ( FX::PLACEMENT_OWNER ) )
  {
    // Set the background color from the dialog's color.
    FX::FXVec4f c2 ( dialog.getRGBA() );
    this->backgroundColor ( osg::Vec4 ( c2[0], c2[1], c2[2], c2[3] ) );
  }

  // If the user cancelled...
  else
  {
    // Reset the color.
    this->backgroundColor ( osg::Vec4 ( c1[0], c1[1], c1[2], c1[3] ) );
  }

  // Whether the user set a new color or cancelled, we have to render again.
  // They may have cancelled after having moved the controls.
  this->viewer()->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  set the background color.  Returns old value
//
///////////////////////////////////////////////////////////////////////////////
void Canvas::setBackground ( const osg::Vec4 &color)
{
  this->backgroundColor ( color );
  this->viewer()->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the user is editing the background color.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onBackgroundChanged ( FX::FXObject *object, FX::FXSelector, void * )
{
  FOX_TOOLS_ERROR_CHECKER ( TRUE == object->isMemberOf ( FXMETACLASS ( FX::FXColorDialog ) ) );
  FX::FXColorDialog *dialog = (FX::FXColorDialog *) object;
  FX::FXVec4f c ( dialog->getRGBA() );
  this->backgroundColor ( osg::Vec4 ( c[0], c[1], c[2], c[3] ) );
  this->viewer()->render();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the user wants the default background.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandDefaultBackground ( FX::FXObject *, FX::FXSelector, void * )
{
  this->defaultBackground();
  this->viewer()->render();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Bounds Box" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandBoundingBox ( FX::FXObject *, FX::FXSelector, void * )
{
  this->viewer()->boundingBox ( !this->viewer()->boundingBox() );
  this->viewer()->render();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Bounds Sphere" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandBoundingSphere ( FX::FXObject *, FX::FXSelector, void * )
{
  this->viewer()->boundingSphere ( !this->viewer()->boundingSphere() );
  this->viewer()->render();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Bounds Box" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onUpdateBoundingBox ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::check ( this->viewer()->boundingBox(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Bounds Sphere" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onUpdateBoundingSphere ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::check ( this->viewer()->boundingSphere(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a chore to fit the camera.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::addChoreCameraFit()
{
  this->getApp()->removeChore ( this, Canvas::ID_CAMERA_FIT );
  this->getApp()->addChore    ( this, Canvas::ID_CAMERA_FIT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a chore to reset the camera.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::addChoreCameraReset()
{
  this->getApp()->removeChore ( this, Canvas::ID_CAMERA_RESET );
  this->getApp()->addChore    ( this, Canvas::ID_CAMERA_RESET );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Rendering Passes" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandNumRenderPasses ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the number of passes.
  FX::FXWindow *window ( SAFE_CAST_FOX ( FX::FXWindow, object ) );
  const void *data ( ( window ) ? window->getUserData() : 0x0 );
  unsigned int numPasses ( USUL_UNSAFE_CAST ( unsigned int, data ) );

  // Set the number of passes.
  this->viewer()->numRenderPasses ( ( numPasses > 0 ) ? numPasses : this->viewer()->numRenderPasses() );

  // Handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Rendering Passes" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onUpdateNumRenderPasses ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the number of passes.
  FX::FXWindow *window ( SAFE_CAST_FOX ( FX::FXWindow, object ) );
  const void *data ( ( window ) ? window->getUserData() : 0x0 );
  unsigned int numPasses ( USUL_UNSAFE_CAST ( unsigned int, data ) );

  // Set the check if the number of passes matches.
  FoxTools::Functions::check ( this->viewer() && numPasses == this->viewer()->numRenderPasses(), object );

  // Handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set sort back to front state.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandSortBackToFront ( FX::FXObject *, FX::FXSelector, void * )
{
  if( this->viewer() )
  {
    this->viewer()->sortBackToFront ( !this->viewer()->sortBackToFront() );
    this->viewer()->render();
  }

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the check box based on the sort back to front state.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onUpdateSortBackToFront    ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::check ( this->viewer() && this->viewer()->sortBackToFront(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make everything in the model semi transparent.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandSemiTransparent ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXMenuCommand *command = SAFE_CAST_FOX ( FX::FXMenuCommand, object );
  if( 0x0 == command )
    return 0;

  float alpha ( 0.0 );
  std::istringstream in ( command->getText().text() );
  in >> alpha;

  this->viewer()->setAllMaterialsAlpha ( alpha );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the FOV.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onCommandFOV ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXMenuRadio *radio = SAFE_CAST_FOX ( FX::FXMenuRadio, object );
  if( 0x0 == radio )
    return 0;

  double fov ( 0.0 );
  std::istringstream in ( radio->getText().text() );
  in >> fov;

  Usul::Shared::Preferences::instance().setDouble( Usul::Registry::Keys::FOV, fov );

  unsigned int width  ( static_cast < unsigned int > ( this->getWidth() ) );
  unsigned int height ( static_cast < unsigned int > ( this->getHeight() ) );
  this->viewer()->resize ( width, height );

  this->viewer()->render();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the FOV radio button.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onUpdateFOV ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXMenuRadio *radio = SAFE_CAST_FOX ( FX::FXMenuRadio, object );
  if( 0x0 == radio )
    return 0;

  double fov ( 0.0 );
  std::istringstream in ( radio->getText().text() );
  in >> fov;

  FoxTools::Functions::check( fov == Usul::Shared::Preferences::instance().getDouble( Usul::Registry::Keys::FOV ), object );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the devices.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::clearDevices()
{
  // Clear all devices.
  for ( DeviceList::iterator iter = _devices.begin(); iter != _devices.end(); ++iter )
    (*iter)->clear();

  _devices.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the display lists.
//
///////////////////////////////////////////////////////////////////////////////

long Canvas::onDisplayListSet ( FX::FXObject *, FX::FXSelector, void * )
{
  this->viewer()->setDisplayLists();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Button was pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::buttonPressed  ( unsigned int button, const Usul::Devices::State& state )
{
  if ( 0 == button )
    this->_buttonZeroPressed ( state );
  else if ( 1 == button )
    this->_buttonOnePressed ( state );
  else if ( 2 == button )
    this->_buttonTwoPressed ( state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Button was released.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::buttonReleased ( unsigned int button, const Usul::Devices::State& state )
{
  if ( 0 == button )
    this->_buttonZeroReleased ( state );
  else if ( 1 == button )
    this->_buttonOneReleased ( state );
  else if ( 2 == button )
    this->_buttonTwoReleased ( state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::keyPressed ( unsigned int code )
{
  // Stop any spin.
  this->viewer()->spin ( false );

  // Process the key.
  switch ( code )
  {
    // See if it was the space-bar or the r-key...
    case Usul::Devices::KEY_space:
    case Usul::Devices::KEY_r:

      // Move the camera.
      this->viewer()->camera ( OsgTools::Render::Viewer::RESET );
      break;

    // See if it was the f-key...
    case Usul::Devices::KEY_f:

      // Move the camera.
      this->viewer()->camera ( OsgTools::Render::Viewer::FIT );
      break;

    // See if it was the right-arrow key...
    case Usul::Devices::KEY_Right:

      // Move the camera.
      this->viewer()->rotate ( osg::Vec3 ( 0, 1, 0 ), -osg::PI_2 );
      break;

    // See if it was the left-arrow key...
    case Usul::Devices::KEY_Left:

      // Move the camera.
      this->viewer()->rotate ( osg::Vec3 ( 0, 1, 0 ), osg::PI_2 );
      break;

    // See if it was the up-arrow key...
    case Usul::Devices::KEY_Up:

      // Move the camera.
      this->viewer()->rotate ( osg::Vec3 ( 1, 0, 0 ), osg::PI_2 );
      break;

    // See if it was the down-arrow key...
    case Usul::Devices::KEY_Down:

      // Move the camera.
      this->viewer()->rotate ( osg::Vec3 ( 1, 0, 0 ), -osg::PI_2 );
      break;

    // See if it was the p-key...
    case Usul::Devices::KEY_p:
      if( !this->viewer()->picking())
        this->viewer()->setMode ( OsgTools::Render::Viewer::PICK );
      break;

    // See if it was the n-key...
    case Usul::Devices::KEY_n:
      if( !this->viewer()->navigating() )
        this->viewer()->setMode ( OsgTools::Render::Viewer::NAVIGATION );
      break;

    // See if it was one of the control keys
    case Usul::Devices::KEY_Control_L:
    case Usul::Devices::KEY_Control_R:
      this->viewer()->loadLastTool();
      break;
    // See if it was the s key...
    case Usul::Devices::KEY_s:
      this->viewer()->stereo ( !this->viewer()->stereo() );
      this->viewer()->render();
      break;

    // See if it was the h key...
    case Usul::Devices::KEY_h:
      if( this->viewer()->polygonMode() == Usul::Interfaces::IPolygonMode::HIDDEN_LINES )
        this->viewer()->polygonMode( Usul::Interfaces::IPolygonMode::NONE );
      else
        this->viewer()->polygonMode( Usul::Interfaces::IPolygonMode::HIDDEN_LINES );
      this->viewer()->render();
      break;

    // See if it was the w key...
    case Usul::Devices::KEY_w:
      if( this->viewer()->polygonMode() == Usul::Interfaces::IPolygonMode::WIRE_FRAME )
        this->viewer()->polygonMode( Usul::Interfaces::IPolygonMode::NONE );
      else
        this->viewer()->polygonMode( Usul::Interfaces::IPolygonMode::WIRE_FRAME );
      this->viewer()->render();
      break;
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Key released.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::keyReleased ( unsigned int code )
{
  // See what key was just released.
  switch ( code )
  {
    case Usul::Devices::KEY_Escape: 
    case Usul::Devices::KEY_v:
      this->viewer()->cycleMode();
      break;

    case Usul::Devices::KEY_Control_L:
    case Usul::Devices::KEY_Control_R:
      {
        Usul::Interfaces::ICleanUp::QueryPtr cleanUp ( this->viewer()->getTool() );
        if( cleanUp.valid() )
          cleanUp->cleanUp( this->viewer()->queryInterface( Usul::Interfaces::IUnknown::IID ) );
        this->viewer()->doneTool();
      }
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Motion occured.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::onMotion( const Usul::Devices::State& state )
{
  bool left   ( state.buttonPressed( 0 ) );
  bool middle ( state.buttonPressed( 1 ) );
  bool right  ( state.buttonPressed( 2 ) );

  float x ( state.x() );
  float y ( state.y() );

  // See if any mouses button are down.
  unsigned int mouse ( left || middle || right );

  // Set the event type.
  typedef OsgTools::Render::EventAdapter EventAdapter;
  EventAdapter::EventType type ( ( mouse ) ? EventAdapter::DRAG : EventAdapter::MOVE );

  // Handle the events. Make sure you pick before you drag.
  this->viewer()->handleNavigation ( x, y, left, middle, right, type );
  this->viewer()->handlePicking    ( x, y, false, 0 );
  this->viewer()->handleDragging   ( x, y, OsgTools::Draggers::Dragger::MOVE );

  // Handle tool.
  this->viewer()->handleTool ( left, middle, right, true, x, y, 0.0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Button zero pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::_buttonZeroPressed ( const Usul::Devices::State& state )
{
  bool left   ( state.buttonPressed( 0 ) );
  bool middle ( state.buttonPressed( 1 ) );
  bool right  ( state.buttonPressed( 2 ) );

  float x ( state.x() );
  float y ( state.y() );

  // Pop this window to the front.
  this->setFocus();

  this->viewer()->buttonPress ( x, y, left, middle, right );

  unsigned int numClicks ( state.numClicks() );

  // Handle the events. Make sure you pick before you drag.
  this->viewer()->handlePicking  ( x, y, left, numClicks );
  this->viewer()->handleDragging ( x, y, OsgTools::Draggers::Dragger::START );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Button one pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::_buttonOnePressed ( const Usul::Devices::State& state )
{
  bool left   ( state.buttonPressed( 0 ) );
  bool middle ( state.buttonPressed( 1 ) );
  bool right  ( state.buttonPressed( 2 ) );

  float x ( state.x() );
  float y ( state.y() );

  this->viewer()->buttonPress ( x, y, left, middle, right );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Button two pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::_buttonTwoPressed ( const Usul::Devices::State& state )
{
  bool left   ( state.buttonPressed( 0 ) );
  bool middle ( state.buttonPressed( 1 ) );
  bool right  ( state.buttonPressed( 2 ) );

  float x ( state.x() );
  float y ( state.y() );

  this->viewer()->buttonPress ( x, y, left, middle, right );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Button zero released.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::_buttonZeroReleased ( const Usul::Devices::State& state )
{
  bool left   ( state.buttonPressed( 0 ) );
  bool middle ( state.buttonPressed( 1 ) );
  bool right  ( state.buttonPressed( 2 ) );

  float x ( state.x() );
  float y ( state.y() );

  this->viewer()->buttonRelease ( x, y, left, middle, right );

  unsigned int numClicks ( state.numClicks() );

  // Handle the events. Make sure you pick before you drag.
  this->viewer()->handlePicking  ( x, y, left, numClicks );
  this->viewer()->handleDragging ( x, y, OsgTools::Draggers::Dragger::FINISH );

  // Make sure.
  OsgTools::Draggers::Dragger::dragging ( 0x0, osg::Matrixd::identity() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Button one released.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::_buttonOneReleased ( const Usul::Devices::State& state )
{
  bool left   ( state.buttonPressed( 0 ) );
  bool middle ( state.buttonPressed( 1 ) );
  bool right  ( state.buttonPressed( 2 ) );

  float x ( state.x() );
  float y ( state.y() );

  this->viewer()->buttonRelease ( x, y, left, middle, right );

  // Make sure.
  OsgTools::Draggers::Dragger::dragging ( 0x0, osg::Matrixd::identity() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Button two released.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::_buttonTwoReleased ( const Usul::Devices::State& state )
{
  bool left   ( state.buttonPressed( 0 ) );
  bool middle ( state.buttonPressed( 1 ) );
  bool right  ( state.buttonPressed( 2 ) );

  float x ( state.x() );
  float y ( state.y() );

  this->viewer()->buttonRelease ( x, y, left, middle, right );

  float rootX ( state.rootX() );
  float rootY ( state.rootY() );

  // Handle the menu.
  this->handleMenu ( rootX, rootY );

  // Make sure.
  OsgTools::Draggers::Dragger::dragging ( 0x0, osg::Matrixd::identity() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Mouse Wheel Moved.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::onMouseWheel ( double delta )
{
  this->viewer()->zoom ( delta );
}
