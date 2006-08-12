
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
#include "JugglerFunctors.h"
#include "Exceptions.h"
#include "ConfigFiles.h"
#include "SceneFunctors.h"
#include "ScenePredicates.h"

#include "CadViewer/Interfaces/IPostModelLoad.h"

#include "CadViewer/Functors/WandMatrix.h"
#include "CadViewer/Functors/WandRotation.h"
#include "CadViewer/Functors/ToolPair.h"

#include "CadViewer/Pick/Select.h"
#include "CadViewer/Pick/Seek.h"

#include "VRV/Interfaces/IParseRestart.h"
#include "VRV/Interfaces/IMenuRead.h"
#include "VRV/Interfaces/IMenuGet.h"

#include "SAL/Interfaces/INode.h"
#include "SAL/Interfaces/IOSG.h"
#include "SAL/Interfaces/IRead.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Components/Object.h"
#include "Usul/Print/Vector.h"
#include "Usul/Print/Matrix.h"
#include "Usul/Math/Constants.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/System/Host.h"
#include "Usul/Interfaces/IRead.h"
#include "Usul/Functors/If.h"
#include "Usul/Predicates/UnaryPair.h"

#include "vrj/Kernel/Kernel.h"
#include "vrj/Display/Projection.h"

#include "vrjGA/ButtonGroup.h"
#include "vrjGA/TrackerDevice.h"

#include "osg/MatrixTransform"
#include "osg/AutoTransform"
#include "osg/Matrix"
#include "osg/LightSource"
#include "osg/Projection"
#include "osg/Version"
#include "osg/AnimationPath"
#include "osgFX/Scribe"

#include "osgDB/ReadFile"
#include "osgDB/WriteFile"
#include "osgDB/ReaderWriter"

#include "OsgTools/Axes.h"
#include "OsgTools/Text.h"
#include "OsgTools/Group.h"
#include "OsgTools/Convert.h"
#include "OsgTools/Font.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Visitor.h"

#include "MenuKit/MemFunCallback.h"

#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////
//
//  Button mappings.
//
///////////////////////////////////////////////////////////////////////////////

namespace CV
{
  // Buttons.
  const unsigned long BUTTON0 = 0x00000001;
  const unsigned long BUTTON1 = 0x00000002;
  const unsigned long BUTTON2 = 0x00000004;
  const unsigned long BUTTON3 = 0x00000008;
  const unsigned long BUTTON4 = 0x00000010;
  const unsigned long BUTTON5 = 0x00000020;

  // For convenience.
  const unsigned long BUTTON_RED      = CV::BUTTON1;
  const unsigned long BUTTON_YELLOW   = CV::BUTTON0;
  const unsigned long BUTTON_GREEN    = CV::BUTTON2;
  const unsigned long BUTTON_BLUE     = CV::BUTTON3;
  const unsigned long BUTTON_JOYSTICK = CV::BUTTON4;
  const unsigned long BUTTON_TRIGGER  = CV::BUTTON5;

