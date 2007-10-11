
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The application class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Application.h"
#include "Constants.h"
#include "ErrorChecker.h"
#include "SceneFunctors.h"
#include "ScenePredicates.h"

#include "VRV/Common/Libraries.h"
#include "CadViewer/Functors/ToolPair.h"

#include "CadViewer/Pick/Select.h"
#include "CadViewer/Pick/Seek.h"

#include "Usul/App/Application.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Components/Manager.h"
#include "Usul/Math/Constants.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/System/Host.h"
#include "Usul/Functors/General/If.h"
#include "Usul/Predicates/UnaryPair.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IButtonID.h"

#include "VRV/Common/Buttons.h"

#include "osg/MatrixTransform"
#include "osg/AutoTransform"
#include "osg/Matrix"
#include "osg/Projection"

#include "osgDB/WriteFile"

#include "OsgTools/Axes.h"
#include "OsgTools/Text.h"
#include "OsgTools/Convert.h"
#include "OsgTools/Group.h"
#include "OsgTools/Font.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Visitor.h"

#include "MenuKit/MemFunCallback.h"

#include "vrj/Kernel/Kernel.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Button mappings.
//
///////////////////////////////////////////////////////////////////////////////

namespace CV
{
  // For convenience.
  const unsigned long BUTTON_RED      = VRV::BUTTON1;
  const unsigned long BUTTON_YELLOW   = VRV::BUTTON0;
  const unsigned long BUTTON_GREEN    = VRV::BUTTON2;
  const unsigned long BUTTON_BLUE     = VRV::BUTTON3;
  const unsigned long BUTTON_JOYSTICK = VRV::BUTTON4;
  const unsigned long BUTTON_TRIGGER  = VRV::BUTTON5;

  // Button combinations.
  const unsigned long COMMAND_MENU_TOGGLE      = BUTTON_JOYSTICK;
  const unsigned long COMMAND_MENU_SELECT      = BUTTON_TRIGGER;
  const unsigned long COMMAND_MENU_LEFT        = BUTTON_RED;
  const unsigned long COMMAND_MENU_RIGHT       = BUTTON_BLUE;
  const unsigned long COMMAND_MENU_UP          = BUTTON_YELLOW;
  const unsigned long COMMAND_MENU_DOWN        = BUTTON_GREEN;
  
  const unsigned long COMMAND_SELECT           = BUTTON_TRIGGER;
  const unsigned long COMMAND_HIDE_SELECTED    = BUTTON_YELLOW;
  const unsigned long COMMAND_UNSELECT_VISIBLE = BUTTON_GREEN;
  const unsigned long COMMAND_SHOW_ALL         = BUTTON_RED;  