  // Button combinations.
/*#if 0
  const unsigned long COMMAND_QUIT             = BUTTON_RED | BUTTON_YELLOW | BUTTON_GREEN;
  const unsigned long COMMAND_RESET_WORLD      = BUTTON_RED | BUTTON_BLUE;
  const unsigned long COMMAND_NAVIGATION       = BUTTON_JOYSTICK;
  const unsigned long COMMAND_SEEK             = 0xffffffff; // Disable for now.
  const unsigned long COMMAND_ANALOG_TRIM      = BUTTON_RED | BUTTON_GREEN;
  const unsigned long COMMAND_HIDE_SELECTED    = BUTTON_YELLOW;
  const unsigned long COMMAND_UNSELECT_VISIBLE = BUTTON_GREEN;
  const unsigned long COMMAND_SHOW_ALL         = BUTTON_RED;
#else
  const unsigned long COMMAND_MENU_TOGGLE      = BUTTON_JOYSTICK;
  const unsigned long COMMAND_MENU_SELECT      = BUTTON_TRIGGER;
  const unsigned long COMMAND_MENU_LEFT        = BUTTON_RED;
  const unsigned long COMMAND_MENU_RIGHT       = BUTTON_BLUE;
  const unsigned long COMMAND_MENU_UP          = BUTTON_YELLOW;
  const unsigned long COMMAND_MENU_DOWN        = BUTTON_GREEN;
  const unsigned long COMMAND_SELECT           = BUTTON_TRIGGER;
#endif*/
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
#if defined (INV3RSION_NAV)
  const unsigned long INVR_TRANSLATE  = BUTTON_YELLOW;
  const unsigned long INVR_ROTATE     = BUTTON_RED;
  const unsigned long INVR_SCALE      = BUTTON_GREEN;
  const unsigned long NAVIGATE_FLY    = BUTTON_BLUE;
  const unsigned long INVR_NAV_RESET  = BUTTON_YELLOW | BUTTON_RED;
#else
  const unsigned long NAVIGATE_TOGGLE = BUTTON_YELLOW;
  const unsigned long STOP_NAV_TOOL   = BUTTON_RED;       // helps greatly in sim mode
  const unsigned long NAVIGATE_FLY    = BUTTON_BLUE;      // Joseph doesn't need it, but its handy if anybody else does
  const unsigned long TOOL_SCALE      = BUTTON_GREEN;
#endif
  const unsigned long NAVIGATE_NO_NAV = 0x00000040;
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
//  Define component names so that we can change them in one place.
//
///////////////////////////////////////////////////////////////////////////////

#define CV_SCENE_OPERATIONS         "Scene Operations"
#define CV_COLLISION_DETECTION      "Collision Detection"
#define CV_SCENE_ABSTRACTION_LAYER  "Scene Abstraction Layer"
#define CV_RESTART_FILE_PARSER      "Restart File Parser"
#define CV_GRAPHICAL_USER_INTERFACE "Graphical User Interface"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application ( Args &args ) :
  BaseClass       ( vrj::Kernel::instance() ),
  _parser         ( new Parser ( args.begin(), args.end() ) ),
  _root           ( new osg::Group ),
  _navBranch      ( new osg::MatrixTransform ),
  _models         ( new osg::MatrixTransform ),
  _gridBranch     ( new osg::MatrixTransform ),
  _cursor         ( new osg::MatrixTransform ),
  _cursorActiveWithRot  ( new osg::MatrixTransform ),
  _cursorRedWithRot     ( new osg::MatrixTransform ),
  _cursorYellowWithRot  ( new osg::MatrixTransform ),
  _cursorGreenWithRot   ( new osg::MatrixTransform ),
  _cursorBlueWithRot    ( new osg::MatrixTransform ),
  _cursorTriggerWithRot ( new osg::MatrixTransform ),
  _cursorActiveNoRot    ( new osg::MatrixTransform ),
  _cursorRedNoRot       ( new osg::MatrixTransform ),
  _cursorYellowNoRot    ( new osg::MatrixTransform ),
  _cursorGreenNoRot     ( new osg::MatrixTransform ),
  _cursorBlueNoRot      ( new osg::MatrixTransform ),
  _cursorTriggerNoRot   ( new osg::MatrixTransform ),
  _cursor_zoom          ( osgDB::readNodeFile( "/home/users/hansenp/models/Cursors/zoom_in.flt" ) ),
  _menuBranch     ( new osg::MatrixTransform ),
  _statusBranch   ( new osg::MatrixTransform ),
  _origin         ( new osg::Group ),
  _auxiliary      ( new osg::Group ),
  _textBranch     ( new osg::Projection ),
  _navigatorH     ( 0x0 ),
  _navigatorV     ( 0x0 ),
  _sceneTool      ( 0x0 ),
  _intersector    ( 0x0 ),
  _buttons        ( new vrjGA::ButtonGroup ),
  _tracker        ( new vrjGA::TrackerDevice ( "VJWand" ) ),
  _joystick       ( new vrjGA::JoystickDevice ( "VJAnalog0", "VJAnalog1" ) ),
  _analogTrim     ( 0, 0 ),
  _rotCenter      ( 0, 0, 0 ),
  _pickText       ( new OsgTools::Text ),
  _navText        ( new OsgTools::Text ),
  _frameText      ( new OsgTools::Text ),
  _msgText        ( new OsgTools::Text ),
  _vp             ( 0, 0, 200, 200 ),
  _flags          ( 0 ),
  _wandOffset     ( 0, 0, 0 ), // feet (used to be z=-4)
  _frameTime      ( 1 ), // Not zero!
  _cursorMatrix   ( 0x0 ),
  _sceneMutex     (),
  _iVisibility    ( Usul::Components::Object::create ( CV::Interfaces::IVisibility::IID,
                                                       CV_SCENE_OPERATIONS ) ),
  _iSelection     ( Usul::Components::Object::create ( CV::Interfaces::ISelection::IID,
                                                       CV_SCENE_OPERATIONS ) ),
  _iMaterialStack ( Usul::Components::Object::create ( CV::Interfaces::IMaterialStack::IID,
                                                       CV_SCENE_OPERATIONS ) ),
  _iCollider      ( Usul::Components::Object::create ( Collision::Interfaces::ICollider::IID,
                                                       CV_COLLISION_DETECTION,
                                                       false,
                                                       false ) ),
  _clipDist       ( 0, 0 ),
  _menu           ( new MenuKit::OSG::Menu() ),
  _statusBar      ( new MenuKit::OSG::Menu() ),
  _buttonMap      (),
  _prefs          ( new VRV::Prefs::Settings ),
  _home           ( osg::Matrixf::identity() ),
  _colorMap       (),
  _textures       ( true ),
  _scribeBranch   ( new osg::MatrixTransform ),
  _autoPlacement  ( false ),
  _animations     ( true ),
  _anim_steps     ( 0 ),
  _animModel      ( NULL ),
  _nextFrameTime  ( 0.0 )
#if defined (INV3RSION_NAV)
  , _invrNav( new invr::nav::CAD(&_tracker->matrix()) )
#endif
#if defined (INV3RSION_COLLABORATE)
  , _avatarFactory    ( NULL )
  , _localAvatar      ( NULL )
  , _localAvatarName  ( "NULL" )
  , _localAvatarFileName ( "NULL" )
  , _controlAvatar    ( NULL )
  , _headTracker      ( new vrjGA::TrackerDevice ( "VJHead" ) )
  , _bodyMaxYawRate   ( 0.001 )
  , _avatarWaitCount  ( 0 )
  , _headYawOffset    ( 0 )
  , _prevHeadYaw      ( 0 )
#endif
{
  ErrorChecker ( 1067097070u, 0 == _appThread );
  ErrorChecker ( 2970484549u, 0 == _mainThread );
  ErrorChecker ( 1074058928u, 0x0 != _parser.get() );
  ErrorChecker ( 1067094626u, _root.valid() );
  ErrorChecker ( 1067222084u, _navBranch.valid() );
  ErrorChecker ( 1067094627u, _models.valid() );
  ErrorChecker ( 1067094628u, _gridBranch.valid() );
  ErrorChecker ( 1067094631u, _cursor.valid() );
  ErrorChecker ( 1325879383u, _menuBranch.valid() );
  ErrorChecker ( 3423749732u, _statusBranch.valid() );
  ErrorChecker ( 1068249416u, _origin.valid() );
  ErrorChecker ( 1069021589u, _auxiliary.valid() );
  ErrorChecker ( 1071446158u, _textBranch.valid() );
  ErrorChecker ( 1067094629u, _scribeBranch.valid() );
  ErrorChecker ( 1071551353u, 0x0 != _pickText.get() );
  ErrorChecker ( 1071551354u, 0x0 != _navText.get() );
  ErrorChecker ( 1071551355u, 0x0 != _frameText.get() );
  ErrorChecker ( 1071551356u, 0x0 != _msgText.get() );

  // Initialize the main thread.
  _mainThread = Usul::Threads::currentThreadId();

  // Read the user's preference file, if any.
  this->_readUserPreferences();

  // Did we load a collider?
  WarningChecker ( 1082589164u, _iCollider.valid(), "Failed to load collider module." );

  // Hook up the branches.
  _root->addChild      ( _cursor.get()       );
  _root->addChild      ( _menuBranch.get()   );
  _root->addChild      ( _statusBranch.get() );
  _root->addChild      ( _origin.get()       );
  _root->addChild      ( _auxiliary.get()    );
  _root->addChild      ( _textBranch.get()   );
  _root->addChild      ( _navBranch.get()    );
  _navBranch->addChild ( _models.get()       );
  _navBranch->addChild ( _gridBranch.get()   );
  _navBranch->addChild ( _scribeBranch.get() );

  // Name the branches.
  _root->setName         ( "_root"         );
  _navBranch->setName    ( "_navBranch"    );
  _models->setName       ( "_models"       );
  _gridBranch->setName   ( "_gridBranch"   );
  _cursor->setName       ( "_cursor"       );
  _menuBranch->setName   ( "_menuBranch"   );
  _statusBranch->setName ( "_statusBranch" );
  _origin->setName       ( "_origin"       );
  _auxiliary->setName    ( "_auxiliary"    );
  _textBranch->setName   ( "_textBranch"   );
  _scribeBranch->setName ( "_scribeBranch" );

  // Hook up the joystick callbacks.
  USUL_VALID_REF_POINTER(JoystickCB) jcb ( new JoystickCB ( this ) );
  _joystick->callback ( vrjGA::JOYSTICK_ENTERING_RIGHT, jcb.get() );
  _joystick->callback ( vrjGA::JOYSTICK_ENTERING_LEFT,  jcb.get() );
  _joystick->callback ( vrjGA::JOYSTICK_ENTERING_UP,    jcb.get() );
  _joystick->callback ( vrjGA::JOYSTICK_ENTERING_DOWN,  jcb.get() );

  // Have to load the config files now. Remove them from the arguments.
  Parser::Args configs = _parser->files ( ".jconf", true );
  this->_loadConfigFiles ( configs );

  // populate the color map
  _colorMap["red"]    = osg::Vec4 ( 1.0,0.0,0.0,1.0);
  _colorMap["yellow"] = osg::Vec4 ( 1.0,1.0,0.0,1.0);
  _colorMap["green"]  = osg::Vec4 ( 0.0,1.0,0.0,1.0);
  _colorMap["teal"]   = osg::Vec4 ( 0.0,1.0,1.0,1.0);
  _colorMap["blue"]   = osg::Vec4 ( 0.0,0.0,1.0,1.0);
  _colorMap["white"]  = osg::Vec4 ( 1.0,1.0,1.0,1.0);
  _colorMap["grey"]   = osg::Vec4 ( 0.5,0.5,0.5,1.0);
  _colorMap["black"]  = osg::Vec4 ( 0.0,0.0,0.0,1.0);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
  unsigned int i;
  
  for( i=0; i<_gridFunctors.size(); ++i){
    delete _gridFunctors[i];
  }
  _gridFunctors.clear();

#if defined (INV3RSION_NAV)
  if(_invrNav) delete _invrNav;
# endif

#if defined (INV3RSION_COLLABORATE)
  if(_localAvatar)
  {
    std::string cmd = "CV AVATAR_DELETE = ";
    cmd += _localAvatar->name;
    _sinterSendCommand(cmd, true);
  }
#endif

# if defined (USE_SINTERPOINT)
  if( _sinterReceiver )
  {
    _sinterReceiver->Disconnect();
    delete _sinterReceiver;
  }
# endif
#if defined (INV3RSION_COLLABORATE)
  if( _sinterCollabSender )
  {
    _sinterCollabSender->Disconnect();
    delete _sinterCollabSender;
  }
  
  if(_sinterCollabReceiver)
  {
    _sinterCollabReceiver->Disconnect();
    delete _sinterCollabReceiver;
  }
  
  for( i = 0; i < _avatars.size(); i++)
  {
    delete _avatars[i]->avatar;
    delete _avatars[i];
  }
  _avatars.clear();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called once for each display to initialize the OpenGL context.
//
///////////////////////////////////////////////////////////////////////////////

void Application::contextInit()
{
  ErrorChecker ( 1071202040u, !isAppThread() );

  // Call the base class's function first.
  BaseClass::contextInit();

  // Get the viewport. This assumes all displays are the same size.
  GLint vp[4];
  ::glGetIntegerv ( GL_VIEWPORT, vp );
  _vp.set ( vp[0], vp[1], vp[2], vp[3] );

#if defined (INV3RSION_NAV)
  _invrNav->contextInit();
#endif

#if defined (INV3RSION_COLLABORATE)
  if(_avatarFactory) _avatarFactory->contextInit();
  _avatarTime = _tracker->time();
#endif

}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Application::init()
{
  bool stop ( false );

  try
  {
    this->_init();
  }

  catch ( const std::exception &e )
  {
    std::cout << "Error 1082603967: "
              << "Exception generated while initializing."
              << "\n\tWhat: " << e.what()
              << std::endl;
    stop = true;
  }
#if 1
  catch ( ... )
  {
    std::cout << "Error 1082603859: "
              << "Unknown exception generated while initializing." 
              << std::endl;
    stop = true;
  }
#endif
  // Are you supposed to stop?
  if ( stop )
    vrj::Kernel::instance()->stop();
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
  ErrorChecker ( 1067093507, _root.valid() );
  ErrorChecker ( 1067093508, _models.valid() );

  // Initialize the application thread.
  _appThread = Usul::Threads::currentThreadId();

  // Call the base class's function first.
  BaseClass::init();
  
  // Set the global GL_NORMALIZE flag.
  this->normalize ( _prefs->normalizeVertexNormalsGlobal() );
  
  // Set Auto-Placement flag
  _autoPlacement = _prefs->autoPlacement();
  
  // Create & clear tmp subdirectory
  _initTmpDir();
  
  // Set the background color.
  const Preferences::Vec4f &bc = _prefs->backgroundColor();
  this->setBackgroundColor ( osg::Vec4 ( bc[0], bc[1], bc[2], bc[3] ) );

  // Initialize the button group by adding the individual buttons.
  _buttons->add ( new vrjGA::ButtonDevice ( CV::BUTTON0, "VJButton0" ) );
  _buttons->add ( new vrjGA::ButtonDevice ( CV::BUTTON1, "VJButton1" ) );
  _buttons->add ( new vrjGA::ButtonDevice ( CV::BUTTON2, "VJButton2" ) );
  _buttons->add ( new vrjGA::ButtonDevice ( CV::BUTTON3, "VJButton3" ) );
  _buttons->add ( new vrjGA::ButtonDevice ( CV::BUTTON4, "VJButton4" ) );
  _buttons->add ( new vrjGA::ButtonDevice ( CV::BUTTON5, "VJButton5" ) );

  // Set up lights.
  this->_initLight();

  // Set the scene-viewer's scene.
  this->setSceneData ( _root.get() );

  // Setup Sinterpoint if enabled
#if defined (USE_SINTERPOINT)
  this->_sinterPointInit();
#endif

  // Parse the command-line arguments.
  this->_parseCommandLine();

  // Move the model-group so that the models are centered and visible.
  this->viewAll ( _models.get(), _prefs->viewAllScaleZ() );
  this->viewAll ( _scribeBranch.get(), _prefs->viewAllScaleZ() );

  // Now that the models are centered, draw a grid in proportion to the size 
  // of the models.
  this->_initGrid ( _models.get() );

  // Save the "home" position.
  this->_setHome();

  // Set a default cursor matrix functor.
  //this->_setCursorMatrixFunctor ( new CV::Functors::WandMatrix ( this->_thisUnknown() ) );

  // Based on the scene size, set the near and far clipping plane distances.
  this->_setNearAndFarClippingPlanes();

  // Initialize the menu.
  this->_initMenu();

  // Initialize the status-bar.
  this->_initStatusBar();

  // Turn on navigation.
  //this->_setNavigator(); // This breaks time-based navigation. TODO, fix.
  this->_handleNavigationEvent( NAVIGATE_NO_NAV );   // activate default navigation
                                                          // TODO: read from _pref
  // Note: we cannot initialize the text yet because the viewport has not been set.

#if defined (INV3RSION_NAV)
  _invrNav->init();
  _invrNav->SetIconColor(_prefs->iconColor()[0], 
                         _prefs->iconColor()[1], 
                         _prefs->iconColor()[2], 
                         _prefs->iconColor()[3]);
  _invrNav->SetIconSize(_prefs->iconSize());
  _invrNav->SetTranslationAccel(_prefs->acceleration());
  _invrNav->SetInvertRotation(_prefs->invertRotation());
#endif
#if defined (INV3RSION_COLLABORATE)
  if( _localAvatarName != "NULL" && _localAvatarFileName != "NULL" )
    _sendAddAvatarCommand( _localAvatarFileName, _localAvatarName);
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the text.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initText()
{
  ErrorChecker ( 1071444744, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 1071444745, _textBranch.valid() );
  ErrorChecker ( 1071444746, 0x0 != _pickText.get() );
  ErrorChecker ( 1071444747, 0x0 != _navText.get() );
  ErrorChecker ( 1071444748, 0x0 != _frameText.get() );
  ErrorChecker ( 1071444749, 0x0 != _msgText.get() );
  ErrorChecker ( 1071556909, false == Usul::Bits::has ( _flags, Detail::_TEXT_IS_INITIALIZED ) );
  ErrorChecker ( 1071558814, 0 == _textBranch->getNumChildren() );

  // Create a matrix-transform relative to the global coordinate system.
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  ErrorChecker ( 1071452071, mt.valid() );

#ifdef _MSC_VER
  mt->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );
#else
  mt->setReferenceFrame ( osg::Transform::RELATIVE_RF );
#endif
 
  mt->setMatrix ( osg::Matrix::identity() );

  // Make the text branch an orthographic projection.
  float x ( _vp[0] );
  float y ( _vp[1] );
  float w ( _vp[2] );
  float h ( _vp[3] );
  _textBranch->setMatrix ( osg::Matrix::ortho2D ( x, w, y, h ) );

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
  _textBranch->addChild ( mt.get() );

#if 0 // Use for debugging their placement.
  _navText->text   ( "_navText"   );
  _msgText->text   ( "_msgText"   );
  _frameText->text ( "_frameText" );
  _pickText->text  ( "_pickText"  );
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
  int i;

  // Get the bounding sphere of the node.
  const osg::BoundingSphere &bs = node->getBound();

  // Handle zero-sized bounding spheres.
  float r = ( bs.radius() <= 1e-6 ) ? 1 : bs.radius();

  // Clean up any old grids
  for(unsigned int i=0; i<_gridFunctors.size(); ++i){
    delete _gridFunctors[i];
  }
  _gridFunctors.clear();

  // Set the properties.
  for(i=0; i<_prefs->numGrids(); ++i){
    OsgTools::Grid *grid = new OsgTools::Grid();
    grid->numBlocks ( _prefs->numGridBlocks(i) );
    grid->size ( r * _prefs->gridScale(i) );
    grid->color ( _prefs->gridColor(i) );
    grid->fillColor ( _prefs->gridFillColor(i) );
    Usul::Math::Matrix44f o;
    o.makeRotation ( _prefs->gridRotationAngleRad(i), _prefs->gridRotationVector(i) );
    grid->orientation ( o );

    // Move the center so that it is below the bounding sphere of the node.
    osg::Vec3 c ( bs.center() );
    if(_prefs->offsetGrid(i)) c[1] = -r;
    grid->center ( Usul::Math::Vec3f ( c[0], c[1], c[2] ) );
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
  for(unsigned int i=0; i<_gridFunctors.size(); ++i){
    _gridBranch->addChild ( (*(_gridFunctors[i]))() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the light.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initLight()
{
  ErrorChecker ( 1067093691, isAppThread(), CV::NOT_APP_THREAD );

  osg::ref_ptr<osg::LightSource> source ( new osg::LightSource );
  osg::ref_ptr<osg::Light> light ( new osg::Light );
  osg::Vec3 ld;
  osg::Vec4 lp;
  int i;
  for(i=0; i<4; ++i){
    lp[i]=_prefs->lightPosition()[i];
  }
  for(i=0; i<3; ++i){
    ld[i]=_prefs->lightDirection()[i];
  }
  light->setPosition( lp );
  light->setDirection( ld );

  source->setLight ( light.get() );
  source->setLocalStateSetModes ( osg::StateAttribute::ON );

  this->setSceneDecorator ( source.get() );

  const char *home = ::getenv ( "HOME" );
  if ( home )
  {
    std::ostringstream filename;
    filename << home << "/.cadviewer/lighting.txt";
    std::ifstream in ( filename.str().c_str() );
    if ( in )
    {
      osg::Vec4 ambient, diffuse, specular;
      in >>  ambient[0] >>  ambient[1] >>  ambient[2] >>  ambient[3];
      in >>  diffuse[0] >>  diffuse[1] >>  diffuse[2] >>  diffuse[3];
      in >> specular[0] >> specular[1] >> specular[2] >> specular[3];
      light->setAmbient  ( ambient );
      light->setDiffuse  ( diffuse );
      light->setSpecular ( specular );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to register callbacks in the map.
//
///////////////////////////////////////////////////////////////////////////////

#define CV_REGISTER(member_function,name) \
  _buttonMap[name] = MenuKit::memFunCB2 \
    ( this, &Application::member_function )


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the menu.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initMenu()
{
  ErrorChecker ( 1155745824u, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 2049202602u, _menu.valid() );
  typedef USUL_VALID_REF_POINTER(MenuKit::Menu) ValidMenu;
  typedef USUL_VALID_REF_POINTER(osg::StateSet) StateSet;
  
  // Set menu's background and text colors from preferences.xml stored in VRV::Prefs::Settings _prefs
  const float* BNarrayf = _prefs->menuBgColorNorm().get();
  _menu->skin()->bg_color_normal( osg::Vec4(BNarrayf[0], BNarrayf[1], BNarrayf[2], BNarrayf[3]) );
  const float* BHarrayf = _prefs->menuBgColorHLght().get();
  _menu->skin()->bg_color_highlight( osg::Vec4(BHarrayf[0], BHarrayf[1], BHarrayf[2], BHarrayf[3]) );
  const float* BDarrayf = _prefs->menuBgColorDsabl().get();
  _menu->skin()->bg_color_disabled( osg::Vec4(BDarrayf[0], BDarrayf[1], BDarrayf[2], BDarrayf[3]) );
  const float* TNarrayf = _prefs->menuTxtColorNorm().get();
  _menu->skin()->text_color_normal( osg::Vec4(TNarrayf[0], TNarrayf[1], TNarrayf[2], TNarrayf[3]) );
  const float* THarrayf = _prefs->menuTxtColorHLght().get();
  _menu->skin()->text_color_highlight( osg::Vec4(THarrayf[0], THarrayf[1], THarrayf[2], THarrayf[3]) );
  const float* TDarrayf = _prefs->menuTxtColorDsabl().get();
  _menu->skin()->text_color_disabled( osg::Vec4(TDarrayf[0], TDarrayf[1], TDarrayf[2], TDarrayf[3]) );

  // Set the menu scene.
  osg::Matrixf mm;
  OsgTools::Convert::matrix ( _prefs->menuMatrix(), mm );
  _menuBranch->setMatrix ( mm );
  _menu->scene ( _menuBranch.get() );

  // Make the menu always draw on top (last). See osgfxbrowser.cpp.
	StateSet ss ( _menuBranch->getOrCreateStateSet() );
	ss->setRenderBinDetails ( 100, "RenderBin" );
	ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );

  // take lighting off of the menu
  ss->setMode( GL_LIGHTING , osg::StateAttribute::OFF );

  // set the stateset
  _menuBranch->setStateSet( ss.get() );


  #define CV_REGISTER(member_function,name) \
  _buttonMap[name] = MenuKit::memFunCB2 \
    ( this, &Application::member_function )
  // Fill the callback map.
  CV_REGISTER ( _quitCallback,     "exit" );    // macro expands to ...
      // _buttonmap["exit"] = MenuKit::memFunCB2 ( this, &Application::_quitCallback ) 
  CV_REGISTER ( _quitCallback,     "quit" );    // macro expands to ...
      // _buttonmap["quit"] = MenuKit::memFunCB2 ( this, &Application::_quitCallback )
  CV_REGISTER ( _hideSelected,     "hide_selected" );    // macro expands to ...
      // _buttonmap["hide_selected"] = MenuKit::memFunCB2 ( this, &Application::_hideSelected )
  CV_REGISTER ( _showAll,          "show_all" );
  CV_REGISTER ( _unselectVisible,  "unselect_visible" );
  CV_REGISTER ( _exportSelected,   "export_selected" );
  CV_REGISTER ( _exportWorld,      "export_world" );
  CV_REGISTER ( _exportScene,      "export_scene" );
  CV_REGISTER ( _normScene,        "normalize_scene" );
  CV_REGISTER ( _normSelected,     "normalize_selected" );
  CV_REGISTER ( _hTransWandPosX,   "horizontal_translate_wand_positive_x" );
  CV_REGISTER ( _hTransWandNegX,   "horizontal_translate_wand_negative_x" );
  CV_REGISTER ( _vTransWandPosY,   "vertical_translate_wand_positive_y" );
  CV_REGISTER ( _vTransWandPosZ,   "vertical_translate_wand_positive_z" );
  CV_REGISTER ( _vTransWandNegZ,   "vertical_translate_wand_negative_z" );
  CV_REGISTER ( _hTransGlobalPosX, "horizontal_translate_global_positive_x" );
  CV_REGISTER ( _hTransGlobalNegX, "horizontal_translate_global_negative_x" );
  CV_REGISTER ( _vTransGlobalPosY, "vertical_translate_global_positive_y" );
  CV_REGISTER ( _vTransGlobalPosZ, "vertical_translate_global_positive_z" );
  CV_REGISTER ( _vTransGlobalNegZ, "vertical_translate_global_negative_z" );
  CV_REGISTER ( _hRotWandPosY,     "horizontal_rotate_wand_clockwise_y" );
  CV_REGISTER ( _hRotGlobalPosY,   "horizontal_rotate_global_clockwise_y" );
  CV_REGISTER ( _vScaleWorld,      "vertical_scale_world" );
  CV_REGISTER ( _vScaleSelected,   "vertical_scale_selected" );
  CV_REGISTER ( _wMoveSelLocal,    "move_selected_local_wand" );
  CV_REGISTER ( _wMoveTopLocal,    "move_world_local_wand" );
  CV_REGISTER ( _raySelector,      "ray_selection" );
  CV_REGISTER ( _setAnalogTrim,    "calibrate_joystick" );
  CV_REGISTER ( _polysSmooth,      "polygons_smooth" );
  CV_REGISTER ( _polysFlat,        "polygons_flat" );
  CV_REGISTER ( _polysWireframe,   "polygons_wireframe" );
  CV_REGISTER ( _polysPoints,      "polygons_points" );
  CV_REGISTER ( _polysScribe,      "polygons_scribe" );
  CV_REGISTER ( _gridVisibility,   "grid_visibility" );
  CV_REGISTER ( _statusBarVis,     "status_bar_visibility" );
  CV_REGISTER ( _viewHome,         "camera_view_home" );
  CV_REGISTER ( _viewWorld,        "camera_view_world" );
  CV_REGISTER ( _viewScene,        "camera_view_scene" );
  CV_REGISTER ( _setAsHome,        "set_as_home_view" );
  CV_REGISTER ( _resizeGrid,       "resize_grid" );
  CV_REGISTER ( _backgroundColor,  "background_color" );
  CV_REGISTER ( _gridColor,        "grid_color" );
  CV_REGISTER ( _selectionColor,   "selection_color" );
  CV_REGISTER ( _resetClipping,    "reset_clipping" );
  CV_REGISTER ( _hvTransWandXZ,    "hv_trans_wand_xz" );
  CV_REGISTER ( _hvTransGlobalXZ,  "hv_trans_global_xz" );
  CV_REGISTER ( _poleNav,          "hy_rotate_vy_trans_global" );
  CV_REGISTER ( _gotoViewFront,    "goto_front_view" );
  CV_REGISTER ( _gotoViewBack,     "goto_back_view" );
  CV_REGISTER ( _gotoViewTop,      "goto_top_view" );
  CV_REGISTER ( _gotoViewBottom,   "goto_bottom_view" );
  CV_REGISTER ( _gotoViewRight,    "goto_right_view" );
  CV_REGISTER ( _gotoViewLeft,     "goto_left_view" );
  CV_REGISTER ( _rotateWorld,      "rotate_world" );
  CV_REGISTER ( _dropToFloor,      "drop_to_floor" );
  CV_REGISTER (_toggleAnimations,  "toggle_animations" );
  CV_REGISTER (_animStepFwd,  "animation_step_fwd" );
  //CV_REGISTER ( _saveView,         "save_camera_view" );

  // Get the component.
  VRV::Interfaces::IMenuRead::ValidQueryPtr reader
    ( Usul::Components::Object::create( VRV::Interfaces::IMenuRead::IID,
      CV_GRAPHICAL_USER_INTERFACE ) );

  // Find the path to the config file.
  std::string filename ( CV::Config::filename ( "menu" ) );

  // Read the configuration file.
  reader->readMenuFile ( filename, ValidUnknown ( this ) );

  // Set the menu.
  VRV::Interfaces::IMenuGet::ValidQueryPtr menu ( reader );
  _menu->menu ( menu->getMenu() );

  // Default settings, so that the menu has the correct toggle's checked.
  OsgTools::State::StateSet::setPolygonsFilled ( _models.get(), false );
  OsgTools::State::StateSet::setPolygonsSmooth ( _models.get() );
  OsgTools::Group::removeAllOccurances ( _scribeBranch.get(), _navBranch.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the button callback specified by the string.
//
///////////////////////////////////////////////////////////////////////////////

void Application::setCallback ( const std::string &id, Button *button )
{
  ErrorChecker ( 4127973658u, isAppThread(), CV::NOT_APP_THREAD );

  // Handle bad input.
  if ( 0x0 == button )
    return;

  // Make sure it is lower case.
  std::string cb ( id );
  std::transform ( cb.begin(), cb.end(), cb.begin(), ::tolower );

  // Get the button.
  ButtonMap::const_iterator i = _buttonMap.find ( cb );

  // Hook up the default callback.
  MenuKit::Callback::Ptr dcb ( MenuKit::memFunCB2 ( this, &Application::_defaultCallback ) );
  button->callback ( MenuKit::MESSAGE_FOCUS_ON, dcb );

  // See if it is a known button.
  if ( _buttonMap.end() != i )
  {
    // Set the callbacks.
    button->callback ( MenuKit::MESSAGE_SELECTED,  i->second.get() );
    button->callback ( MenuKit::MESSAGE_UPDATE,    i->second.get() );
  }

  // Otherwise...
  else
  {
    // Hook up the default callback.
    MenuKit::Callback::Ptr cb ( MenuKit::memFunCB2 ( this, &Application::_defaultCallback ) );
    button->callback ( MenuKit::MESSAGE_SELECTED, dcb );
    button->callback ( MenuKit::MESSAGE_UPDATE,   dcb );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the menu callback specified by the string.
//
///////////////////////////////////////////////////////////////////////////////

void Application::setCallback ( const std::string &, MenuKit::Menu *menu )
{
  ErrorChecker ( 2386250199u, isAppThread(), CV::NOT_APP_THREAD );

  // Handle bad input.
  if ( 0x0 == menu )
    return;

  // Hook up the default callback.
  MenuKit::Callback::Ptr cb ( MenuKit::memFunCB2 ( this, &Application::_defaultCallback ) );
  menu->callback ( MenuKit::MESSAGE_SELECTED,  cb );
  menu->callback ( MenuKit::MESSAGE_UPDATE,    cb );
  menu->callback ( MenuKit::MESSAGE_FOCUS_ON,  cb );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the status-bar.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initStatusBar()
{
  ErrorChecker ( 2652041460u, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 1890904769u, _statusBar.valid() );
  typedef USUL_VALID_REF_POINTER(MenuKit::Menu) ValidStatusBar;
  typedef USUL_VALID_REF_POINTER(osg::StateSet) StateSet;

  // Set the status-bar scene.
  osg::Matrixf sbm;
  OsgTools::Convert::matrix ( _prefs->statusBarMatrix(), sbm );
  _statusBranch->setMatrix ( sbm );
  _statusBar->scene ( _statusBranch.get() );

  // Set the status-bar's properties.
  _statusBar->menu()->append ( new MenuKit::Button );
  _statusBar->menu()->expanded ( _prefs->statusBarVisibleAtStartup() );
  _statusBar->updateScene();

  // Make the status-bar always draw on top (last). See osgfxbrowser.cpp.
	StateSet ss ( _statusBranch->getOrCreateStateSet() );
	ss->setRenderBinDetails ( 100, "RenderBin" );
	ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the command-line arguments.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_parseCommandLine()
{
  ErrorChecker ( 1067093692u, isAppThread(), CV::NOT_APP_THREAD );

  // Extract the restart-files and remove them from the remaining arguments.
  Parser::Args restart = _parser->files ( ".cv", true );

  // Load the restart files.
  std::for_each ( restart.begin(),
                  restart.end(), 
                  Usul::Adaptors::memberFunction ( this, &Application::_loadRestartFile ) );

#if defined (INV3RSION_COLLABORATE)
  // Extract the avatar cfg files and remove them from the remaining arguments.
  Parser::Args avatar = _parser->files ( ".cfg", true );

  // Load the avatar files.
  std::for_each ( avatar.begin(),
                  avatar.end(), 
                  Usul::Adaptors::memberFunction ( this, &Application::_registerAvatar ) );
#endif

  // Extract the model files and remove them from the remaining arguments.
  Parser::Args models = _parser->files ( true );

  // Load the model files.
  std::for_each ( models.begin(),
                  models.end(), 
                  Usul::Adaptors::memberFunction ( this, &Application::_loadModelFile ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the frame is drawn.
//
///////////////////////////////////////////////////////////////////////////////

void Application::preFrame()
{
  // For safety.
  try
  {
    this->_preFrame();
  }

  // Catch exceptions.
  catch ( const std::exception &e )
  {
    this->_update ( *_msgText, "Error 1999395957: Application::preFrame(): exception: " + std::string ( e.what() ) );
  }
#if 1
  // Catch exceptions.
  catch ( ... )
  {
    this->_update ( *_msgText, "Error 2847458960: Application::preFrame(): unknown exception." );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the frame is drawn.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_preFrame()
{
  ErrorChecker ( 1067093580, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 1067222382, _cursor.valid() );
  
  // Call the base class's function.
  BaseClass::preFrame();
  
  // Update the frame-time.
  this->_updateFrameTime();

  // Update these input devices.
  _buttons->update();
  _tracker->update();
  _joystick->update();

  // Send any notifications.
  _joystick->notify();

  // Update the cursor.
  this->_updateCursor();

  // Update the frame-rate's text.
  this->_updateFrameRateDisplay();

  // Update the analog-input text.
  this->_updateAnalogText();

  // See if there are any commands in the queue.
  this->_processCommands();

  // Process button states.
  this->_processButtons();

  // Use the scene-tool if we are supposed to.
  this->_useSceneTool();

  // Navigate if we are supposed to.
  this->_navigate();

  // Check to see if we are receiving a model, if enabled
# if defined (USE_SINTERPOINT)
    this->_sinterReceiveData();
# endif

#if defined (INV3RSION_COLLABORATE)
    _headTracker->update();
    this->_updateAvatars();
#endif

}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the button states.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_processButtons()
{
  ErrorChecker ( 1083961848u, isAppThread(), CV::NOT_APP_THREAD );

#if 1
  switch ( _buttons->pressed() )
  {
    case CV::BUTTON0: std::cout << CV::BUTTON0 << " Button 0 pressed (YELLOW)" << std::endl; break;
    case CV::BUTTON1: std::cout << CV::BUTTON1 << " Button 1 pressed (RED)" << std::endl; break;
    case CV::BUTTON2: std::cout << CV::BUTTON2 << " Button 2 pressed (GREEN)" << std::endl; break;
    case CV::BUTTON3: std::cout << CV::BUTTON3 << " Button 3 pressed (BLUE)" << std::endl; break;
    case CV::BUTTON4: std::cout << CV::BUTTON4 << " Button 4 pressed (JOYSTICK)" << std::endl; break;
    case CV::BUTTON5: std::cout << CV::BUTTON5 << " Button 5 pressed (TRIGGER)" << std::endl; break;
  }
#endif
  
  // Let the menu process first.
  if ( this->_handleMenuEvent() )
    return;

  // Now process the intersector buttons.
  if ( this->_handleIntersectionEvent() )
    return;

  if ( this->_handleNavigationEvent() )
    return;

  if ( this->_handleToolEvent() )
    return;

  if ( this->_handleCancelEvent() )
    return;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the button states and apply to the menu.
//  Returns true if the event was handled.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::_handleMenuEvent()
{
  ErrorChecker ( 1071559313u, isAppThread(), CV::NOT_APP_THREAD );

  // First see if you are supposed to show or hide it. Always do this first.
  if ( COMMAND_MENU_TOGGLE == _buttons->released() )
  {
    _menu->toggleVisible();
    return true;
  }

  // If we are not expanded then we should not handle button events.
  if ( !_menu->menu()->expanded() )
    return false;

  // Initialize.
  bool handled ( true );

  // Process button states iff the menu is showing.
  switch ( _buttons->released() )
  {
    case COMMAND_MENU_SELECT:
      _menu->selectFocused();
      break;

    case COMMAND_MENU_LEFT:
      _menu->moveFocused ( MenuKit::Behavior::LEFT );
      break;

    case COMMAND_MENU_RIGHT:
      _menu->moveFocused ( MenuKit::Behavior::RIGHT );
      break;

    case COMMAND_MENU_UP:
      _menu->moveFocused ( MenuKit::Behavior::UP );
      break;

    case COMMAND_MENU_DOWN:
      _menu->moveFocused ( MenuKit::Behavior::DOWN );
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

bool Application::_handleIntersectionEvent()
{
  ErrorChecker ( 2588614392u, isAppThread(), CV::NOT_APP_THREAD );

  if(!_intersector) return false;

  // Process pressed states.
  if ( COMMAND_SELECT == _buttons->down() )
  {
    this->_intersect();
    return true;
  }
  
  else if ( COMMAND_HIDE_SELECTED == _buttons->down() )
  {
    this->_hideSelected ( MenuKit::MESSAGE_SELECTED, NULL );
    return true;
  }
  
  else if ( COMMAND_UNSELECT_VISIBLE == _buttons->down() )
  {
    this->_unselectVisible ( MenuKit::MESSAGE_SELECTED, NULL );
    return true;
  }
  
  else if ( COMMAND_SHOW_ALL == _buttons->down() )
  {
    this->_showAll ( MenuKit::MESSAGE_SELECTED, NULL );
    return true;
  }

  // Process released states.
  if ( COMMAND_SELECT == _buttons->released() )
  {
    this->_select();
    this->_updateSceneTool();
    return true;
  }

  // We didn't handle the event.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the button states and apply to Navigation mode selection.
//  Returns true if the event was handled.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::_handleNavigationEvent( const unsigned long eventRequest )
{
  MenuKit::Message nav_message = MenuKit::MESSAGE_SELECTED;   // simulate message/item from MenuKit
  MenuKit::Item *nav_item (0x0);                              // NULL b/c it's never needed
  const unsigned long walkID = 1084438120u;                   // comes from AppCallback.cpp
  bool handled;
  unsigned long mode;
  
  if(_intersector) return false;                              // skip this code if we're in an intersection mode

#if defined (INV3RSION_NAV)
  mode = _buttons->pressed();                                 // Button press event

  if ( eventRequest )
    mode = eventRequest;

  switch ( mode )
  {
    case INVR_TRANSLATE :
      std::cout << "INV3RSION CAD TRANSLATE" << std::endl;
#if defined (INV3RSION_COLLABORATE)
      _requestNavControl();
#endif
      _syncInvrNav();
      this->_removeCursorChildren();
      _navigatorH = 0x0;
      _navigatorV = 0x0;
      _invrNav->SetMode(invr::nav::CAD::TRANSLATE);
      handled = true;
      break;
    case INVR_ROTATE :
      std::cout << "INV3RSION CAD ROTATE" << std::endl;
#if defined (INV3RSION_COLLABORATE)
      _requestNavControl();
#endif
      _syncInvrNav();
      this->_removeCursorChildren();
      _navigatorH = 0x0;
      _navigatorV = 0x0;
      if(_prefs->newRotation())
        _invrNav->SetMode(invr::nav::CAD::NEWROTATE);
      else 
        _invrNav->SetMode(invr::nav::CAD::ROTATE);
      handled = true;
      break;
    case INVR_SCALE :
      std::cout << "INV3RSION CAD SCALE" << std::endl;
#if defined (INV3RSION_COLLABORATE)
      _requestNavControl();
#endif
      _syncInvrNav();
      this->_removeCursorChildren();
      _navigatorH = 0x0;
      _navigatorV = 0x0;
      _invrNav->SetMode(invr::nav::CAD::SCALE);
      handled = true;
      break;
    case INVR_NAV_RESET :
      std::cout << "INV3RSION CAD RESET" << std::endl;
#if defined (INV3RSION_COLLABORATE)
      _requestNavControl();
#endif
      this->_removeCursorChildren();
      if ( _autoPlacement )
        _doAutoPlacement(true);
      else
        _navBranch->setMatrix ( _home );
      _syncInvrNav();
      break;
  }

#endif
  mode = _buttons->released();                                // Button release event

  if ( eventRequest )                                         // if mode NOT specified with function call ...
    mode = eventRequest;                                      // ... get information from buttons

  switch ( mode )                                             // which button was used???
  {
#if defined (INV3RSION_NAV)
    case INVR_TRANSLATE :
    case INVR_ROTATE :
    case INVR_SCALE :
      std::cout << "STOP NAVIGATION" << std::endl;
#if defined (INV3RSION_COLLABORATE)
      if( _controlAvatar == _localAvatar )
      {
        std::string cmd = "CV AVATAR_CONTROL_RELEASE\n";
        _sinterSendCommand(cmd, false );
      }
#endif
      _navigatorH = 0x0;                                      // invalidate response to horizontal joystick
      _navigatorV = 0x0;                                      // invalidate response to vertical joystick
      _invrNav->SetMode(invr::nav::CAD::NO_NAV);
      this->_setNearAndFarClippingPlanes();
      handled = true;
      break;
#else
    case NAVIGATE_TOGGLE :                                    // BUTTON_YELLOW, if not walking, set walk ...
      std::cout << "TOGGLE: ";                                //                if walking, set pole
      if ( _navigatorH.valid() &&                             // IF h and v control are valid
           _navigatorV.valid() &&                             // AND both set to walkID
           walkID == _navigatorV->id() &&
           walkID == _navigatorH->id() )
      {                                                       // if WALK, set navigation to POLE
        std::cout << "POLE" << std::endl;
        // Stop navigation first, to prevent menu style callbacks from toggling navigation off
        _navigatorH = 0x0;                                    // invalidate response to horizontal joystick
        _navigatorV = 0x0;                                    // invalidate response to vertical joystick
        // Set navigation mode using 'simulated' call from menu
        this->_poleNav ( nav_message, nav_item );             // activate navigation (POLE)
        handled = true;                                       // button event has been handled
      }
      else                                                    // if anything else, set navigation to WALK
      {
        std::cout << "WALK" << std::endl;
        _navigatorH = 0x0;                                    // invalidate response to horizontal joystick
        _navigatorV = 0x0;                                    // invalidate response to vertical joystick
        this->_hvTransGlobalXZ ( nav_message, nav_item );     // activate navigation (WALK)
        handled = true;                                       // button event has been handled
      }
      break;
#endif      
    case NAVIGATE_FLY :                                       // BUTTON_BLUE (FLY in the BLUE sky)
      std::cout << "FLY" << std::endl;
      // Stop navigation first.  This prevents toggling off when already in a nav mode
      _navigatorH = 0x0;                                      // invalidate response to horizontal joystick
      _navigatorV = 0x0;                                      // invalidate response to vertical joystick
      // set (or reset) navigation mode.  Simulated a call from the menu.
      this->_hvTransWandXZ ( nav_message, nav_item );         // activate navigation (FLY)
      handled = true;                                         // button event has been handled
      break;

    case NAVIGATE_NO_NAV :                                    // possibly called from _init()
      std::cout << "STOP NAVIGATION" << std::endl;
      _navigatorH = 0x0;                                      // invalidate response to horizontal joystick
      _navigatorV = 0x0;                                      // invalidate response to vertical joystick
      handled = true;                                         // button event has been handled
      break;

    default :
      handled = false;                                        // button event has NOT been handled
  };

  return handled;  // if false, button event was not handled (doesn't effect navigation)
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the button states and apply to Tool selection.
//  Returns true if the event was handled.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::_handleToolEvent()
{
  MenuKit::Message tool_message = MenuKit::MESSAGE_SELECTED;  // simulate message/item from MenuKit
  MenuKit::Item *tool_item (0x0);                             // NULL b/c it's never needed
  bool handled(false);

#if !defined (INV3RSION_NAV)
  if ( _buttons->released() == TOOL_SCALE )                   // BUTTON_GREEN
  {
    std::cout << "SCALE" << std::endl;
    //_sceneTool=0x0;                                         // invalidate scale tool to prevent toggling off
    this->_vScaleWorld ( tool_message, tool_item );           // if previous line commented, toggle scale; if not, activate scale
    handled = true;                                           // button event has been handled
  }
#endif

  return handled;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Process the button states and apply to cancelation or tools and navigation.
//  Returns true if the event was handled.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::_handleCancelEvent()
{
  //MenuKit::Message tool_message = MenuKit::MESSAGE_SELECTED;  // simulate message/item from MenuKit
  //MenuKit::Item *tool_item (0x0);                             // NULL b/c it's never needed
  bool handled(false);

#if !defined (INV3RSION_NAV)
  if ( _buttons->released() == STOP_NAV_TOOL )                // BUTTON_RED
  {
    std::cout << "STOP NAVIGATION/TOOLS" << std::endl;
    _navigatorH = 0x0;                                        // invalidate response to horizontal joystick
    _navigatorV = 0x0;                                        // invalidate response to vertical joystick
    _sceneTool  = 0x0;                                        // invalidate scale tool to prevent toggling off

    this->_removeCursorChildren();

    handled = true;                                           // button event has been handled
  }
#endif

  return handled;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Callback for the joystick.
//
///////////////////////////////////////////////////////////////////////////////

void Application::JoystickCB::operator() ( vrjGA::Message m, vrjGA::Referenced * )
{
  ErrorChecker ( 1915253659u, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 4165917933u, 0x0 != _app );

  MenuKit::OSG::Menu *menu = _app->_menu.get();
  ErrorChecker ( 3990552070u, 0x0 != menu );

  switch ( m )
  {
    case vrjGA::JOYSTICK_ENTERING_RIGHT:
      menu->moveFocused ( MenuKit::Behavior::RIGHT );
      break;

    case vrjGA::JOYSTICK_ENTERING_LEFT:
      menu->moveFocused ( MenuKit::Behavior::LEFT );
      break;

    case vrjGA::JOYSTICK_ENTERING_UP:
      menu->moveFocused ( MenuKit::Behavior::UP );
      break;

    case vrjGA::JOYSTICK_ENTERING_DOWN:
      menu->moveFocused ( MenuKit::Behavior::DOWN );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after the frame is drawn.
//
///////////////////////////////////////////////////////////////////////////////

void Application::postFrame()
{
  // For safety.
  try
  {
    this->_postFrame();
  }

  // Catch exceptions.
  catch ( const std::exception &e )
  {
    std::ostringstream message;
    message << "Error 3956579158: Application::postFrame(): exception: "
            << e.what();
    this->_update ( *_msgText, message.str() );
  }

  // Catch exceptions.
  catch ( ... )
  {
    std::string message ( "Error 2660463460: Application::postFrame(): unknown exception." );
    this->_update ( *_msgText, message );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after the frame is drawn.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_postFrame()
{
  ErrorChecker ( 1071439494, isAppThread(), CV::NOT_APP_THREAD );

  // Call the base class's function.
  BaseClass::postFrame();

#if 1
  // Initialize the text if we need to. We cannot call this sooner because 
  // contextInit() has to be called first.
  if ( false == Usul::Bits::has ( _flags, Detail::_TEXT_IS_INITIALIZED ) )
    if ( this->getFrameStamp()->getFrameNumber() > 10 )
      this->_initText();
#endif

  if(_anim_steps)
  {
    _anim_steps--;
    if( _anim_steps == 0 && _animModel != NULL)
    {
      _animationsOnOff(false, _animModel);
      _animModel = NULL;
    }
  }

#if defined (USE_SINTERPOINT)
    this->_sinterProcessData();
#if defined (INV3RSION_COLLABORATE)
    this->_sinterProcessCollabData();
#endif
#endif

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
//  Set the navigator based on the current one and the button states.
//
///////////////////////////////////////////////////////////////////////////////

#if 0
void Application::_setNavigator()
{
  ErrorChecker ( 1068651354, isAppThread(), CV::NOT_APP_THREAD );

  // This instance's unknown pointer.
  Unknown::ValidRefPtr unknown ( this->_thisUnknown() );

  // Initialize the transformation functor.
  CV::Functors::BaseFunctor::RefPtr trans ( 0x0 );

  // These are the analog input functors.
  CV::Functors::AnalogInput::ValidRefPtr hai ( new CV::Functors::JoystickHorizontal ( unknown ) );
  CV::Functors::AnalogInput::ValidRefPtr vai ( new CV::Functors::JoystickVertical   ( unknown ) );

  // These are the functors for getting matrices.
  CV::Functors::MatrixFunctor::ValidRefPtr wm ( new CV::Functors::WandRotation   ( unknown ) );
  CV::Functors::MatrixFunctor::ValidRefPtr im ( new CV::Functors::IdentityMatrix ( unknown ) );

  // The three axes.
  CV::Functors::Direction::Vector x ( 1, 0, 0 );
  CV::Functors::Direction::Vector y ( 0, 1, 0 );
  CV::Functors::Direction::Vector z ( 0, 0, 1 );

  // For getting directions in the wand's coordinate system.
  CV::Functors::Direction::ValidRefPtr pwx ( new CV::Functors::Direction ( unknown, x, wm ) );
  CV::Functors::Direction::ValidRefPtr pwy ( new CV::Functors::Direction ( unknown, y, wm ) );
  CV::Functors::Direction::ValidRefPtr pwz ( new CV::Functors::Direction ( unknown, z, wm ) );

  // For getting directions in global coordinate system.
  CV::Functors::Direction::ValidRefPtr pgx ( new CV::Functors::Direction ( unknown, x, im ) );
  CV::Functors::Direction::ValidRefPtr pgy ( new CV::Functors::Direction ( unknown, y, im ) );
  CV::Functors::Direction::ValidRefPtr pgz ( new CV::Functors::Direction ( unknown, z, im ) );

  // Negative wand-space directions.
  CV::Functors::Direction::ValidRefPtr nwx ( new CV::Functors::Direction ( unknown, -x, wm ) );
  CV::Functors::Direction::ValidRefPtr nwy ( new CV::Functors::Direction ( unknown, -y, wm ) );
  CV::Functors::Direction::ValidRefPtr nwz ( new CV::Functors::Direction ( unknown, -z, wm ) );

  // Negative global directions.
  CV::Functors::Direction::ValidRefPtr ngx ( new CV::Functors::Direction ( unknown, -x, im ) );
  CV::Functors::Direction::ValidRefPtr ngy ( new CV::Functors::Direction ( unknown, -y, im ) );
  CV::Functors::Direction::ValidRefPtr ngz ( new CV::Functors::Direction ( unknown, -z, im ) );

  // Get the id of the current navigator.
  unsigned int id = ( _navigator.valid() ) ? _navigator->id() : NO_NAVIGATION;

  switch ( id )
  {
    case NO_NAVIGATION:
    case ROTATE_XY_LOCAL:
      this->_update ( *_msgText, "Translate in local XZ-plane" );
      trans = new CV::Functors::TransformPair 
      (
        unknown, 
        new CV::Functors::Translate ( unknown, pwx, hai, CV::DEFAULT_MAX_RELATIVE_SPEED ), 
        new CV::Functors::Translate ( unknown, nwz, vai, CV::DEFAULT_MAX_RELATIVE_SPEED ), 
        TRANSLATE_XZ_LOCAL
      );

      this->_setCursor ( OsgTools::Axes::POSITIVE_X | OsgTools::Axes::NEGATIVE_X |
                         OsgTools::Axes::POSITIVE_Z | OsgTools::Axes::NEGATIVE_Z |
                         OsgTools::Axes::ORIGIN_CUBE );
      this->_setCursorMatrixFunctor ( new CV::Functors::WandMatrix ( unknown ) );
      break;

    case TRANSLATE_XZ_LOCAL:
      this->_update ( *_msgText, "Translate in global XZ-plane" );
      trans = new CV::Functors::TransformPair 
      (
        unknown, 
        new CV::Functors::Translate ( unknown, pgx, hai, CV::DEFAULT_MAX_RELATIVE_SPEED ), 
        new CV::Functors::Translate ( unknown, ngz, vai, CV::DEFAULT_MAX_RELATIVE_SPEED ), 
        TRANSLATE_XZ_GLOBAL
      );

      this->_setCursor ( OsgTools::Axes::POSITIVE_X | OsgTools::Axes::NEGATIVE_X |
                         OsgTools::Axes::POSITIVE_Z | OsgTools::Axes::NEGATIVE_Z |
                         OsgTools::Axes::ORIGIN_CUBE );
      this->_setCursorMatrixFunctor ( new CV::Functors::WandPosition ( unknown ) );
      break;

    case TRANSLATE_XZ_GLOBAL:
      this->_update ( *_msgText, "Rotate about and translate along the global Y-axis" );
      trans = new CV::Functors::TransformPair
      (
        unknown,
        new CV::Functors::Translate ( unknown, pgy, vai, CV::DEFAULT_MAX_RELATIVE_SPEED ),
        new CV::Functors::Rotate    ( unknown, pgy, hai, CV::DEFAULT_MAX_ANGULAR_SPEED  ),
        ROTATE_XY_LOCAL
      );

      this->_setCursor ( OsgTools::Axes::ROTATE_Y | 
                         OsgTools::Axes::POSITIVE_Y | 
                         OsgTools::Axes::NEGATIVE_Y );
      this->_setCursorMatrixFunctor ( new CV::Functors::WandPosition ( unknown ) );
      break;

    default:
      this->_update ( *_msgText, "Unknown navigation mode" );
      break;
  }

  // Call the other one.
  this->_setNavigator ( trans );
}
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Set the cursor.
//
///////////////////////////////////////////////////////////////////////////////

//void Application::_setCursor ( unsigned int state )
//{
//  ErrorChecker ( 1072722817u, isAppThread(), CV::NOT_APP_THREAD );
//
//  OsgTools::Axes axes;
//  axes.length ( 0.5f );
//  axes.lineWidth ( 0.05f * axes.length() );
//  axes.state ( state );
//
//  OsgTools::Group::removeAllChildren ( _cursor.get() );
//  _cursor->addChild ( axes() );
//}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the cursor's matrix functor.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_removeCursorChildren()
{
  OsgTools::Group::removeAllChildren( _cursorActiveWithRot.get() );
  OsgTools::Group::removeAllChildren( _cursorRedWithRot.get() );
  OsgTools::Group::removeAllChildren( _cursorYellowWithRot.get() );
  OsgTools::Group::removeAllChildren( _cursorGreenWithRot.get() );
  OsgTools::Group::removeAllChildren( _cursorBlueWithRot.get() );
  OsgTools::Group::removeAllChildren( _cursorTriggerWithRot.get() );

  OsgTools::Group::removeAllChildren( _cursorActiveNoRot.get() );
  OsgTools::Group::removeAllChildren( _cursorRedNoRot.get() );
  OsgTools::Group::removeAllChildren( _cursorYellowNoRot.get() );
  OsgTools::Group::removeAllChildren( _cursorGreenNoRot.get() );
  OsgTools::Group::removeAllChildren( _cursorBlueNoRot.get() );
  OsgTools::Group::removeAllChildren( _cursorTriggerNoRot.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the cursor's matrix functor.
//
///////////////////////////////////////////////////////////////////////////////
//
//void Application::_setCursorMatrixFunctor ( CV::Functors::MatrixFunctor *f )
//{
//  _cursorMatrix = f;
//}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the model.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_loadModelFile ( const std::string &filename )
{
  ErrorChecker ( 1067093696u, isAppThread(), CV::NOT_APP_THREAD );

  // Need an identity matrix.
  Matrix44f matrix;
  matrix.identity();

  // Append the request.
  this->requestRead ( filename, matrix );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the restart file.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_loadRestartFile ( const std::string &filename )
{
  ErrorChecker ( 1071091051u, isAppThread(), CV::NOT_APP_THREAD );

  // The reader type.
  typedef VRV::Interfaces::IParseRestart Reader;

  // Declare a restart-file reader.
  Reader::ValidQueryPtr reader 
    ( Usul::Components::Object::create
      ( Reader::IID, CV_RESTART_FILE_PARSER, true, true ) );

  // User feedback.
  this->_update ( *_msgText, "Reading file: " + filename );

  // Read the file.
  reader->parseRestartFile ( filename, ValidUnknown ( this ) );

  // User feedback.
  this->_update ( *_msgText, "Done reading: " + filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the model from the named source and position it using the matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Application::requestRead ( const std::string &filename, const Matrix44f &matrix )
{
  ErrorChecker ( 1584829920u, isAppThread(), CV::NOT_APP_THREAD );

  try
  {
    // Read the model.
    this->_readModel ( filename, matrix );
  }

  catch ( const std::exception &e )
  {
    std::ostringstream out;
    out << "Error 2802808344: Exception caught when loading file."
        << "\n\tWhat: " << e.what()
        << "\n\tFile: " << filename;
    this->_update ( *_msgText, out.str() );
  }

  catch ( ... )
  {
    std::ostringstream out;
    out << "Error 4225577750: Unknown exception caught when loading file."
        << "\n\tFile: " << filename;
    this->_update ( *_msgText, out.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the model and position it using the matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_readModel ( const std::string &filename, const Matrix44f &matrix )
{
  ErrorChecker ( 1901000691u, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 1067093697u, _models.valid() );

  // User feedback.
  this->_update ( *_msgText, "Reading file: " + filename );

  // Create a component for reading the model.
  SAL::Interfaces::IRead::ValidQueryPtr reader 
    ( Usul::Components::Object::create 
      ( SAL::Interfaces::IRead::IID, CV_SCENE_ABSTRACTION_LAYER, true, true ) );

  // Read the model.
  Usul::Interfaces::IUnknown *tmp=NULL;
  SAL::Interfaces::INode::ValidQueryPtr model ( reader->readNodeFile ( filename ) );

  // Get the interface we need.
  SAL::Interfaces::IOSG::ValidQueryPtr iosg ( model );

  // Get the node.
  USUL_VALID_REF_POINTER(osg::Node) node ( dynamic_cast < osg::Node * > ( iosg->osgReferenced() ) );

  // User feedback.
  this->_update ( *_msgText, "Done reading: " + filename );

  // Are we supposed to set the normalize flag? We only turn it on, 
  // not off, because we want to inherit the global state.
  bool norm ( _prefs->normalizeVertexNormalsModels() );
  if ( norm )
    OsgTools::State::StateSet::setNormalize ( node.get(), norm );

  // Make a matrix transform for this model.
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  mt->setName ( std::string ( "Branch for: " ) + filename );

  // Set its matrix.
  osg::Matrixf M;
  OsgTools::Convert::matrix ( matrix, M );
  mt->setMatrix ( M );

  // Set its name to the filename, minus the pathing portion, if there is no name.
  if ( node->getName().empty() )
  {
    unsigned int loc = filename.find_last_of ( "/\\" );
    std::string name = filename.substr ( loc + 1 );
    node->setName ( name );
  }
 
  // Create the scribe effect since it must attach to the model
  osg::ref_ptr<osgFX::Scribe> scribe = new osgFX::Scribe;
  const Preferences::Vec4f &sc = _prefs->scribeColor();
  scribe->setWireframeColor ( osg::Vec4 ( sc[0], sc[1], sc[2], sc[3] ) );
  scribe->setWireframeLineWidth( _prefs->scribeWidth() );
  scribe->addChild ( node.get() );
  _scribeBranch->addChild ( scribe.get() );
  
  // Hook things up.
  mt->addChild ( node.get() );

  // Hook things up.
  _models->addChild ( mt.get() );

  // Do any post-processing.
  this->_postProcessModelLoad ( filename, node.get() );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Return the group node child that is a MatrixTransform
//
///////////////////////////////////////////////////////////////////////////////

osg::MatrixTransform* Application::_getGroupMatrixTransform( osg::Group *grp )
{
  int num_children = grp->getNumChildren();
  int i = 0;
  osg::Transform *xform;
  
  // First test the group node
  if( xform = grp->asTransform() )
  {
    return xform->asMatrixTransform();
  }
  
  // next test its children
  while ( i < num_children )
  {
    xform = grp->getChild(i)->asTransform();
    if(xform)
    {
      return xform->asMatrixTransform();
    }
    i++;
  }
  
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Add or replace a node in the scenegraph
//
///////////////////////////////////////////////////////////////////////////////

void Application::_replaceNode( osg::ref_ptr<osg::Node> node, const std::string &name )
{
  ErrorChecker ( 1901000692u, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 1067093698u, _models.valid() );
  
  // See if this node name matches any other in the scene
  bool matched = false;
  osg::Node *m = dynamic_cast<osg::Node*>( _models.get() );
  if ( m )
  {
    Matcher match;
    if ( _recursiveMatchNodeName ( name, m, &match ) )
    {
      // We found a match
      std::cout << "Match found, replacing node" << std::endl;
      _autoPlacement = false; // do not relocate model if it isn't new
      node->setName ( name );
      
      // Get matrix from original (old) node and apply it to new node
      osg::Matrix mtx;
      osg::Group *old_grp = match.node->asGroup();
      osg::Group *new_grp = node->asGroup();
            
      if ( old_grp && new_grp )
      {
        osg::MatrixTransform *mxform_old = _getGroupMatrixTransform( old_grp );
        if ( mxform_old )
        {
          mtx = mxform_old->getMatrix();
          
          osg::MatrixTransform *mxform_new = _getGroupMatrixTransform( new_grp );
          if ( mxform_new )
          {
            mxform_new->setMatrix( mtx );
          }
        }
      }
      
      // replace the node
      if ( match.parent->replaceChild ( match.node, node.get() ) )
      {
        matched = true;
      }
    }

    // Now rebuild the scribe node corresponding to the match in _models
    if ( matched )
    {
      // Replace the scribe node at the same position in _scribeBranch
      // as the replaced node was in _models, using match.modelNum
      osg::ref_ptr<osgFX::Scribe> scribe = new osgFX::Scribe;
      const Preferences::Vec4f &sc = _prefs->scribeColor();
      scribe->setWireframeColor ( osg::Vec4 ( sc[0], sc[1], sc[2], sc[3] ) );
      scribe->setWireframeLineWidth( _prefs->scribeWidth() );
      scribe->addChild ( node.get() );
      _scribeBranch->setChild ( match.modelNum, scribe.get() );
    }
  }
    
  // Otherwise, add it as a new node
  if ( !matched )
  {
    std::cout << "Adding model as new osg node" << std::endl;

    // Are we supposed to set the normalize flag? We only turn it on, 
    // not off, because we want to inherit the global state.
    bool norm ( _prefs->normalizeVertexNormalsModels() );
    if ( norm )
      OsgTools::State::StateSet::setNormalize ( node.get(), norm );

    // Make a matrix transform for this model.
    osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
    mt->setName ( std::string ( "Branch for: " ) + name );

    // Set its matrix.
    osg::Matrixf M;
    Matrix44f matrix;
    matrix.identity();
    OsgTools::Convert::matrix ( matrix, M );
    mt->setMatrix ( M );

    // Set the node name
    node->setName ( name );

    // Create the scribe effect since it must attach to the model
    osg::ref_ptr<osgFX::Scribe> scribe = new osgFX::Scribe;
    const Preferences::Vec4f &sc = _prefs->scribeColor();
    scribe->setWireframeColor ( osg::Vec4 ( sc[0], sc[1], sc[2], sc[3] ) );
    scribe->setWireframeLineWidth( _prefs->scribeWidth() );
    scribe->addChild ( node.get() );
    _scribeBranch->addChild ( scribe.get() );
    
    // Hook things up.
    mt->addChild ( node.get() );

    // Hook things up.
    _models->addChild ( mt.get() );
  }
  
  // Do any post-processing.
  this->_postProcessModelLoad ( std::string("streamed"), node.get() );
}

#if defined (USE_SINTERPOINT)


///////////////////////////////////////////////////////////////////////////////
//
//  Stream in the model and position it using the matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_streamModel ( std::stringstream &modelstream, const std::string &name )
{
  ErrorChecker ( 1901000692u, isAppThread(), CV::NOT_APP_THREAD );
  
  // User feedback.
  this->_update ( *_msgText, "Reading model stream" );

  // Stream in the file
  osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension( _sinterFileType );
  if(!rw){
	  std::ostringstream out;
    out << "Error: " << _sinterFileType << " plugin not available, streamed file input will fail.";
    this->_update ( *_msgText, out.str() );
    return;
  }
  osgDB::ReaderWriter::ReadResult rr = rw->readNode ( modelstream );
  if(!rr.validNode()){
	  std::ostringstream out;
    out << "Error: Streamed input of node file failed, resulting node not vaild.";
    this->_update ( *_msgText, out.str() );
    return;
  }
  NodePtr node = rr.getNode();
  
  this->_replaceNode(node, name);
  
  // Signal user when done
  this->_update ( *_msgText, "Done reading model stream" );
}

#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Run the program.
//
///////////////////////////////////////////////////////////////////////////////

void Application::run()
{
  ErrorChecker ( 1067094378, isMainThread() );

  // Tell the kernel that we are the app.
  vrj::Kernel::instance()->setApplication ( this );

  // Start the kernel.
  vrj::Kernel::instance()->start();

  // Wait for events.
  vrj::Kernel::instance()->waitForKernelStop();
}


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
//  Get the analog input in the range [-1,1].
//
///////////////////////////////////////////////////////////////////////////////

float Application::joystickHorizontal() const
{
  return 2.0f * ( _joystick->horizontal() + _analogTrim[0] ) - 1.0f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the analog input in the range [-1,1].
//
///////////////////////////////////////////////////////////////////////////////

float Application::joystickVertical() const
{
  return 2.0f * ( _joystick->vertical() + _analogTrim[1] ) - 1.0f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Multiply the navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Application::postMultiply ( const Matrix44f &m )
{
  ErrorChecker ( 1067221542, _navBranch.valid() );
  _navBranch->postMult ( osg::Matrixf ( m.get() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Multiply the navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Application::preMultiply ( const Matrix44f &m )
{
  ErrorChecker ( 1067221543, _navBranch.valid() );
  _navBranch->preMult ( osg::Matrixf ( m.get() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Navigate if we are supposed to.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_navigate()
{
  ErrorChecker ( 1068000936, isAppThread(), CV::NOT_APP_THREAD );

  // If the menu is showing then we don't navigate.
  if ( _menu->menu()->expanded() )
    return;

  // If we have a valid tool then we don't navigate.
  if ( _sceneTool.valid() )
    return;

  // Tell the horizontal-input navigator to execute.
  if ( _navigatorH.valid() )
    (*_navigatorH)();

  // Tell the vertical-input navigator to execute.
  if ( _navigatorV.valid() )
    (*_navigatorV)();

#if defined (INV3RSION_NAV)
  if( !_navigatorH.valid() && !_navigatorV.valid())
  {
    _invrNav->preFrame();
    _navBranch->setMatrix(osg::Matrix(_invrNav->GetTransform()->mData));
    
#if defined (INV3RSION_COLLABORATE)
    if( _controlAvatar == _localAvatar )
    {
      _sinterSendNavUpdate( _invrNav->GetTransform()->mData, false );
    }
#endif
  }
#endif
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
  if ( _menu->menu()->expanded() )
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
  if ( _menu->menu()->expanded() )
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
//  Get the maximum world size.
//
///////////////////////////////////////////////////////////////////////////////

float Application::worldRadius() const
{
  ErrorChecker ( 1068000937, isAppThread(), CV::NOT_APP_THREAD );
  return ( _navBranch.valid() ) ? _navBranch->getBound().radius() : 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the wand's position.
//
///////////////////////////////////////////////////////////////////////////////

void Application::wandPosition ( Usul::Math::Vec3f &p ) const
{
  ErrorChecker ( 1069036812, isAppThread(), CV::NOT_APP_THREAD );

  // Get the wand's offset.
  Usul::Math::Vec3f offset;
  this->wandOffset ( offset );

  // Set the vector from the wand's position plus the offset.
  p[0] = _tracker->x() + offset[0];
  p[1] = _tracker->y() + offset[1];
  p[2] = _tracker->z() + offset[2];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the wand's matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Application::wandMatrix ( Matrix44f &W ) const
{
  ErrorChecker ( 1068004399, isAppThread(), CV::NOT_APP_THREAD );

  // Set the given matrix from the wand's matrix.
  W.set ( _tracker->matrix().getData() );

  // Get the wand's offset.
  Usul::Math::Vec3f offset;
  this->wandOffset ( offset );

  // Translate by our wand's offset.
  Matrix44f T;
  T.makeTranslation ( offset );
  W = T * W;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the wand's offset.
//
///////////////////////////////////////////////////////////////////////////////

void Application::wandOffset ( Usul::Math::Vec3f &v ) const
{
  ErrorChecker ( 4218812392u, isAppThread(), CV::NOT_APP_THREAD );
  v = _wandOffset;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the wand's offset.
//
///////////////////////////////////////////////////////////////////////////////

void Application::wandOffset ( const Usul::Math::Vec3f &v )
{
  ErrorChecker ( 3731142380u, isAppThread(), CV::NOT_APP_THREAD );
  _wandOffset = v;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the rotation portion of the wand's matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Application::wandRotation ( Matrix44f &W ) const
{
  ErrorChecker ( 1068242095, isAppThread(), CV::NOT_APP_THREAD );

  // Get the wand's matrix.
  this->wandMatrix ( W );

  // Zero-out the translations.
  W.setTranslation ( Usul::Math::Vec3f ( 0, 0, 0 ) );
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
//  Get the navigation scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Group *Application::navigationScene() const
{
  ErrorChecker ( 1069794380, _navBranch.valid() );
  return _navBranch.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the navigation scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Application::navigationScene()
{
  ErrorChecker ( 1069794381, _navBranch.valid() );
  return _navBranch.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the models scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Group *Application::modelsScene() const
{
  ErrorChecker ( 1071420702, _models.valid() );
  return _models.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the models scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Application::modelsScene()
{
  ErrorChecker ( 1071420703, _models.valid() );
  return _models.get();
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
//  Update the frame-time.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_updateFrameTime()
{
  ErrorChecker ( 1074146688, isAppThread(), CV::NOT_APP_THREAD );

  // It's ok to have static variables because access to this 
  // function only occurs in the application thread.
  static double lastFrameTime ( 0 );

  // Grab the last-time until it is non-zero.
  if ( 0 == lastFrameTime )
  {
    lastFrameTime = this->_getElapsedTime();
    return;
  }
  // Get the current frame-time.
  double currentFrameTime ( this->_getElapsedTime() );
    
  // Set the time interval.
  _frameTime = currentFrameTime - lastFrameTime;
    
#if 0
  // Make sure it is not zero.
  WarningChecker ( 1074200431u, _frameTime > 0, "Frame-time is zero" );
#endif

  // Save the current time.
  lastFrameTime = currentFrameTime;
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
  double ft ( this->_getFrameTime() );
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
//  Read the user's preferences.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_readUserPreferences()
{
  ErrorChecker ( 1074032451u, isMainThread() );

  // If the file is missing then it throws.
  try
  {
    // Find the path to the preference file.
    std::string filename ( CV::Config::filename ( "preferences" ) );
                                                        // searches for preferences.xml in $HOME/.cadviewer/ ...
                                                        // ... returns full path if file is found

    // Read the preferences.
    _prefs->read ( filename );
  }

  // Catch expected exceptions.
  catch ( const std::exception &e )
  {
    std::ostringstream message;
    message << "Warning 1083877168, exception generated when attempting "
            << "to read user-preferences file."
            << "\n\t" << e.what();
    this->_update ( *_msgText, message.str() );
  }

  // Make sure there is a writer-machine.
  WarningChecker ( 2816534029u,
                   !_prefs->fileWriterMachineName().empty(), 
                   "No machine specified as the file-writer in user-preferences." );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame-time.
//
///////////////////////////////////////////////////////////////////////////////

double Application::_getFrameTime() const
{
  ErrorChecker   ( 1074147408u, isAppThread(), CV::NOT_APP_THREAD );
#if 0
  WarningChecker ( 1076367925u, _frameTime > 0, "Frame-time is corrupt" );
#endif
  return _frameTime;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame-time.
//
///////////////////////////////////////////////////////////////////////////////

float Application::frameTime() const
{
  return this->_getFrameTime();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the cursor.
//
///////////////////////////////////////////////////////////////////////////////

//void Application::_updateCursor()
//{
//  ErrorChecker ( 1074207542u, isAppThread(), CV::NOT_APP_THREAD );
//
//  // If we have a matrix functor...
//  if ( _cursorMatrix.valid() )
//  {
//    // Update the internal matrix.
//    (*_cursorMatrix)();
//
//    // Set the cursor's matrix.
//    osg::Matrixf M;
//    OsgTools::Convert::matrix ( _cursorMatrix->matrix(), M );
//    _cursor->setMatrix ( M );
//  }
//}

void Application::_updateCursor()
{
  ErrorChecker ( 1100576052u, isAppThread(), CV::NOT_APP_THREAD );

  OsgTools::Group::removeAllChildren( _cursor.get() );

  osg::Matrixf oM;                                // OSG Matrix
  Usul::Math::Matrix44f uM;                       // Usul Matrix
  this->wandMatrix( uM );                         // get wand's matrix
  OsgTools::Convert::matrix ( uM , oM );          // oM contains wand's Matrix

  osg::Matrixf oRM;                               // OSG Matrix
  Usul::Math::Matrix44f uRM;                      // Usul Matrix
  this->wandRotation ( uRM );                     // get wand's rotational Matrix
  OsgTools::Convert::matrix ( uRM, oRM );         // oRM contains wand's Rotational Orientation

  float radius(1.0);                              // offset of each cursor from wand
  osg::Matrixf oS;
  oS.makeScale( 0.5, 0.5, 0.5 );                  // change size of each cursor
  osg::Matrixf oTM;                               // oTM will hold an OSG Translation Matrix

  //   0.5000,  0.8660,  0.0    );                // 1:00 position
  //   0.7071,  0.7071,  0.0    );                // 1:30 position
  //  -0.9659, -0.2588,  0.0    );                // 8:30 position
  //  -0.5000, -0.8660,  0.0    );                // 7:00 position
  //   0.5000, -0.8660,  0.0    );                // 5:00 position
  //   0.9659, -0.2588,  0.0    );                // 3:30 position
  //  -0.3536, -0.3536, -0.8660 );                // 7:30, low position

  if ( _cursorActiveWithRot.get() != 0x0 )        // 1:00 position
  {
    oTM.makeTranslate(  0.5000*radius,  0.0000       , -0.8660*radius );
    _cursorActiveWithRot->setMatrix( oS*oTM*oM );
    _cursor->addChild( _cursorActiveWithRot.get() );
  }
  if ( _cursorActiveNoRot.get() != 0x0 )
  {
    oTM.makeTranslate(  0.5000*radius,  0.0000       , -0.8660*radius );
    _cursorActiveNoRot->setMatrix( oS*osg::Matrixf::inverse( oRM )*oTM*oM );
    _cursor->addChild( _cursorActiveNoRot.get() );
  }


  if ( _cursorRedWithRot.get() != 0x0 )           // 8:30 position
  {
    oTM.makeTranslate( -0.9659*radius,  0.0000       ,  0.2588*radius );
    _cursorRedWithRot->setMatrix( oS*oTM*oM );
    _cursor->addChild( _cursorRedWithRot.get() );
  }
  if ( _cursorRedNoRot.get() != 0x0 )
  {
    oTM.makeTranslate( -0.9659*radius,  0.0000       ,  0.2588*radius );
    _cursorRedNoRot->setMatrix( oS*osg::Matrixf::inverse( oRM )*oTM*oM );
    _cursor->addChild( _cursorRedNoRot.get() );
  }


  if ( _cursorYellowWithRot.get() != 0x0 )        // 7:00 position
  {
    oTM.makeTranslate( -0.5000*radius,  0.0000       ,  0.8660*radius );
    _cursorYellowWithRot->setMatrix( oS*oTM*oM );
    _cursor->addChild( _cursorYellowWithRot.get() );
  }
  if ( _cursorYellowNoRot.get() != 0x0 )
  {
    oTM.makeTranslate( -0.5000*radius,  0.0000       ,  0.8660*radius );
    _cursorYellowNoRot->setMatrix( oS*osg::Matrixf::inverse( oRM )*oTM*oM );
    _cursor->addChild( _cursorYellowNoRot.get() );
  }
  

  if ( _cursorGreenWithRot.get() != 0x0 )         // 5:00 position
  {
    oTM.makeTranslate(  0.5000*radius,  0.0000       ,  0.8660*radius );
    _cursorGreenWithRot->setMatrix( oS*oTM*oM );
    _cursor->addChild( _cursorGreenWithRot.get() );
  }
  if ( _cursorGreenNoRot.get() != 0x0 )
  {
    oTM.makeTranslate(  0.5000*radius,  0.0000       ,  0.8660*radius );
    _cursorGreenNoRot->setMatrix( oS*osg::Matrixf::inverse( oRM )*oTM*oM );
    _cursor->addChild( _cursorGreenNoRot.get() );
  }


  if ( _cursorBlueWithRot.get() != 0x0 )          // 3:30 position
  {
    oTM.makeTranslate(  0.9659*radius,  0.0000       ,  0.2588*radius );
    _cursorBlueWithRot->setMatrix( oS*oTM*oM );
    _cursor->addChild( _cursorBlueWithRot.get() );
  }
  if ( _cursorBlueNoRot.get() != 0x0 )
  {
    oTM.makeTranslate(  0.9659*radius,  0.0000       ,  0.2588*radius );
    _cursorBlueNoRot->setMatrix( oS*osg::Matrixf::inverse( oRM )*oTM*oM );
    _cursor->addChild( _cursorBlueNoRot.get() );
  }


  if ( _cursorTriggerWithRot.get() != 0x0 )       // 7:30, low position
  {
    oTM.makeTranslate( -0.3536*radius, -0.8660*radius,  0.3536*radius );
    _cursorTriggerWithRot->setMatrix( oS*oTM*oM );
    _cursor->addChild( _cursorTriggerWithRot.get() );
  }
  if ( _cursorTriggerNoRot.get() != 0x0 )
  {
    oTM.makeTranslate( -0.3536*radius, -0.8660*radius,  0.3536*radius );
    _cursorTriggerNoRot->setMatrix( oS*osg::Matrixf::inverse( oRM )*oTM*oM );
    _cursor->addChild( _cursorTriggerNoRot.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the usage string.
//
///////////////////////////////////////////////////////////////////////////////

void Application::usage ( const std::string &exe, std::ostream &out )
{
  out << "usage: " << exe << ' ';
  out << "<juggler1.config> [juggler2.config ... jugglerN.config] ";
  out << "[file1.osg ... fileN.osg] ";
  out << '\n';
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load all the config files.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_loadConfigFiles ( const Parser::Args &configs )
{
  ErrorChecker ( 1074300066, isMainThread() );

  // See if there is at least one config file. Do not use the ErrorChecker.
  if ( configs.empty() )
  {
    std::cout << "No VRJuggler config-files specified."
              << "\n\tAttempting to use a sim-mode configuration."
              << std::endl;
    this->_loadSimConfigs();
    return;
  }

  // Load the config files.
  std::for_each ( configs.begin(), configs.end(), CV::Detail::LoadConfigFile() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the default config files.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_loadSimConfigs()
{
  ErrorChecker ( 1082599461u, isMainThread() );

  std::cout << "Looking for environment variable VJ_CONFIG_DIR" << std::endl;

  // Try this environment variable.
  const char *dir = ::getenv ( "VJ_CONFIG_DIR" );
  if ( dir )
  {
    // Load default config files from this directory.
    this->_loadSimConfigs ( dir );
    return;
  }

  std::cout << "Environment variable VJ_CONFIG_DIR not found." << std::endl;
  std::cout << "Looking for environment variable VJ_BASE_DIR" << std::endl;

  // Try this environment variable.
  dir = ::getenv ( "VJ_BASE_DIR" );
  if ( dir )
  {
    // Make sure there is a slash.
    std::string d ( dir );
    std::string::size_type last ( d.size() - 1 );
    if ( '/' != d[last] || '\\' != d[last] )
      d += '/';

    // Add the sub-directory.
    d += "share/vrjuggler/data/configFiles";

    // Load default config files from this directory.
    this->_loadSimConfigs ( d );
    return;
  }

  std::cout << "Environment variable VJ_BASE_DIR not found." << std::endl;

  // If we get this far then we failed.
  Usul::Exceptions::Thrower < CV::Exceptions::UserInput > 
    ( "No default VRJuggler config-files found." );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the default config files from the given directory.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_loadSimConfigs ( std::string dir )
{
  ErrorChecker ( 1082600560u, isMainThread() );
  ErrorChecker ( 1082600585u, !dir.empty() );

  // Make sure there is a slash.
  std::string::size_type last ( dir.size() - 1 );
  if ( '/' != dir[last] || '\\' != dir[last] )
    dir += '/';

  // The config-file loader.
  CV::Detail::LoadConfigFile loader;

  // Load the config files.
  loader ( dir + "sim.base.jconf" );
  loader ( dir + "sim.wand.mixin.jconf" );
  loader ( dir + "sim.analog.mixin.jconf" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elapsed time since the program started (in seconds).
//
///////////////////////////////////////////////////////////////////////////////

double Application::_getElapsedTime()
{
  ErrorChecker ( 1074354783, isAppThread(), CV::NOT_APP_THREAD );
#if 0 // Lousy on linux.
  static clock_t start ( ::clock() );
  static double  CYCLES_TO_SECONDS ( 1.0 / double ( CLOCKS_PER_SEC ) );
  clock_t current ( ::clock() );
  double  elapsed ( double ( current - start ) * CYCLES_TO_SECONDS );
  ::printf ( "%f %d %f %d\n", CYCLES_TO_SECONDS, start, elapsed, current );
  return ( elapsed );
#endif
  return this->getFrameStamp()->getReferenceTime();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the command queue.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_processCommands()
{
  ErrorChecker ( 1075270955, isAppThread(), CV::NOT_APP_THREAD );
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
  if ( allowDeletion )
    BaseClass::unref();
  else
    BaseClass::unref_nodelete();
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
  case CV::Interfaces::IApplication::IID:
    return static_cast<CV::Interfaces::IApplication *>(this);
  case CV::Interfaces::IAuxiliaryScene::IID:
    return static_cast<CV::Interfaces::IAuxiliaryScene *>(this);
  case CV::Interfaces::INavigationScene::IID:
    return static_cast<CV::Interfaces::INavigationScene *>(this);
  case CV::Interfaces::IModelsScene::IID:
    return static_cast<CV::Interfaces::IModelsScene *>(this);
  case CV::Interfaces::IFrameInfoFloat::IID:
    return static_cast<CV::Interfaces::IFrameInfoFloat *>(this);
  case CV::Interfaces::IWorldInfoFloat::IID:
    return static_cast<CV::Interfaces::IWorldInfoFloat *>(this);
  case CV::Interfaces::IMatrixMultiplyFloat::IID:
    return static_cast<CV::Interfaces::IMatrixMultiplyFloat *>(this);
  case CV::Interfaces::IWandStateFloat::IID:
    return static_cast<CV::Interfaces::IWandStateFloat *>(this);
  case CV::Interfaces::IJoystickFloat::IID:
    return static_cast<CV::Interfaces::IJoystickFloat *>(this);
  case CV::Interfaces::IClippingDistanceFloat::IID:
    return static_cast<CV::Interfaces::IClippingDistanceFloat *>(this);
  case VRV::Interfaces::IRequestRead::IID:
    return static_cast<VRV::Interfaces::IRequestRead *>(this);
  case VRV::Interfaces::IButtonCallback::IID:
    return static_cast<VRV::Interfaces::IButtonCallback *>(this);
  case VRV::Interfaces::IMenuCallback::IID:
    return static_cast<VRV::Interfaces::IMenuCallback *>(this);
  case CV::Interfaces::IVisibility::IID:
    return _iVisibility.get();
  case CV::Interfaces::ISelection::IID:
    return _iSelection.get();
  case CV::Interfaces::IMaterialStack::IID:
    return _iMaterialStack.get();
  case Usul::Interfaces::IUnknown::IID:
    return static_cast<Usul::Interfaces::IUnknown *>(static_cast<CV::Interfaces::IApplication *>(this));
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post-process the model loading.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_postProcessModelLoad ( const std::string &filename, osg::Node *model )
{
  ErrorChecker ( 1075424504, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 1075424505, false == filename.empty() );
  ErrorChecker ( 1075424506, 0x0   != model );
  ErrorChecker ( 1075424507, model->referenceCount() > 0 ); // Should be in scene.

  // To shorten the lines.
  typedef Usul::Components::Object Object;
  typedef Object::UnknownList UnknownList;
  typedef CV::Interfaces::IPostModelLoad PostProcess;

  // See if we have any appropriate plugins.
  UnknownList unknowns;
  Object::create ( PostProcess::IID, unknowns );

  // Loop through the found plugins.
  for ( UnknownList::iterator i = unknowns.begin(); i != unknowns.end(); ++i )
  {
    // Grab the pointer.
    Usul::Interfaces::IUnknown::ValidRefPtr u ( *i );

    // Query for the one we need. This should always work.
    PostProcess::ValidQueryPtr pp ( u.get() );

    // Call the function.
    pp->postProcessModelLoad ( filename, model );
  }
  
  if ( _autoPlacement )
    _doAutoPlacement(true);
  
  // reset this boolean because it may have been changed by stream loader
  _autoPlacement = _prefs->autoPlacement();
  
#if defined (INV3RSION_NAV)
  _syncInvrNav();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return an unknown pointer to this instance.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Application::_thisUnknown()
{
  return this->queryInterface ( Usul::Interfaces::IUnknown::IID );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the near and far clipping planes based on the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_setNearAndFarClippingPlanes()
{
  ErrorChecker ( 2381597435u, isAppThread(), CV::NOT_APP_THREAD );

  // Get the bounding sphere.
  const osg::BoundingSphere &sphere = _root->getBound();
  float radius = sphere.radius();

  // Handle zero-sized bounding spheres.
  if ( radius <= 1e-6 )
    radius = 1;

  // Set both distances.
  _clipDist[0] = 0.1f;
  _clipDist[1] = 5 * radius;

  // Don't allow a far plane less that 40; it's just too small
  if ( _clipDist[1] < 40 )
  {
    _clipDist[1] = 40.0f;
  }
  
  // Set the clipping planes
  vrj::Projection::setNearFar ( _clipDist[0], _clipDist[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the near and far clipping plane from file.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_readNearAndFarClippingPlanes()
{
  ErrorChecker ( 3749122612u, isAppThread(), CV::NOT_APP_THREAD );

  const char *home = ::getenv ( "HOME" );
  if ( !home )
  {
    this->_update ( *_msgText, "Error 2675346972: Environment variable 'HOME' not found." );
    return;
  }

  std::ostringstream filename;
  filename << home << "/.cadviewer/near_and_far_clipping_plane_distances.txt";
  std::ifstream in ( filename.str().c_str() );
  if ( !in )
  {
    std::ostringstream message;
    message << "Error 2987625210: Failed to open file: " << filename;
    this->_update ( *_msgText, message.str() );
    return;
  }

  float zNear ( -1 ), zFar ( -1 );
  in >> zNear >> zFar;

  if ( zNear > 0 && zNear < zFar )
    vrj::Projection::setNearFar ( zNear, zFar );

  else
  {
    std::ostringstream message;
    message << "Error 3424960281: Invalid clipping-plane distances."
            << "\n\tFile: " << filename
            << "\n\tNear: " << zNear
            << "\n\t Far: " << zFar;
    this->_update ( *_msgText, message.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the clipping planes
//
///////////////////////////////////////////////////////////////////////////////

void Application::getClippingDistances ( float &nearDist, float &farDist ) const
{
  ErrorChecker ( 1348789405u, isAppThread(), CV::NOT_APP_THREAD );
  nearDist = _clipDist[0];
  farDist  = _clipDist[1];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the clipping planes.
//
///////////////////////////////////////////////////////////////////////////////

void Application::setClippingDistances ( float nearDist, float farDist )
{
  ErrorChecker ( 3211238302u, isAppThread(), CV::NOT_APP_THREAD );
  _clipDist[0] = nearDist;
  _clipDist[1] = farDist;
  vrj::Projection::setNearFar ( nearDist, farDist );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the scene to file.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_writeScene ( const std::string &filename, const osg::Node *node ) const
{
  ErrorChecker ( 2380772833u, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 3998470026u, 0x0 != node );

  // If the machine name is the same as the writer...
  const std::string host    ( Usul::System::Host::name() );
  const std::string &writer = _prefs->fileWriterMachineName();

  // Make sure there is a writer-machine.
  ErrorChecker ( 2519309141u, !writer.empty(), 
    "No machine specified as the file-writer in user-preferences." );

  // Write the file iff the machine name is the same as the writer-name.
  if ( host == writer )
  {
    if ( false == osgDB::writeNodeFile ( *node, filename ) )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 1421787869, failed to write file.",
          "\n\tFile Name: ", filename,
          "\n\tNode Name: ", node->getName(),
          "\n\tAddress:   ", node );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Generate a string from the integer.
//
///////////////////////////////////////////////////////////////////////////////

std::string Application::_counter ( unsigned int num ) const
{
  ErrorChecker ( 1823321404u, isAppThread(), CV::NOT_APP_THREAD );
  const unsigned int size ( 512 );
  char buffer[size];
  ::sprintf ( buffer, "%04d", num );
  return std::string ( buffer );
}


///////////////////////////////////////////////////////////////////////////////
//
// Update the status-bar. The status-bar is a menu with a single button.
// This was done as a work-around for allignment issues.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_updateStatusBar ( const std::string &s )
{
  ErrorChecker ( 2977280421u, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 1987011863u, !_statusBar->menu()->empty() );

#if 0 // Remove when confident.
  // If the menu is not expanded then punt.
  if ( !_statusBar->menu()->expanded() )
    return;
#endif

  // Get the button and set its text.
  MenuKit::Menu::iterator item = _statusBar->menu()->begin();
  ErrorChecker ( 3931363718u, 0x0 != item->get() );
  item->get()->text ( ( s.empty() ) ? "Ready" : s );

  // Rebuild the scene.
  _statusBar->updateScene();
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the current camera position as the home view.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_setHome()
{
  ErrorChecker ( 3948236564u, isAppThread(), CV::NOT_APP_THREAD );
  _home = _navBranch->getMatrix();
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the color from the string.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4& Application::_getColor ( const std::string& word ) const
{
  ErrorChecker ( 1084416660u, isAppThread(), CV::NOT_APP_THREAD );

  std::string lc ( word );
  std::transform ( lc.begin(), lc.end(), lc.begin(), ::tolower );
  ColorMap::const_iterator iter = _colorMap.find ( lc );

  if ( _colorMap.end() == iter )
    return this->getBackgroundColor();

  else
    return iter->second;
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
  typedef Usul::Functors::IfThen<IsSelected,Append> IfThen;
//  typedef Usul::Functors::IfThen<IsWanted,Append> IfThen;
  typedef OsgTools::Visitor<MT,IfThen> Visitor;

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
  _models->accept ( *visitor );

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
//  Compare new & old node names for a match 
//
///////////////////////////////////////////////////////////////////////////////

bool Application::_matchNodeNames( const std::string &new_name, const std::string &old_name )
{
  // Make copies of the strings to compare.
  std::string newname = new_name;
  std::string oldname = old_name;
  
  // Convert names to lowercase because match is case-insensitive.
  std::transform ( newname.begin(), newname.end(), newname.begin(), ::tolower );
  std::transform ( oldname.begin(), oldname.end(), oldname.begin(), ::tolower );
  
  if( newname == oldname )
  {
    return true;
  }
  else
  {
    // If we don't find an exact match, we need to look for a looser match.
    // New name_par_4.jt may match old name.par_4
    std::string::size_type nsz = newname.rfind(std::string("_par"), newname.size() - 1);
    std::string::size_type osz = oldname.rfind(std::string(".par"), oldname.size() - 1);
    
    // If that format seems to be the case, truncate the strings and re-test.
    if( nsz != std::string::npos && osz != std::string::npos )
    {
      newname = newname.substr( 0, nsz );
      oldname = oldname.substr( 0, osz );
      
      if( newname == oldname )
      {
        return true;
      }
    }
    
  }
  
  return false;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Recursively check if model node name matches any others in _models
//  and returns the matching node/group as a node 
//
///////////////////////////////////////////////////////////////////////////////

bool Application::_recursiveMatchNodeName ( const std::string &name, osg::Node *model, Matcher *match )
{
  osg::Group *g;

  // Check for match on node name
  if ( _matchNodeNames( name, model->getName() ) )
  {
    // Only assign the model, the others are done by the caller
    match->node = model;
    match->parent = NULL;
    match->modelNum = 0;
    return true;
  }
  
  // If model is a group, it may contain more nodes, so search
  g = model->asGroup();
  if ( g )
  {
    for ( unsigned int i=0; i<g->getNumChildren(); ++i )
    {
      if ( _recursiveMatchNodeName ( name, g->getChild(i), match ) )
      {
        // Only assign the parent if we just found the node
        if ( match->parent == NULL )
        {
          match->parent = g;
        }
        
        // Keep reassigning the modelNum, since we need the index from _models 
        match->modelNum = i;

        return true;
      }
    }
  }
  
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wipe out all models in the scene
//
///////////////////////////////////////////////////////////////////////////////

void Application::_deleteScene()
{
  // Loop through the _models deleting every child it can find until no more exist
  // This needs to be done since OSG apparently moves the children back up when the parent
  // is deleted
  while ( _models->getNumChildren() > 0 )
  {
    for( unsigned int k=0; k<_models->getNumChildren(); k++){
      _models->removeChild(k);
    }
  }

  // Also delete all scribe effects
  while ( _scribeBranch->getNumChildren() > 0 )
  {
    for(unsigned int k=0; k<_scribeBranch->getNumChildren(); k++){
      _scribeBranch->removeChild(k);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete temporary files, create temporary directory
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initTmpDir()
{
#ifdef _MSC_VER
  // Compile-time sanity check.
  const unsigned int bufSize ( 16383 ); // 2^14 - 1
  USUL_STATIC_ASSERT ( bufSize > MAX_PATH );

  char directory[bufSize + 1];
  DWORD result ( ::GetTempPath ( bufSize, directory ) );

  std::string d ( result + "/CadViewer" );

  ::_mkdir ( d.c_str() );

  _tmpDirName = d;
#else
  // If directory name is empty, generate it
  if ( _tmpDirName.size() == 0 )
  {
    _tmpDirName = "/tmp/cv-";
    _tmpDirName += getenv( "USER" );
    _tmpDirName += "/";
  }
  
  struct stat fbuf;
  std::string cmd;
  
  // if directory exists, delete it
  if ( !::stat ( _tmpDirName.c_str(), &fbuf ) )
  {
    // Delete directory and all its subdirectories
    cmd = "rm -r ";
    cmd += _tmpDirName;
    system ( cmd.c_str() );
  }
  
  // Make the directory
  cmd = "mkdir ";
  cmd += _tmpDirName;
  system ( cmd.c_str() );
#endif
}

#if defined (USE_SINTERPOINT)


///////////////////////////////////////////////////////////////////////////////
//
//  Patch node with diff residing in stream nodeDiff
//  On completion nodeDiff contains the new node ready for streamed input
//  Note this makes use of tmp files and system calls for the "patch" command
//
///////////////////////////////////////////////////////////////////////////////

bool Application::_patchNodeWithDiff ( const std::string &nodeName, std::stringstream &nodeDiff )
{
  std::string tmpDiffFileName = _tmpDirName + "cvDiffFile.patch";
  std::string tmpNodeFileName = _tmpDirName + _sinterNodeName + "." + _sinterFileType;
  
  // Open a temporary diffFile and drop the nodeDiff stream data in
  std::ofstream diffFile;
  diffFile.open ( tmpDiffFileName.c_str() );
  if ( !diffFile.is_open() )
  {
    std::cout << "ERROR: diff file " << tmpDiffFileName << " failed to open" << std::endl;
    return false;
  }
  diffFile << nodeDiff.rdbuf();
  diffFile.close();

  // Find the matching node based on name
  Matcher match;
  bool matched=false;
  osg::Node *m = dynamic_cast<osg::Node*>( _models.get() );
  if ( m )
  {
    if ( _recursiveMatchNodeName ( nodeName, m, &match ) )
    {
      // We found a match
      matched=true;
    }
  }

  if ( !matched )
  {
    std::cout << "No matching node found to patch with diff" << std::endl;
    return false;
  }

  // Now patch the node file with the diff
  std::cout << "Patching node file" << std::endl;
  std::string cmd;
  cmd = std::string("patch -l -u ") + tmpNodeFileName + std::string(" ") + tmpDiffFileName;
  system(cmd.c_str());
  
  // Load patched file =====
  
  // reset this boolean
  //_autoPlacement = _prefs->autoPlacement();
  
  // User feedback.
  this->_update ( *_msgText, "Reading model from patched file" );

  // Load in the file
  osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension( _sinterFileType );
  if(!rw){
	  std::ostringstream out;
    out << "Error: " << _sinterFileType << " plugin not available, patched file load will fail.";
    this->_update ( *_msgText, out.str() );
    return false;
  }
  osgDB::ReaderWriter::ReadResult rr = rw->readNode ( tmpNodeFileName );
  if(!rr.validNode()){
	  std::ostringstream out;
    out << "Error: read of patched file failed, resulting node not vaild.";
    this->_update ( *_msgText, out.str() );
    return false;
  }
  NodePtr node = rr.getNode();
  
  this->_replaceNode(node, nodeName);
  
  // Signal user when done
  this->_update ( *_msgText, "Done reading patched model." );
  
  return true;
}

#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Find all animations and turn them on/off
//  Also a recursive functin to find every animation possible under model
//
///////////////////////////////////////////////////////////////////////////////

void Application::_animationsOnOff ( bool onOff, osg::Node *model )
{
  osg::Group *g;
  osg::NodeCallback *nc;
  osg::AnimationPathCallback *apc;
  //osg::AnimationPath *ap;

  // Check to see if we have an animation path
  nc = model->getUpdateCallback();
  if (nc)
  {
    apc = dynamic_cast<osg::AnimationPathCallback*>(nc);
    if (apc)
    {
      apc->setPause(!onOff);
    }
  }
  
  // If model is a group, it may contain more nodes, so search
  g = model->asGroup();
  if ( g ) {
    for ( unsigned int i=0; i<g->getNumChildren(); ++i )
    {
      _animationsOnOff ( onOff, g->getChild(i) );
    }
  }
  
  return;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Step animation by num frames
//
///////////////////////////////////////////////////////////////////////////////

void Application::_animStep ( int num, osg::Node *model )
{ 
  _animModel = model;
  
  _animationsOnOff(true, model);
  
  if(num > 0)
    _anim_steps = num;
  
  return;
}


///////////////////////////////////////////////////////////////////////////////
//
// Setup SinterPoint, if enabled
//
///////////////////////////////////////////////////////////////////////////////

# if defined (USE_SINTERPOINT)

void Application::_sinterPointInit()
{
  _sinterReceiver = NULL;

  // If the machine name is the same as the writer...
  const std::string writer = _prefs->sinterPointWriter();
      std::cout << "Sinter Point Writer machine = " << writer.c_str() << std::endl;

  // Make sure there is a writer-machine.
  ErrorChecker ( 2519309141u, !writer.empty(), 
    "ERROR: No machine specified as the Sinter Point Writer in user-preferences." );

  // Now everyone initializes the SinterAppData
  vpr::GUID newGuid("87f22bd9-61f7-4fa4-bf60-a19953f35d61");
  _sinterAppData.init(newGuid, writer);

  // The writer alone uses sinterpoint
  if( _sinterAppData.isLocal() )
  {
    _sinterReceiver = new sinter::Receiver;
    _sinterReceiver->SetType ( "CADVIEWER2" );
    _sinterReceiver->SetVersion ( osgGetVersion() );
    _sinterReceiver->SetMaxSend ( 1 );

    // Connect to server
    std::string server = _prefs->sinterPointServer();
    int result = _sinterReceiver->Connect ( server.c_str() );
    if (result!=0) 
    {
      std::cout << "ERROR in SinterPoint = " << result << std::endl;
      std::cout << "SinterPoint receiver failed to connect to: " << server.c_str() << std::endl;
      delete _sinterReceiver;
      _sinterReceiver = NULL;
    }
    else
    {
      std::cout << "SinterPoint receiver connected successfully" << std::endl;
    }
  }
  
#if defined (INV3RSION_COLLABORATE)
    _sinterCollabInit();
#endif    

  // Start out looking for commands
  _sinterState = COMMAND;
  _sinterDiffFlag = false;

}

///////////////////////////////////////////////////////////////////////////////
//
//  Save OSG stream to a temp file
//
///////////////////////////////////////////////////////////////////////////////


void Application::_dumpStreamToFile()
{
  // Save model stream to file
  std::string tmpNodeFileName = _tmpDirName + "sinterStream.txt";
  std::ofstream nodeFile;
  nodeFile.open ( tmpNodeFileName.c_str() );
  if ( !nodeFile.is_open() )
  {
    std::cout << "ERROR: node file " << tmpNodeFileName << " failed to open" << std::endl;
  }
  else
  {
    _sinterStream.seekg(0);
    nodeFile << _sinterStream.rdbuf();
    nodeFile.close();
  }
}
    
///////////////////////////////////////////////////////////////////////////////
//
//  Obtain a model across the network with SinterPoint - preFrame portion
//
///////////////////////////////////////////////////////////////////////////////


void Application::_sinterReceiveData()
{    
  // sinter writer only: receive data
  if( _sinterReceiver && _sinterAppData.isLocal() )
  {
    // clear the data buffer -- all processing should be done by the time we
    // get here
    _sinterAppData->_data.clear();

    // The writer obtains the new osg file from Sinterpoint here and sends out
    // application data for the other machines
    int size;
    bool binary=false;

    while( (size = _sinterReceiver->Receive(0,binary)) > 0)
    {
      _sinterTmpString.clear();
      _sinterTmpString.append(_sinterReceiver->Data(),size);
      _sinterTmpString.resize(size);
      _sinterAppData->_data.append(_sinterTmpString);
    }

    // process error codes from negative size
    if(size < 0)
    {
      switch(size)
      {
        case -ETIMEDOUT:
          break;
        case -EPROTONOSUPPORT:
          std::cout << "Error receiving data" << std::endl;
          _dumpStreamToFile();
          break;
        default:
          break;
      }
    }
  }
#if defined (INV3RSION_COLLABORATE)
  _sinterCollabReceiveData();
#endif
}

#endif

#if defined (INV3RSION_COLLABORATE)

///////////////////////////////////////////////////////////////////////////////
//
//  Initialize SinterPoint collaborative members
//
///////////////////////////////////////////////////////////////////////////////

void Application::_sinterCollabInit()
{
    _sinterCollabReceiver = NULL;
    _sinterCollabSender = NULL;
    
    // If the machine name is the same as the writer...
    const std::string writer = _prefs->sinterPointWriter();

    // Make sure there is a writer-machine.
    ErrorChecker ( 2519309141u, !writer.empty(), 
    "ERROR: No machine specified as the Sinter Point Writer in user-preferences." );
    
    // Now everyone initializes the SinterAppData
    vpr::GUID newGuid("26a02fb0-ea01-11da-8ad9-0800200c9a66");
    _sinterCollabData.init(newGuid, writer);

    // The writer alone uses sinterpoint
    if( _sinterCollabData.isLocal() )
    {
      _sinterCollabReceiver = new sinter::Receiver;
      _sinterCollabReceiver->SetType ( "CV-COLLAB" );
      _sinterCollabReceiver->SetVersion ( osgGetVersion() );

      // Connect to server
      std::string server = _prefs->sinterPointServer();
      int result = _sinterCollabReceiver->Connect ( server.c_str() );
      if (result!=0) 
      {
        std::cout << "ERROR in SinterPoint = " << result << std::endl;
        std::cout << "SinterPoint collab receiver failed to connect to: " << server.c_str() << std::endl;
        delete _sinterCollabReceiver;
        _sinterCollabReceiver = NULL;
      }
      else
      {
        std::cout << "SinterPoint receiver connected successfully" << std::endl;
      }

      _sinterCollabSender = new sinter::Sender;
      _sinterCollabSender->SetType( "CV-COLLAB" );
      _sinterCollabSender->SetVersion ( osgGetVersion() );

      // Connect to server
      result = _sinterCollabSender->Connect ( server.c_str() );
      if (result!=0) 
      {
        std::cout << "ERROR in SinterPoint = " << result << std::endl;
        std::cout << "SinterPoint collab sender failed to connect to: " << server.c_str() << std::endl;
        delete _sinterCollabSender;
        _sinterCollabSender = NULL;
      }
      else
      {
        std::cout << "SinterPoint sender connected successfully" << std::endl;
      }
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  SinterPoint send command for avatar & nav data
//
///////////////////////////////////////////////////////////////////////////////

void Application::_sinterSendCommand(std::string &cmd, bool cached)
{
  if( _sinterCollabSender && _sinterCollabData.isLocal() )
  {
    if(cmd[cmd.size()-1] != '\n')
    {
      std::string tmpString = cmd;
      tmpString += "\n";
      if(cached)
        _sinterCollabSender->Send(tmpString.c_str());
      else 
        _sinterCollabSender->SendUnCached(tmpString.c_str());
    }
    else
      _sinterCollabSender->Send(cmd.c_str());
  }
}

void Application::_sinterCollabReceiveData()
{
  // sinter writer only: receive data
  if( _sinterCollabReceiver && _sinterCollabData.isLocal() )
  {
    _sinterCollabData->_data.clear();

    int size;

    while( (size = _sinterCollabReceiver->Receive(0)) > 0)
    {
      _sinterTmpString.clear();
      _sinterTmpString.append(_sinterCollabReceiver->Data(),size);
      _sinterTmpString.resize(size);
      _sinterCollabData->_data.append(_sinterTmpString);
    }

    // process error codes from negative size
    if(size < 0)
    {
      switch(size)
      {
        case -ETIMEDOUT:
          break;
        case -EPROTONOSUPPORT:
          std::cout << "Error receiving data" << std::endl;
          break;
        default:
          break;
      }
    }
  }
}

#endif

#if (INV3RSION_NAV && INV3RSION_COLLABORATE)

///////////////////////////////////////////////////////////////////////////////
//
//  Construct and send a navigation update command
//
///////////////////////////////////////////////////////////////////////////////

void Application::_sinterSendNavUpdate( const float *matrix, const bool cached )
{    
  if( _controlAvatar == _localAvatar )
  {
    std::stringstream ss;

    ss << "CV NAV_MATRIX =";
    for(int i = 0; i < 16; i++)
    {
      ss << " " << matrix[i];
    }
    ss << std::endl;

    std::string cmd = ss.str();
    _sinterSendCommand( cmd, cached );
  }
}


void  Application::_requestNavControl()
{  
  if(_localAvatar)
  {
    std::string cmd = "CV AVATAR_CONTROLLER = ";
    cmd += _localAvatar->name;
    _sinterSendCommand(cmd, true);
  }
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  SinterPoint data processor -- active on all cluster nodes
//
///////////////////////////////////////////////////////////////////////////////

# if defined (USE_SINTERPOINT)

void Application::_sinterProcessData()
{    
  if( !_sinterAppData->_data.empty() )
  {
    // get size of the message we need to process
    int size = _sinterAppData->_data.size();

    int processed_size = 0;

    while ( processed_size < size )
    {         
      // If we are in data receive mode, just keep grabbing data
      if ( _sinterState == DATA )
      {        
        // Avoid the c_str() command in case we have binary data; pointer to beginning of _data is better
        const char *data = &(_sinterAppData->_data)[0] + processed_size;
        int data_size = size - processed_size;

        // if necessary, reduce data size so we don't copy too much data into the stream
        if ( (_sinterStreamSize + data_size) > _sinterDataSize )
        {
          data_size = _sinterDataSize - _sinterStreamSize;
        }

        // write the data to our stream
        _sinterStream.write( data, data_size );
        _sinterStreamSize += data_size;
        _sinterState = DATA;

        std::cout << "." << std::flush;

        // Check to see if we are finished receiving data
        if ( _sinterStreamSize == _sinterDataSize )
        {
          _sinterTime2 = _getClockTime();
          std::cout << "\nOSG node receive completed" << std::endl;
          std::cout << "Total comm time = " << _sinterTime2-_sinterTime1 << std::endl;
          _sinterDataSize = 0;

          // If we received a diff, patch the diff against the proper node file name
          // If successful _sinterStream will contain the patched node file
          if ( _sinterDiffFlag == true )
          {
            _patchNodeWithDiff ( _sinterNodeName, _sinterStream );
            _sinterDiffFlag = false;
          }
          else
          {
            // Load the model into the scene
            this->_streamModel ( _sinterStream, _sinterNodeName );   

            // Save model stream to file for later diff if format is not binary
            if(_sinterFileType != "ive")
            {
              std::string tmpNodeFileName = _tmpDirName + _sinterNodeName + "." + _sinterFileType;
              std::ofstream nodeFile;
              nodeFile.open ( tmpNodeFileName.c_str() );
              if ( !nodeFile.is_open() )
              {
                std::cout << "ERROR: node file " << tmpNodeFileName << " failed to open" << std::endl;
              }
              else
              {
                _sinterStream.seekg(0);
                nodeFile << _sinterStream.rdbuf();
                nodeFile.close();
              }
            }
          }

          _sinterState = COMMAND;
        }

        processed_size += data_size;
      }

      // Otherwise parse for commands
      else
      {
        std::string cmd;
        int cmdStart = _sinterAppData->_data.find ( "CV ", processed_size );
        int cmdEnd = _sinterAppData->_data.find ( "\n", cmdStart );

        // Keep parsing until no more CV commands are found
        if ( cmdStart != std::string::npos && cmdEnd != std::string::npos )
        {
          processed_size += cmdEnd - cmdStart + 1;
          cmdStart += 3;
          cmd.assign ( _sinterAppData->_data, cmdStart, cmdEnd-cmdStart );

          //std::cout << "Got Command: " << cmd << std::endl;

          // Get the node name
          if ( cmd.find ( "NODE_NAME", 0 ) != std::string::npos )
          {
            _sinterNodeName = _getCmdValue(cmd);
            // convert to lowercase to prevent problems with matching later
            std::transform ( _sinterNodeName.begin(), _sinterNodeName.end(), _sinterNodeName.begin(), ::tolower );
            std::cout << "Received node name = " << _sinterNodeName.c_str() << std::endl;
          }

          // Get the data type
          else if ( cmd.find ( "FILE_TYPE", 0 ) != std::string::npos )
          {
            _sinterFileType = _getCmdValue(cmd);
            std::cout << "Received file type = " << _sinterFileType.c_str() << std::endl;
          }

          // Get the node size
          else if ( cmd.find ( "NODE_SIZE", 0 ) != std::string::npos )
          {
            std::stringstream ss;
            ss.str(_getCmdValue(cmd));
            ss >> _sinterDataSize;
            std::cout << "Received node size = " << _sinterDataSize << std::endl;
          }

          // The next node data transfer will be a diff
          else if ( cmd.find ( "NODE_DIFF", 0 ) != std::string::npos )
          {
            _sinterDiffFlag = true;
            std::cout << "Next node file will be a diff for node: " << _sinterNodeName << std::endl;
          }

          // Enter receive data mode
          else if ( cmd.find ( "NODE_DATA", 0 ) != std::string::npos )
          {
            if ( _sinterDataSize > 0 )
            {
              _sinterStream.clear();
              _sinterStream.str("");
              _sinterStreamSize = 0;
              _sinterState = DATA;
              std::cout << "Begin receive of osg node..." << std::endl;
              _sinterTime1 = _getClockTime();
            }
            else
            {
              std::cout << "Data Size Zero; Not entering Data Receive Mode." << std::endl;
            }
          }

          // Delete the whole scene
          else if ( cmd.find ( "CLEARALL", 0 ) != std::string::npos )
          {
            std::cout << "Deleting all models in scene" << std::endl;
            _deleteScene();
            _initTmpDir();
          }

          else
          {
            std::cout << "Warning: Unrecognized CadViewer Command." << std::endl;
          }

        }
        else
        {
          std::cout << "Warning: CadViewer Command Not Found" << std::endl;
        }
      }
    }
  }
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Navigation -- auto place & size model
//
///////////////////////////////////////////////////////////////////////////////

void Application::_doAutoPlacement( const bool replace_matrix )
{  
  // first remove any transforms to the model
  osg::Matrixf idMatrix;
  idMatrix.identity();
  _models->setMatrix(idMatrix);
  
  // get _models bounding sphere and translate & scale accordingly
  const osg::BoundingSphere &sphere = _models->getBound();

  // center the models
  osg::Matrixf autoPlaceXform;
  osg::Matrixf tempMatrix;
  
  // scale model only, not grid
  const float scale = _prefs->autoPlaceRadius() / sphere.radius();
  tempMatrix.makeScale( scale, scale, scale );
  _models->preMult( tempMatrix );
    
  // translate to origin first
  const osg::Vec3 centertrans =  sphere.center() * -scale;
  autoPlaceXform.makeTranslate( centertrans );
  
  // next apply rotation
  Usul::Math::Matrix44f m;
  m.makeRotation ( _prefs->autoRotationAngle(), _prefs->autoRotationVector() );
  osg::Matrixf mm;
  OsgTools::Convert::matrix (m, mm );
  autoPlaceXform.postMult(mm);
  
  // next translate to auto-center position
  const Preferences::Vec3f &ac = _prefs->autoPlaceCenter();
  const osg::Vec3 actrans =  osg::Vec3 ( ac[0], ac[1], ac[2] );
  tempMatrix.makeTranslate( actrans );
  autoPlaceXform.postMult( tempMatrix );
  
  // either replace the nav matrix or multiply the current one
  if( replace_matrix )
    _navBranch->setMatrix( autoPlaceXform );
  else
    _navBranch->postMult( autoPlaceXform );

#if defined (INV3RSION_NAV)
  // not sure why this works without a pre-transform
  // TODO: look into this
/*  osg::Matrixf m1;
  m1.makeTranslate( sphere.center() * -scale );
  _invrNav->SetPreTransform(m1.ptr());*/

  osg::Matrixf m2;
  //m2.makeTranslate( actrans );
  m2.makeTranslate( sphere.center() * -scale );
  _invrNav->SetPostTransform(m2.ptr());

#if (INV3RSION_NAV && INV3RSION_COLLABORATE)
    const float *matptr = _invrNav->GetPostTransform();
    std::stringstream ss;

    ss << "CV NAV_POSTXFORM =";
    for(int i = 0; i < 16; i++)
    {
      ss << " " << matptr[i];
    }
    ss << std::endl;

    // ss << "CV NAV_MODEL_SCALE = " << scale << std::endl;

    std::string cmd = ss.str();
    _sinterSendCommand( cmd, true );
#endif  
  _syncInvrNav();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Drawing -- draw extra geometry
//
///////////////////////////////////////////////////////////////////////////////

void Application::draw()
{
  osgVRJ::Application::draw();   // base class draw method

#if defined (INV3RSION_COLLABORATE)
  if(!_avatars.empty())
  {
    glEnable(GL_DEPTH_TEST);
    
    for(unsigned int i = 0; i < _avatars.size(); i++)
    {
      if ((_avatars[i] != NULL) && (_avatars[i] != _localAvatar) &&
          (_avatars[i]->avatar != NULL) && _avatars[i]->visible &&
          (_avatars[i]->avatar->isReady()))
      {
        glPushMatrix();
        
        //Render mesh version
        if ( !_avatars[i]->avatar->renderMesh(vjAvatar::DrawMesh) )
           std::cout << "Error: Avatar " << i << " did not render" << std::endl;
        
        // Render controller icon
        if( _avatars[i] == _controlAvatar )
        {
          CalBone* wrist_bone = _avatars[i]->avatar->getBone( "R_WRIST" );
          const CalVector wrist_pos = wrist_bone->getTranslationAbsolute();
                    
          glEnable(GL_BLEND);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glPushMatrix();          
          glTranslatef(_avatars[i]->avatar->getPos()[0], 
                       _avatars[i]->avatar->getPos()[1], 
                       _avatars[i]->avatar->getPos()[2]);
          glRotatef(_avatars[i]->bodyYaw, 0.0, 1.0, 0.0);
          glTranslatef(-wrist_pos.x, wrist_pos.z, wrist_pos.y);
          glScalef(0.35, 0.35, 0.35);
          glColor4f(1.0, 0.0, 0.0, 0.45);
          invr::draw::Sphere();
          glPopMatrix();
        }
        
        glPopMatrix();
      }
    }
  }
#endif

#if defined (INV3RSION_NAV)
  _invrNav->draw(); // Nav draw method
#endif
}

#if defined (INV3RSION_NAV)

///////////////////////////////////////////////////////////////////////////////
//
//  Navigation -- set INVR nav matrix to current nav matrix
//
///////////////////////////////////////////////////////////////////////////////

void Application::_syncInvrNav()
{
  // set Nav transform to match matrix from model load
  const double *matptr = _navBranch->getMatrix().ptr();
  float matfloat[16];
  for(int i = 0; i < 16; i++) { matfloat[i] = (float) matptr[i]; }  
  _invrNav->SetTransform(matfloat);
  
#if defined (INV3RSION_COLLABORATE)
  _sinterSendNavUpdate( _invrNav->GetTransform()->mData, true );
#endif
}

#endif

#if defined (INV3RSION_COLLABORATE)

///////////////////////////////////////////////////////////////////////////////
//
//  Send Command to add an avatar to the scene
//
///////////////////////////////////////////////////////////////////////////////

void Application::_sendAddAvatarCommand( const std::string &filename, const std::string &name )
{
  ErrorChecker ( 1067093696u, isAppThread(), CV::NOT_APP_THREAD );
  
  std::string cmd = "CV AVATAR_NEW = ";
  cmd += filename;
  cmd += " ";
  cmd += name;
  _sinterSendCommand(cmd, true); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add an avatar to the scene
//
///////////////////////////////////////////////////////////////////////////////

void Application::_addAvatar ( const std::string &filename, const std::string &name )
{
  ErrorChecker ( 1067093696u, isAppThread(), CV::NOT_APP_THREAD );
  
  if(!_avatarFactory)
  {    
    _avatarFactory = new vjAvatarFactory;
    
    //First, check that certain environment variables are set for data loading
    char* data_dir = getenv( "AVATAR_DATA_DIR" );
    if (data_dir == NULL)
    {
      std::cout << "Warning: Must set AVATAR_DATA_DIR environment to enable loading of avatars." << std::endl;
    }
    else
    {
      _avatarFactory->addPath( std::string( data_dir ) );
    }
  }
  
  std::cout << "Loading avatar config: " << filename << std::endl;
  
  //Add the avatar to the factory
  if (!_avatarFactory->addAvatar( filename, name )) return;

  // Create new avatar instantiation
  vjAvatar* currentAvatar = _avatarFactory->newAvatar( name ) ;

  //Set up position/orientation/scaling
  currentAvatar->setWCS( gmtl::EulerAngleXYZf(gmtl::Math::deg2Rad(-90.0f),0,0) ) ; //Convert to Juggler coordinates
  currentAvatar->setRot( gmtl::EulerAngleXYZf(0,0,0) );
  currentAvatar->setPos( gmtl::Vec3f(0,0,0));
  
  // trigger the first animation
  currentAvatar->triggerAnimationCycle("IDLE");
  
  AvatarData *currentData = new AvatarData( name, currentAvatar );
  
  if( name == _localAvatarName ) _localAvatar = currentData;
  
  _avatars.push_back(currentData);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load a local avatar from command-line
//
///////////////////////////////////////////////////////////////////////////////

void Application::_registerAvatar ( const std::string &filename )
{  
  // strip out anything before the filename
  int pos = filename.rfind("/", filename.size());
  
  if(pos == std::string::npos) _localAvatarFileName = filename;
  else _localAvatarFileName = filename.substr( pos + 1, filename.size() - pos );
  
  _localAvatarName = _prefs->userName();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate values for avatar tracking
//
///////////////////////////////////////////////////////////////////////////////

void Application::_updateLocalAvatar()
{
  if(_localAvatar)
  {  
    AvatarData *avatar = _localAvatar;

    //Get head data
    gmtl::Matrix44f head_matrix;
    head_matrix.set ( _headTracker->matrix().getData() );

    // Get head position data for body position
    avatar->bodyPos = gmtl::makeTrans<gmtl::Vec3f>(head_matrix);
    avatar->bodyPos[1] = 0.0;

    //Get the head yaw data
    gmtl::Vec4f transZero(0.0, 0.0, 0.0, 1.0);
    gmtl::Vec4f unit(1.0, 0.0, 0.0, 1.0);
    gmtl::Matrix44f head_rot = head_matrix;    
    gmtl::setTrans(head_rot, transZero);
    gmtl::xform(transZero, head_rot, unit);
    float head_yaw = gmtl::Math::rad2Deg( atan2f( -transZero[2], transZero[0] ) ) - avatar->bodyYaw;
    
    // filter out change of sign in the angle    
    if( head_yaw > _prevHeadYaw + 300 )
      _headYawOffset -= 360.0;    
    else if( head_yaw < _prevHeadYaw - 300 )
      _headYawOffset += 360.0; 
    
    _prevHeadYaw = head_yaw;
    
    head_yaw += _headYawOffset;
    
        
    //Get the shoulder and hand data
    //This is the position of the shoulder relative to the head position
    gmtl::Vec3f shoulder_trans(0.8, -0.6, 0.0);
    gmtl::Matrix44f shoulder_relative_pos = gmtl::makeTrans<gmtl::Matrix44f>( shoulder_trans );

    //Rotate the shoulder_relative position as to the current yaw of the 
    //body
    gmtl::Matrix44f temp_rot = gmtl::make<gmtl::Matrix44f>( gmtl::EulerAngleXYZf(0.0f, gmtl::Math::deg2Rad(avatar->bodyYaw), 0.0f));

    gmtl::preMult( shoulder_relative_pos, temp_rot );

    gmtl::Vec3f head_trans = gmtl::makeTrans<gmtl::Vec3f>( head_matrix );

    shoulder_trans = gmtl::makeTrans<gmtl::Vec3f>(shoulder_relative_pos);

    gmtl::Vec3f shoulder_pos = head_trans + shoulder_trans;

    // set the centering
    float body_center_yaw = avatar->bodyYaw + head_yaw;

    float yaw_rate = powf(fabs(avatar->bodyYaw - body_center_yaw), 1.5) * _bodyMaxYawRate;

    //Rotate the body if it is not centered
    if (avatar->bodyYaw > body_center_yaw)
    {
      avatar->bodyYaw -= yaw_rate;
      if(avatar->bodyYaw < body_center_yaw) avatar->bodyYaw = body_center_yaw;
    }
    else
    {
      avatar->bodyYaw += yaw_rate;
      if(avatar->bodyYaw > body_center_yaw) avatar->bodyYaw = body_center_yaw;
    }

    //Adjust the head's rotation for the body's current rotation   
    //Figure how much to rotate the head_matrix (same as inverse of body rotation)
    temp_rot = gmtl::make<gmtl::Matrix44f>( gmtl::EulerAngleXYZf(0.0f, gmtl::Math::deg2Rad(-avatar->bodyYaw), 0.0f));

    gmtl::preMult(head_matrix, temp_rot) ;

    //Get the head matrix to make a quaternion representation of it
    //First extract the euler angles from the head matrix
    gmtl::EulerAngleXYZf head_eulers = gmtl::make<gmtl::EulerAngleXYZf>( head_matrix );

    avatar->headQuat = gmtl::make<gmtl::Quatf>( head_eulers );

    //Convert to the head's coordinate system
    gmtl::Quatf temp_quat = avatar->headQuat;
    avatar->headQuat[0] = -temp_quat[1];
    avatar->headQuat[1] = temp_quat[0];
    avatar->headQuat[2] = -temp_quat[2];

    //Find the hand position and rotation,.etc.
    gmtl::Matrix44f hand_matrix;
    hand_matrix.set ( _tracker->matrix().getData() );

    //Rotate it the same as the current body yaw
    gmtl::Vec3f hand_pos = gmtl::makeTrans<gmtl::Vec3f>( hand_matrix );

    hand_pos -= shoulder_pos;

    gmtl::Matrix44f hand_mat = gmtl::makeTrans<gmtl::Matrix44f>( hand_pos );

    gmtl::preMult( hand_mat, temp_rot );
    hand_pos = gmtl::makeTrans<gmtl::Vec3f>(hand_mat);

    gmtl::normalize(hand_pos);

    //Get the shortest arc quaternion from origin_vec to hand_vec
    avatar->handQuat = gmtl::makeRot<gmtl::Quatf>( gmtl::Vec3f(1,0,0), hand_pos);

    // transmit avatar data to SinterPoint server
    if ( _avatarWaitCount == 0 )
    {
      std::stringstream ss;

      ss << "CV AVATAR_BODY_YAW = " << avatar->name << " " << avatar->bodyYaw << std::endl;

      ss << "CV AVATAR_BODY_POS = " << avatar->name << " " 
         << avatar->bodyPos[0] << " " << avatar->bodyPos[1] << " " << avatar->bodyPos[2] << std::endl;

      ss << "CV AVATAR_HEAD_QUAT = " << avatar->name << " " 
         << avatar->headQuat[0] << " " << avatar->headQuat[1] << " " 
         << avatar->headQuat[2] << " " << avatar->headQuat[3] << std::endl;

      ss << "CV AVATAR_HAND_QUAT = " << avatar->name << " " 
         << avatar->handQuat[0] << " " << avatar->handQuat[1] << " " 
         << avatar->handQuat[2] << " " << avatar->handQuat[3] << std::endl;

      std::string cmd = ss.str();
      _sinterSendCommand( cmd, false );
      _avatarWaitCount = _prefs->avatarWaitFrames();
    }
    else
    {
      _avatarWaitCount--;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Per-frame update of avatars
//
///////////////////////////////////////////////////////////////////////////////

void Application::_updateAvatars()
{
  if(_avatars.empty()) return;
  
  double timeNow = _tracker->time();
  double timeElapsed = timeNow - _avatarTime;
  _avatarTime = timeNow;
  
  _updateLocalAvatar(); // update local data
  
  for (unsigned int i=0; i < _avatars.size(); i++)
  {
    if(_avatars[i]->visible)
    {
      _avatars[i]->avatar->updateAnimations(timeElapsed); 

      //Rotate body based on the current body yaw
      gmtl::EulerAngleXYZf body_rot = _avatars[i]->avatar->getRotXYZ();
      body_rot[1] = gmtl::Math::deg2Rad(_avatars[i]->bodyYaw+180);
      _avatars[i]->avatar->setRot( body_rot );

      _avatars[i]->avatar->updateMove(timeElapsed);

      // Translate to current head pos in X-Z
      _avatars[i]->avatar->setPos( _avatars[i]->bodyPos );

      CalBone* head_bone = _avatars[i]->avatar->getBone( "NECK" );
      if (head_bone != NULL)
      {
        //Set the head's angle of rotation
        head_bone->setCoreStateRecursive() ;
        head_bone->setRotation( getCalQuat( _avatars[i]->headQuat ) ) ;
        head_bone->calculateState();
      }

      //Get the right shoulder bone
      CalBone* right_shoulder_bone = _avatars[i]->avatar->getBone( "R_SHOULDER" );
      if (right_shoulder_bone != NULL)
      {
        right_shoulder_bone->setCoreStateRecursive() ;
        right_shoulder_bone->setRotation( getCalQuat( _avatars[i]->handQuat ) ) ;
        right_shoulder_bone->calculateState();
      }

      _avatars[i]->avatar->updateMesh(timeElapsed);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Search avatar list for named avatar
//
///////////////////////////////////////////////////////////////////////////////

int Application::_getAvatarIndexByName(std::string &name)
{
  if(_avatars.empty()) return -1;
  
  for ( int i = 0; i < _avatars.size(); i++ )
  {
    if( name == _avatars[i]->name )
      return i;
  }
  return -1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process collaboration data from SinterPoint
//
///////////////////////////////////////////////////////////////////////////////

void Application::_sinterProcessCollabData()
{    
  if( !_sinterCollabData->_data.empty() )
  {
    // get size of the message we need to process
    int size = _sinterCollabData->_data.size();

    int processed_size = 0;

    while ( processed_size < size )
    {         
      // parse for commands
      std::string cmd;
      int cmdStart = _sinterCollabData->_data.find ( "CV ", processed_size );
      int cmdEnd = _sinterCollabData->_data.find ( "\n", cmdStart );

      // Keep parsing until no more CV commands are found
      if ( cmdStart != std::string::npos && cmdEnd != std::string::npos )
      {
        processed_size += cmdEnd - cmdStart + 1;
        cmdStart += 3;
        cmd.assign ( _sinterCollabData->_data, cmdStart, cmdEnd-cmdStart );

        // Add an avatar
        if ( cmd.find ( "AVATAR_NEW", 0 ) != std::string::npos )
        {
          std::string subcmd = _getCmdValue(cmd);
          int pos = 0;
          std::string filename = _getCmdToken(subcmd, " ", pos);
          std::string name = _getCmdToken(subcmd, " ", pos);
          std::cout << "Adding New Avatar: " << name << std::endl;
          
          int i = _getAvatarIndexByName(name);
          if( i != -1 && _avatars[i] != _localAvatar )
          {
            _avatars[i]->visible = true;
          }
          else
          {
            _addAvatar ( filename, name );
          }
        }

        else if ( cmd.find ( "AVATAR_DELETE", 0 ) != std::string::npos )
        {
          std::string name = _getCmdValue(cmd);
          int i = _getAvatarIndexByName(name);
          if( i != -1 && _avatars[i] != _localAvatar )
          {
            std::cout << "Removing Avatar: " << _avatars[i]->name << std::endl;
            if(_controlAvatar == _avatars[i]) _controlAvatar = NULL;
            _avatars[i]->visible = false;
          }
        }

        else if( cmd.find ( "AVATAR_BODY_YAW", 0 ) != std::string::npos )
        {
          std::string subcmd = _getCmdValue(cmd);
          int pos = 0;
          std::string name = _getCmdToken(subcmd, " ", pos);
          int i = _getAvatarIndexByName(name);
          if( i != -1 && _avatars[i] != _localAvatar )
          {
            std::string yaw = _getCmdToken(subcmd, " ", pos);
            _avatars[i]->bodyYaw = atof(yaw.c_str());
          }
        }

        else if( cmd.find ( "AVATAR_BODY_POS", 0 ) != std::string::npos )
        {
          std::string subcmd = _getCmdValue(cmd);
          int pos = 0;
          std::string name = _getCmdToken(subcmd, " ", pos);
          int i = _getAvatarIndexByName(name);
          if( i != -1 && _avatars[i] != _localAvatar )
          {
            _avatars[i]->bodyPos[0] = atof( _getCmdToken(subcmd, " ", pos).c_str() );
            _avatars[i]->bodyPos[1] = atof( _getCmdToken(subcmd, " ", pos).c_str() );
            _avatars[i]->bodyPos[2] = atof( _getCmdToken(subcmd, " ", pos).c_str() );
          }
        }

        else if( cmd.find ( "AVATAR_HEAD_QUAT", 0 ) != std::string::npos )
        {
          std::string subcmd = _getCmdValue(cmd);
          int pos = 0;
          std::string name = _getCmdToken(subcmd, " ", pos);
          int i = _getAvatarIndexByName(name);
          if( i != -1 && _avatars[i] != _localAvatar )
          {
            _avatars[i]->headQuat[0] = atof( _getCmdToken(subcmd, " ", pos).c_str() );
            _avatars[i]->headQuat[1] = atof( _getCmdToken(subcmd, " ", pos).c_str() );
            _avatars[i]->headQuat[2] = atof( _getCmdToken(subcmd, " ", pos).c_str() );
            _avatars[i]->headQuat[3] = atof( _getCmdToken(subcmd, " ", pos).c_str() );
          }
        }

        else if( cmd.find ( "AVATAR_HAND_QUAT", 0 ) != std::string::npos )
        {
          std::string subcmd = _getCmdValue(cmd);
          int pos = 0;
          std::string name = _getCmdToken(subcmd, " ", pos);
          int i = _getAvatarIndexByName(name);
          if( i != -1 && _avatars[i] != _localAvatar )
          {
            _avatars[i]->handQuat[0] = atof( _getCmdToken(subcmd, " ", pos).c_str() );
            _avatars[i]->handQuat[1] = atof( _getCmdToken(subcmd, " ", pos).c_str() );
            _avatars[i]->handQuat[2] = atof( _getCmdToken(subcmd, " ", pos).c_str() );
            _avatars[i]->handQuat[3] = atof( _getCmdToken(subcmd, " ", pos).c_str() );
          }
        }

        else if ( cmd.find ( "AVATAR_CONTROLLER", 0 ) != std::string::npos )
        {
          std::string name = _getCmdValue(cmd);
          int i = _getAvatarIndexByName(name);
          if( i != -1 )
          {
            if( _controlAvatar == _localAvatar )
            {
#if defined (INV3RSION_NAV)
              _invrNav->SetMode(invr::nav::CAD::NO_NAV);
#endif
            }
            _controlAvatar = _avatars[i];
          }
        }

        else if ( cmd.find ( "AVATAR_CONTROL_RELEASE", 0 ) != std::string::npos )
        {
          _controlAvatar = NULL;
        }

#if defined (INV3RSION_NAV)

        else if ( cmd.find ( "NAV_MATRIX", 0 ) != std::string::npos )
        {
          if( _controlAvatar != _localAvatar )
          {
            float matfloat[16];
            std::string subcmd = _getCmdValue(cmd);
            int pos = 0;
            for(int i = 0; i < 16; i++)
            {
              matfloat[i] = atof( _getCmdToken(subcmd, " ", pos).c_str() );
            }

            _invrNav->SetTransform(matfloat);
          }
        }

        else if ( cmd.find ( "NAV_POSTXFORM", 0 ) != std::string::npos )
        {
          float matfloat[16];
          std::string subcmd = _getCmdValue(cmd);
          int pos = 0;
          for(int i = 0; i < 16; i++)
          {
            matfloat[i] = atof( _getCmdToken(subcmd, " ", pos).c_str() );
          }

          _invrNav->SetPostTransform(matfloat);
        }

        else if ( cmd.find ( "NAV_MODEL_SCALE", 0 ) != std::string::npos )
        {
          std::string subcmd = _getCmdValue(cmd);
          const float scale = atof( subcmd.c_str() );

          osg::Matrixf tempMatrix;
          tempMatrix.makeScale( scale, scale, scale );
          _models->preMult( tempMatrix );
        }

#endif

        else
        {
          std::cout << "Warning: Unrecognized CadViewer Collab Command." << std::endl;
        }

      }
      else
      {
        std::cout << "Warning: CadViewer Collab Command Not Found" << std::endl;
      }
    }
  }
}

#endif