  const unsigned long NAVIGATE_TOGGLE = BUTTON_YELLOW;
  const unsigned long STOP_NAV_TOOL   = BUTTON_RED;       // helps greatly in sim mode
  const unsigned long NAVIGATE_FLY    = BUTTON_BLUE;      // Joseph doesn't need it, but its handy if anybody else does
  const unsigned long TOOL_SCALE      = BUTTON_GREEN;
  const unsigned long NAVIGATE_NO_NAV = BUTTON_RED;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace CV
{
  // Various callback ids.
  enum
  {
    NO_NAVIGATION,
    TRANSLATE_XZ_LOCAL,       // FLY
    TRANSLATE_XZ_GLOBAL,      // WALK
//    ROTATE_Y_TRANS_Y_GLOBAL,  // POLE
    ROTATE_XY_LOCAL,
    ROTATE_XY_GLOBAL,
    INTERSECT_SELECT,
    INTERSECT_SEEK,
    INTERSECT_HIDE,
  };
};


///////////////////////////////////////////////////////////////////////////////
//
//  These bitmask values do not work well with the template functions in 
//  Bitmask.h if they are declared as enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace CV
{
  namespace Detail
  {
    const unsigned int _TEXT_IS_INITIALIZED       = 0x00000001;
  };
};


using namespace CV;


///////////////////////////////////////////////////////////////////////////////
//
//  Static variables.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Application::_appThread  = 0;
unsigned long Application::_mainThread = 0;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application ( ) :
  BaseClass       ( ),
  _gridBranch     ( new osg::MatrixTransform ),
  _auxiliary      ( new osg::Group ),
  _sceneTool      ( 0x0 ),
  _intersector    ( 0x0 ),
  _rotCenter      ( 0, 0, 0 ),
  _pickText       ( new OsgTools::Text ),
  _navText        ( new OsgTools::Text ),
  _frameText      ( new OsgTools::Text ),
  _msgText        ( new OsgTools::Text ),
  _flags          ( 0 ),
  _iVisibility    ( static_cast < CV::Interfaces::IVisibility* >    ( 0x0 ) ),
  _iSelection     ( static_cast < CV::Interfaces::ISelection* >     ( 0x0 ) ),
  _iMaterialStack ( static_cast < CV::Interfaces::IMaterialStack* > ( 0x0 ) ),
  _textures       ( true )
{
  ErrorChecker ( 1067097070u, 0 == _appThread );
  ErrorChecker ( 2970484549u, 0 == _mainThread );
  ErrorChecker ( 1067094628u, _gridBranch.valid() );
  ErrorChecker ( 1069021589u, _auxiliary.valid() );
  ErrorChecker ( 1071551353u, 0x0 != _pickText.get() );
  ErrorChecker ( 1071551354u, 0x0 != _navText.get() );
  ErrorChecker ( 1071551355u, 0x0 != _frameText.get() );
  ErrorChecker ( 1071551356u, 0x0 != _msgText.get() );

  // Initialize the main thread.
  _mainThread = Usul::Threads::currentThreadId();

  // Hook up the branches.
  this->_sceneRoot()->addChild      ( _auxiliary.get()    );
  //_navBranch->addChild ( _gridBranch.get()   );

  // Name the branches.
  _gridBranch->setName   ( "_gridBranch"   );
  _auxiliary->setName    ( "_auxiliary"    );

  // Hook up the joystick callbacks.
  JoystickCB::RefPtr jcb ( new JoystickCB ( this ) );
  this->joystick()->callback ( VRV::Devices::JOYSTICK_ENTERING_RIGHT, jcb.get() );
  this->joystick()->callback ( VRV::Devices::JOYSTICK_ENTERING_LEFT,  jcb.get() );
  this->joystick()->callback ( VRV::Devices::JOYSTICK_ENTERING_UP,    jcb.get() );
  this->joystick()->callback ( VRV::Devices::JOYSTICK_ENTERING_DOWN,  jcb.get() );

  typedef Usul::Components::Manager Manager;

  // Save the plugin pointers.
  _iVisibility    = Manager::instance().getInterface( CV::Interfaces::IVisibility::IID );
  _iSelection     = Manager::instance().getInterface( CV::Interfaces::ISelection::IID );
  _iMaterialStack = Manager::instance().getInterface( CV::Interfaces::IMaterialStack::IID );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
  _gridFunctors.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_init()
{
  ErrorChecker ( 1067096939, 0 == _appThread );
  ErrorChecker ( 1074061336, !isMainThread() );

  // Initialize the application thread.
  _appThread = Usul::Threads::currentThreadId();

  // Call the base class's function first.
  BaseClass::_init();
  
  // Set the global GL_NORMALIZE flag.
  //this->normalize ( this->preferences()->normalizeVertexNormalsGlobal() );

  // Note: we cannot initialize the text yet because the viewport has not been set.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the text.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initText()
{
#ifdef _MSC_VER
  // Removing anything we may have already.
  this->projectionGroupRemove ( "VRV_TEXT" );

  ErrorChecker ( 1071444744, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 1071444746, 0x0 != _pickText.get() );
  ErrorChecker ( 1071444747, 0x0 != _navText.get() );
  ErrorChecker ( 1071444748, 0x0 != _frameText.get() );
  ErrorChecker ( 1071444749, 0x0 != _msgText.get() );
  ErrorChecker ( 1071556909, false == Usul::Bits::has ( _flags, Detail::_TEXT_IS_INITIALIZED ) );

  if( !this->_isHeadNode() )
    return;

  osg::ref_ptr < osg::Group > group ( this->projectionGroupGet ( "VRV_TEXT" ) );

  // Create a matrix-transform relative to the global coordinate system.
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  ErrorChecker ( 1071452071, mt.valid() );

  mt->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );
  mt->setMatrix ( osg::Matrix::identity() );
  mt->setName ( "TextBranch" );

  // Make the text branch an orthographic projection.
  osg::ref_ptr < osg::Viewport > vp ( this->viewport() );
  float x ( vp->x()      );
  float y ( vp->y()      );
  float w ( vp->width()  );
  float h ( vp->height() );

  // Set the text font.
  std::string f ( OsgTools::Font::fontfile ( "courbd" ) ); // Courier New Bold
  _pickText->font  ( f );
  _navText->font   ( f );
  _frameText->font ( f );
  _msgText->font   ( f );

  // Set the text height.
  float th ( 12 );
  _pickText->height  ( th );
  _navText->height   ( th );
  _frameText->height ( th );
  _msgText->height   ( th );

  // Set the text positions.
  float tx ( 5 ), ty ( 5 );
  _msgText->position   ( osg::Vec3 ( tx, ty, 0 ) ); ty = ty + th + 2;
  _pickText->position  ( osg::Vec3 ( tx, ty, 0 ) ); ty =  h - th - 2;
  _frameText->position ( osg::Vec3 ( tx, ty, 0 ) ); tx =  w - 220;
  _navText->position   ( osg::Vec3 ( tx, ty, 0 ) );

  // Hook up the scene.
  mt->addChild ( (*_pickText) () );
  mt->addChild ( (*_navText)  () );
  mt->addChild ( (*_frameText)() );
  mt->addChild ( (*_msgText)  () );
  //_textBranch->addChild ( mt.get() );
  group->addChild ( mt.get() );

#if 0 // Use for debugging their placement.
  _navText->text   ( "_navText"   );
  _msgText->text   ( "_msgText"   );
  _frameText->text ( "_frameText" );
  _pickText->text  ( "_pickText"  );
#endif
#endif
  // Set the flag that says the text is initialized.
  _flags = Usul::Bits::add ( _flags, Detail::_TEXT_IS_INITIALIZED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the grid to be under the given node.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initGrid ( osg::Node *node )
{
  ErrorChecker ( 1067093690, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 1293823849, 0x0 != node );

  // Get the bounding sphere of the node.
  const osg::BoundingSphere &bs = node->getBound();

  // Handle zero-sized bounding spheres.
  float r = ( bs.radius() <= 1e-6 ) ? 1 : bs.radius();

  // Clean up any old grids.
  _gridFunctors.clear();

  // Set the properties.
  for( int i = 0; i < this->preferences()->numGrids(); ++i )
  {
    OsgTools::Grid grid;
    grid.numBlocks ( this->preferences()->numGridBlocks( i ) );
    grid.size ( r * this->preferences()->gridScale( i ) );
    grid.color ( this->preferences()->gridColor( i ) );
    grid.fillColor ( this->preferences()->gridFillColor( i ) );
    Usul::Math::Matrix44f o;
    o.makeRotation ( this->preferences()->gridRotationAngleRad( static_cast < int > ( i ) ), this->preferences()->gridRotationVector( i ) );
    grid.orientation ( o );

    // Move the center so that it is below the bounding sphere of the node.
    osg::Vec3 c ( bs.center() );
    if(this->preferences()->offsetGrid( static_cast < int > ( i ) )) 
      c[1] = -r;

    grid.center ( Usul::Math::Vec3f ( c[0], c[1], c[2] ) );
    _gridFunctors.push_back(grid);
  }

  _rebuildGrid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rebuild the grid in case the _gridFunctor's state has been updated
//
///////////////////////////////////////////////////////////////////////////////

void Application::_rebuildGrid()
{
  // Remove the old grid and add the new one.
  OsgTools::Group::removeAllChildren ( _gridBranch.get() );
  for( unsigned int i = 0; i < _gridFunctors.size(); ++i )
  {
    _gridBranch->addChild ( _gridFunctors[i]() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the frame is drawn.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_latePreFrame()
{
  USUL_TRACE_SCOPE;

  ErrorChecker ( 1067093580, isAppThread(), CV::NOT_APP_THREAD );
  
  // Call the base class's function.
  BaseClass::_latePreFrame();

  // Update the frame-rate's text.
  this->_updateFrameRateDisplay();

  // Update the analog-input text.
  this->_updateAnalogText();

  // Use the scene-tool if we are supposed to.
  this->_useSceneTool();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the button states and apply to the menu.
//  Returns true if the event was handled.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::_handleMenuEvent( unsigned long id )
{
  USUL_TRACE_SCOPE;

  ErrorChecker ( 1071559313u, isAppThread(), CV::NOT_APP_THREAD );

  // Get the menu.
  Menu::RefPtr menu ( this->menu () );

  if ( 0x0 == menu.get() )
    return false;

  // First see if you are supposed to show or hide it. Always do this first.
  if ( COMMAND_MENU_TOGGLE == id )
  {
    menu->toggleVisible();
    return true;
  }

  // If we are not expanded then we should not handle button events.
  if ( !menu->menu()->expanded() )
    return false;

  // Initialize.
  bool handled ( true );

  // Process button states iff the menu is showing.
  switch ( id )
  {
    case COMMAND_MENU_SELECT:
      menu->selectFocused();
      break;

    case COMMAND_MENU_LEFT:
      menu->moveFocused ( MenuKit::Behavior::LEFT );
      break;

    case COMMAND_MENU_RIGHT:
      menu->moveFocused ( MenuKit::Behavior::RIGHT );
      break;

    case COMMAND_MENU_UP:
      menu->moveFocused ( MenuKit::Behavior::UP );
      break;

    case COMMAND_MENU_DOWN:
      menu->moveFocused ( MenuKit::Behavior::DOWN );
      break;

    default:
      handled = false;
  };

  // Return the result.
  return handled;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the button states and apply to the intersector.
//  Returns true if the event was handled.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::_handleIntersectionEvent( unsigned long id )
{
  ErrorChecker ( 2588614392u, isAppThread(), CV::NOT_APP_THREAD );

  if(!_intersector) return false;
#if 0
  // Process pressed states.
  if ( COMMAND_SELECT == id )
  {
    this->_intersect();
    return true;
  }
  
  else if ( COMMAND_HIDE_SELECTED == id )
  {
    this->_hideSelected ( MenuKit::MESSAGE_SELECTED, NULL );
    return true;
  }
  
  else if ( COMMAND_UNSELECT_VISIBLE == id )
  {
    this->_unselectVisible ( MenuKit::MESSAGE_SELECTED, NULL );
    return true;
  }
  
  else if ( COMMAND_SHOW_ALL == id )
  {
    this->_showAll ( MenuKit::MESSAGE_SELECTED, NULL );
    return true;
  }

  // Process released states.
  if ( COMMAND_SELECT == id )
  {
    this->_select();
    this->_updateSceneTool();
    return true;
  }
#endif
  // We didn't handle the event.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the button states and apply to Navigation mode selection.
//  Returns true if the event was handled.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::_handleNavigationEvent( unsigned long id )
{
  // Return if we are in intersect mode.
  if( _intersector.valid () )
    return false;

  bool handled ( true );

  switch ( id )
  {
  // Turn off all navigation.
  case NAVIGATE_NO_NAV:
    this->navigator ( 0x0 );
    break;

  default :
    handled = false;
  }

  return handled;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Callback for the joystick.
//
///////////////////////////////////////////////////////////////////////////////

void Application::JoystickCB::operator() ( VRV::Devices::Message m, Usul::Base::Referenced * )
{
  USUL_TRACE_SCOPE;

  ErrorChecker ( 1915253659u, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 4165917933u, 0x0 != _app );

  MenuKit::OSG::Menu *menu = _app->menu ();
  ErrorChecker ( 3990552070u, 0x0 != menu );

  switch ( m )
  {
  case VRV::Devices::JOYSTICK_ENTERING_RIGHT:
    menu->moveFocused ( MenuKit::Behavior::RIGHT );
    break;

  case VRV::Devices::JOYSTICK_ENTERING_LEFT:
    menu->moveFocused ( MenuKit::Behavior::LEFT );
    break;

  case VRV::Devices::JOYSTICK_ENTERING_UP:
    menu->moveFocused ( MenuKit::Behavior::UP );
    break;

  case VRV::Devices::JOYSTICK_ENTERING_DOWN:
    menu->moveFocused ( MenuKit::Behavior::DOWN );
    break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after the frame is drawn.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_postFrame()
{
  USUL_TRACE_SCOPE;

  ErrorChecker ( 1071439494, isAppThread(), CV::NOT_APP_THREAD );

  // Call the base class's function.
  BaseClass::_postFrame();

  // Initialize the text if we need to. We cannot call this sooner because 
  // contextInit() has to be called first.
  if ( false == Usul::Bits::has ( _flags, Detail::_TEXT_IS_INITIALIZED ) )
  {
    osg::ref_ptr < osg::FrameStamp > framestamp ( this->frameStamp() );
    if ( 0x0 != framestamp.get() && framestamp->getFrameNumber() > 10 )
      this->_initText();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Select the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_select()
{
  ErrorChecker ( 1070920309, isAppThread(), CV::NOT_APP_THREAD );

  // See if we have a selector...
  typedef CV::Pick::Select Select;
  Select::RefPtr selector ( dynamic_cast < Select * > ( _intersector.get() ) );
  if ( selector.valid() )
  {
    // Tell the selector to permanently "select" the intersected node.
    selector->select();

    // Report node name to user.
    const osg::Node *node = selector->node();
    if ( node )
      this->_updateStatusBar ( "Selected node: " + node->getName() );

    // Reset the selector.
    selector->reset();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Seek to the intersected polygon, if any.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_seek()
{
  ErrorChecker ( 1071204677, isAppThread(), CV::NOT_APP_THREAD );

  // See if we have a seeker...
  typedef CV::Pick::Seek Seek;
  Seek::RefPtr seeker ( dynamic_cast < Seek * > ( _intersector.get() ) );
  if ( seeker.valid() )
  {
    // Tell the seeker to seek.
    seeker->seek();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unselect the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_unselect()
{
  ErrorChecker ( 1070922810, isAppThread(), CV::NOT_APP_THREAD );

  // See if we have a selector...
  typedef CV::Pick::Select Select;
  Select::RefPtr selector ( dynamic_cast < Select * > ( _intersector.get() ) );
  if ( selector.valid() )
  {
    // Tell the selector to "unselect" the intersected node.
    selector->unselect();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the seek-intersector.
//
///////////////////////////////////////////////////////////////////////////////
#if 0
void Application::_setSeeker()
{
  ErrorChecker ( 1071204678, isAppThread(), CV::NOT_APP_THREAD );

  // Make a new selector.
  CV::Functors::Direction::Vector nz ( 0, 0, -1 );
  Unknown::ValidRefPtr unknown ( this->_thisUnknown() );
  CV::Functors::MatrixFunctor::ValidRefPtr wm ( new CV::Functors::WandRotation ( unknown ) );
  CV::Functors::Direction::ValidRefPtr nwz ( new CV::Functors::Direction ( unknown, nz, wm.get() ) );
  CV::Pick::Seek::ValidRefPtr seek ( new CV::Pick::Seek ( unknown, nwz.get(), INTERSECT_SEEK ) );

  // Call the other one.
  this->_setIntersector ( seek );
}
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Is this the application thread? The application thread is the one that 
//  calls init(), preFrame() and postFrame().
//
///////////////////////////////////////////////////////////////////////////////

bool Application::isAppThread()
{
  ErrorChecker ( 2104785486u, 0 != _mainThread );
  return ( Usul::Threads::currentThreadId() == _appThread );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this the main thread? The main thread is the original thread of 
//  execution that calls main().
//
///////////////////////////////////////////////////////////////////////////////

bool Application::isMainThread()
{
  ErrorChecker ( 1078956732u, 0 != _mainThread );
  return ( Usul::Threads::currentThreadId() == _mainThread );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Navigate if we are supposed to.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_navigate()
{
  USUL_TRACE_SCOPE;

  ErrorChecker ( 1068000936, isAppThread(), CV::NOT_APP_THREAD );

  Menu::RefPtr menu ( this->menu () );

  // If the menu is showing then we don't navigate.
  if ( menu.valid () && menu->menu()->expanded() )
    return;

  // If we have a valid tool then we don't navigate.
  if ( _sceneTool.valid() )
    return;

  // Call the base class
  BaseClass::_navigate ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Use the scene-tool if we are supposed to.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_useSceneTool()
{
  ErrorChecker ( 2860383896u, isAppThread(), CV::NOT_APP_THREAD );

  // If the menu is showing then we don't use the tool.
  if ( this->menu()->menu()->expanded() )
    return;

  // Tell it to execute.
  if ( _sceneTool.valid() )
    (*_sceneTool)();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect if we are supposed to.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_intersect()
{
  ErrorChecker ( 1069016548, isAppThread(), CV::NOT_APP_THREAD );

  // If the menu is showing then we don't use the tool.
  if ( this->menu()->menu()->expanded() )
    return;

  // If we have a valid intersector...
  if ( _intersector.valid() )
  {
    // Tell it to execute.
    (*_intersector)();

    // If it is a selector...
    CV::Pick::Select *selector = dynamic_cast < CV::Pick::Select * > ( _intersector.get() );
    if ( selector )
    {
      // Report node name to user.
      const osg::Node *node = selector->node();
      if ( node )
        this->_updateStatusBar ( "Intersected node: " + node->getName() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene holding the text.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_update ( OsgTools::Text &t, const std::string &s )
{
  // If this is the application thread...
  if ( isAppThread() )
  {
    // Change the text if it is different.
    if ( false == t.equal ( s ) )
      t.text ( s );
  }

  // Dump text to stdout if this is the message-text or the pick-text.
  if ( &t == _pickText.get() || &t == _msgText.get() )
    std::cout << s << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the auxiliary scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Group *Application::auxiliaryScene() const
{
  ErrorChecker ( 1069022674, _auxiliary.valid() );
  return _auxiliary.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the auxiliary scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Application::auxiliaryScene()
{
  ErrorChecker ( 1069022675, _auxiliary.valid() );
  return _auxiliary.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the center of rotation.
//
///////////////////////////////////////////////////////////////////////////////

void Application::rotationCenter ( const osg::Vec3 &rc )
{
  ErrorChecker ( 1071208595, isAppThread(), CV::NOT_APP_THREAD );
  _rotCenter = rc; // TODO, hook this up.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the frame-rate text.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_updateFrameRateDisplay()
{
  ErrorChecker ( 1071560060, isAppThread(), CV::NOT_APP_THREAD );

  // Make sure the frame-time is greater than zero.
  double ft ( this->frameTime() );
  if ( ft > 0 )
  {
    // Compose the string. Note: std::setprecision() doesn't 
    // work with std::ostringstream.
    const size_t size ( 128 );
    static char buffer[size];
    ::sprintf ( buffer, "Frame rate: %9.6f Hz", 1.0 / ft );

    // Set the text.
    this->_update ( *_frameText, std::string ( buffer ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the analog-input text.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_updateAnalogText()
{
  ErrorChecker ( 1071563205, isAppThread(), CV::NOT_APP_THREAD );

  // Compose the string. Note: std::setprecision() doesn't 
  // work with std::ostringstream.
  const size_t size ( 128 );
  static char buffer[size];
  float h ( this->joystickHorizontal() );
  float v ( this->joystickVertical() );
  ::sprintf ( buffer, "Joystick: %9.6f %9.6f", h, v );

  // Set the text.
  this->_update ( *_navText, std::string ( buffer ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference.
//
///////////////////////////////////////////////////////////////////////////////

void Application::ref()
{
  BaseClass::ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference.
//
///////////////////////////////////////////////////////////////////////////////

void Application::unref ( bool allowDeletion )
{
  BaseClass::unref( allowDeletion );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Application::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case CV::Interfaces::IAuxiliaryScene::IID:
    return static_cast<CV::Interfaces::IAuxiliaryScene *>(this);
  case CV::Interfaces::IVisibility::IID:
    return _iVisibility.get();
  case CV::Interfaces::ISelection::IID:
    return _iSelection.get();
  case CV::Interfaces::IMaterialStack::IID:
    return _iMaterialStack.get();
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns the number of selected
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Application::_numSelected ()
{
  ErrorChecker ( 1084582713u, isAppThread(), CV::NOT_APP_THREAD );

  CV::Functors::Tool::Transforms t;
  this->_selected( t );
  return t.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Fill the given vector with the selected matrix-transforms.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_selected ( CV::Functors::Tool::Transforms &vt )
{
  ErrorChecker ( 1227492927u, isAppThread(), CV::NOT_APP_THREAD );

  // For readability.
  typedef CV::Functors::Tool::TransformPtr TransformPtr;
  typedef osg::MatrixTransform MT;
  typedef CV::OSG::Predicates::IsSelected IsSelected;
  typedef CV::OSG::Predicates::IsVisible IsVisible;
  typedef Usul::Predicates::UnaryPair<IsVisible,std::logical_and<bool>,IsSelected> IsWanted;
  typedef CV::OSG::Functors::Append<MT> Append;
  typedef Usul::Functors::General::IfThen<IsSelected,Append> IfThen;
  //  typedef Usul::Functors::General::IfThen<IsWanted,Append> IfThen;
  typedef OsgTools::Visitor<MT,IfThen> Visitor;

  if( false == _iVisibility.valid() || false == _iSelection.valid () )
    return;

  // Compose the functors. For some reason I need to declare all the functors 
  // as variables, rather than pass, for example, IsSelected().
  IsSelected isSelected ( _iSelection );
  IsVisible isVisible ( _iVisibility );

  // TODO: correct UnaryPair error, use result in IfThen
  //std::logical_and<bool> operation;
  //Usul::Predicates::UnaryPair<IsVisible,std::logical_and<bool>,IsSelected> isWanted(isVisible,
  //                                                                                  operation,
  //                                                                                  isSelected);
  //IfThen ifThen ( isWanted, Append() );
  IfThen ifThen( isSelected , Append() );

  // Make the visitor.
  Visitor::Ptr visitor ( new Visitor ( ifThen ) );

  // Traverse the models-scene and append selected nodes.
  this->models()->accept ( *visitor );

  // Set the given vector.
  Append &append = visitor->op().then();
  vt.resize ( append.size() );
  std::copy ( append.begin(), append.end(), vt.begin() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene-tool, if any.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_updateSceneTool()
{
  ErrorChecker ( 2346088799u, isAppThread(), CV::NOT_APP_THREAD );

  // If we have a scene functor...
  if ( _sceneTool.valid() )
  {
    // See if it is really a tool...
    {
      typedef CV::Functors::Tool Tool;
      Tool::RefPtr tool ( dynamic_cast < Tool * > ( _sceneTool.get() ) );
      if ( tool.valid() )
      {
        // Update the tool's list of selected nodes.
        this->_selected ( tool->transforms() );
        return;
      }
    }

    // It could be a pair...
    {
      typedef CV::Functors::ToolPair Tool;
      Tool::RefPtr tool ( dynamic_cast < Tool * > ( _sceneTool.get() ) );
      if ( tool.valid() )
      {
        // Update the tool's list of selected nodes.
        Tool::Transforms tr;
        this->_selected ( tr );
        tool->transforms ( tr );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Only notify if the menu is not shown..
//
///////////////////////////////////////////////////////////////////////////////

void Application::_updateNotify()
{
  MenuPtr m ( this->menu() );
  if ( true == m.valid() && false == m->isVisible() )
  {
    BaseClass::_updateNotify();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The active document has changed.
//
///////////////////////////////////////////////////////////////////////////////

void Application::activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc )
{
  // Redirect first.
  BaseClass::activeDocumentChanged ( oldDoc, newDoc );

  // Rebuild the menu.
  this->_initMenu();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when button is pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Application::buttonPressNotify ( Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;

  // Redirect.
  BaseClass::buttonPressNotify ( caller );

  // Get the button id.
  Usul::Interfaces::IButtonID::QueryPtr button ( caller );
  if ( button.valid () )
  {
    unsigned long id ( button->buttonID () );

    switch ( id )
    {
    case VRV::BUTTON0: std::cout << VRV::BUTTON0 << " Button 0 pressed (YELLOW)"   << std::endl; break;
    case VRV::BUTTON1: std::cout << VRV::BUTTON1 << " Button 1 pressed (RED)"      << std::endl; break;
    case VRV::BUTTON2: std::cout << VRV::BUTTON2 << " Button 2 pressed (GREEN)"    << std::endl; break;
    case VRV::BUTTON3: std::cout << VRV::BUTTON3 << " Button 3 pressed (BLUE)"     << std::endl; break;
    case VRV::BUTTON4: std::cout << VRV::BUTTON4 << " Button 4 pressed (JOYSTICK)" << std::endl; break;
    case VRV::BUTTON5: std::cout << VRV::BUTTON5 << " Button 5 pressed (TRIGGER)"  << std::endl; break;
    }

    // Let the menu process first.
    bool menuHandled ( this->_handleMenuEvent( id ) );

    // Hide the scene if we are suppose to and it's currently visible.
    bool hideScene ( this->menuSceneShowHide() && this->menu()->isVisible() );

    // The node mask.
    unsigned int mask ( hideScene ? 0 : 0xffffffff );

    // Always set the mask.
    this->modelsScene ( )->setNodeMask ( mask );

    // Return now if the menu was handled.
    if ( menuHandled )
      return;

    // Now process the intersector buttons.
    if ( this->_handleIntersectionEvent( id ) )
      return;

    // Handle the navigation mode.
    if ( this->_handleNavigationEvent( id ) )
      return;

    if ( VRV::BUTTON5 == id )
    {
      this->_readUserPreferences();
      this->_readFunctorFile();
      this->_initMenu();
      this->_initLight();
      this->_initStatusBar();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when button is released.
//
///////////////////////////////////////////////////////////////////////////////

void Application::buttonReleaseNotify ( Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;

  // Redirect.
  BaseClass::buttonReleaseNotify ( caller );

  // Get the button id.
  Usul::Interfaces::IButtonID::QueryPtr button ( caller );
  if ( button.valid () )
  {
    unsigned long id ( button->buttonID () );
  }
}
