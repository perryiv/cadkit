
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
#include "Usul/CommandLine/Parser.h"
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
#include "osgFX/Scribe"

#include "osgDB/ReadFile"
#include "osgDB/WriteFile"

#include "OsgTools/Axes.h"
#include "OsgTools/Text.h"
#include "OsgTools/Group.h"
#include "OsgTools/Convert.h"
#include "OsgTools/Font.h"
#include "OsgTools/State.h"
#include "OsgTools/Visitor.h"

#include "MenuKit/MemFunCallback.h"


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
#if 0
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
#endif
  const unsigned long NAVIGATE_TOGGLE = BUTTON_YELLOW;
  const unsigned long STOP_NAV_TOOL   = BUTTON_RED;       // helps greatly in sim mode
  const unsigned long NAVIGATE_FLY    = BUTTON_BLUE;      // Joseph doesn't need it, but its handy if anybody else does
  const unsigned long TOOL_SCALE      = BUTTON_GREEN;
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
  _scribeBranch   ( new osg::Group )
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
  _models->addChild ( _scribeBranch.get() );

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
  for(int i=0; i<_gridFunctors.size(); ++i){
    delete _gridFunctors[i];
  }
  _gridFunctors.clear();
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

  // Parse the command-line arguments.
  this->_parseCommandLine();

  // Move the model-group so that the models are centered and visible.
  this->viewAll ( _models.get(), _prefs->viewAllScaleZ() );

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
  
  // Setup Sinterpoint if enabled
# if defined (USE_SINTERPOINT)
    this->_sinterPointInit();
# endif
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
  mt->setReferenceFrame ( osg::Transform::RELATIVE_RF );
  mt->setMatrix ( osg::Matrix::identity() );

  // Make the text branch an orthographic projection.
  float x ( _vp[0] );
  float y ( _vp[1] );
  float w ( _vp[2] );
  float h ( _vp[3] );
  _textBranch->setMatrix ( osg::Matrix::ortho2D ( x, w, y, h ) );

  // Set the text font.
  std::string f ( OsgTools::Font::filename ( "courbd" ) ); // Courier New Bold
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
  for(i=0; i<_gridFunctors.size(); ++i){
    delete _gridFunctors[i];
  }
  _gridFunctors.clear();

  // Set the properties.
  for(i=0; i<_prefs->numGrids(); ++i){
    OsgTools::Grid *grid = new OsgTools::Grid();
    grid->numBlocks ( _prefs->numGridBlocks(i) );
    grid->size ( r * _prefs->gridScale(i) );
    grid->color ( _prefs->gridColor(i) );
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
  for(int i=0; i<_gridFunctors.size(); ++i){
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
  //OsgTools::State::setPolygonsFilled ( _models.get(), false );
  OsgTools::State::setPolygonsSmooth ( _models.get() );
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
    this->_sinterReceiveModelPreFrame();
# endif
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

  // Process pressed states.
  if ( COMMAND_SELECT == _buttons->down() )
  {
    this->_intersect();
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

  unsigned long mode = _buttons->released();
  if ( eventRequest )                                         // if mode NOT specified with function call ...
    mode = eventRequest;                                      // ... get information from buttons

  switch ( mode )                                             // which button was used???
  {
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

  if ( _buttons->released() == TOOL_SCALE )                   // BUTTON_GREEN
  {
    std::cout << "SCALE" << std::endl;
    //_sceneTool=0x0;                                         // invalidate scale tool to prevent toggling off
    this->_vScaleWorld ( tool_message, tool_item );           // if previous line commented, toggle scale; if not, activate scale
    handled = true;                                           // button event has been handled
  }

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

  if ( _buttons->released() == STOP_NAV_TOOL )                // BUTTON_RED
  {
    std::cout << "STOP NAVIGATION/TOOLS" << std::endl;
    _navigatorH = 0x0;                                        // invalidate response to horizontal joystick
    _navigatorV = 0x0;                                        // invalidate response to vertical joystick
    _sceneTool  = 0x0;                                        // invalidate scale tool to prevent toggling off

    this->_removeCursorChildren();

    OsgTools::Axes walk_fly;
    walk_fly.state( OsgTools::Axes::POSITIVE_X |
                    OsgTools::Axes::NEGATIVE_X |
                    OsgTools::Axes::POSITIVE_Z |
                    OsgTools::Axes::NEGATIVE_Z );
    _cursorYellowNoRot->addChild( walk_fly() );
    _cursorBlueWithRot->addChild( walk_fly() );

    _cursorGreenWithRot->addChild( _cursor_zoom.get() );

    handled = true;                                           // button event has been handled
  }

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

  // Last steps after application data sync
# if defined (USE_SINTERPOINT)
    this->_sinterReceiveModelPostFrame();
# endif

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
    OsgTools::State::setNormalize ( node.get(), norm );

  // Make a matrix transform for this model.
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  mt->setName ( std::string ( "Branch for: " ) + filename );

  // Set its matrix.
  osg::Matrixf M;
  OsgTools::Convert::matrix ( matrix, M );
  mt->setMatrix ( M );

  // Set its name to the filename if there is no name.
  if ( node->getName().empty() )
    node->setName ( filename );

  // Create the scribe effect since it must attach to the model
  osg::ref_ptr<osgFX::Scribe> sc = new osgFX::Scribe;
  sc->setWireframeColor ( osg::Vec4 ( 1.0,1.0,1.0,1.0 ) );
  sc->addChild ( node.get() );
  _scribeBranch->addChild ( sc.get() );
  
  // Hook things up.
  mt->addChild ( node.get() );

  // Hook things up.
  _models->addChild ( mt.get() );

  // Do any post-processing.
  this->_postProcessModelLoad ( filename, node.get() );
}


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
  loader ( dir + "sim.base.config" );
  loader ( dir + "sim.wand.mixin.config" );
  loader ( dir + "sim.analog.mixin.config" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elapsed time since the program started (in seconds).
//
///////////////////////////////////////////////////////////////////////////////

double Application::_getElapsedTime() const
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
  _clipDist[1] = 4 * radius;
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
//  Static variables necessary for use with the sinterpoint callback
//
///////////////////////////////////////////////////////////////////////////////

# if defined (USE_SINTERPOINT)
    std::string     Application::_sinterFileData;
    vpr::Mutex      Application::_sinterMutex;
    SinterFileState Application::_sinterFileState;
# endif


///////////////////////////////////////////////////////////////////////////////
//
// Setup SinterPoint, if enabled
//
///////////////////////////////////////////////////////////////////////////////

# if defined (USE_SINTERPOINT)

  void Application::_sinterPointInit()
  {
    // If the machine name is the same as the writer...
    const std::string host    ( Usul::System::Host::name() );
    const std::string writer = _prefs->sinterPointWriter();

    // Make sure there is a writer-machine.
    ErrorChecker ( 2519309141u, !writer.empty(), 
      "No machine specified as the Sinter Point Writer in user-preferences." );

    // The writer alone uses sinterpoint
    if ( host == writer )
    {
      _sinterReceiver.SetType ( "DEMO" );
      _sinterReceiver.SetVersion ( "1.0" );
      _sinterReceiver.SetMaxSend ( 1 );

      // Connect to server
      std::string server = _prefs->sinterPointServer();
      int result = _sinterReceiver.Connect ( server.c_str() );
      if (result!=0) 
      {
        std::cout << "ERROR in SinterPoint = " << result << std::endl;
        std::cout << "      SinterPoint reciever failed to connect to: " << server.c_str() << std::endl;
        std::cout << "      SinterPoint communication will be unusable" << std::endl;
      }
      else{
        // give function pointer to receiver
        _sinterReceiver.OnReceive ( &_sinterCallback ); 	  

        std::cout << "SinterPoint connected successfully" << std::endl;
      }

      // Start out doing nothing
      _sinterFileState = NOTHING;
    }

    // Now everyone initializes the SinterAppData
    vpr::GUID newGuid("87f22bd9-61f7-4fa4-bf60-a19953f35d61");
    _sinterAppData.init(newGuid);
  }

# endif

    
///////////////////////////////////////////////////////////////////////////////
//
//  Obtain a model across the network with SinterPoint - preFrame portion
//
///////////////////////////////////////////////////////////////////////////////

# if defined (USE_SINTERPOINT)

  void Application::_sinterReceiveModelPreFrame()
  {
    ErrorChecker ( 2346088800u, isAppThread(), CV::NOT_APP_THREAD );

    // If the machine name is the same as the writer...
    const std::string host    ( Usul::System::Host::name() );
    const std::string writer = _prefs->sinterPointWriter();

    // Make sure there is a writer-machine.
    ErrorChecker ( 2519309141u, !writer.empty(), 
      "No machine specified as the Sinter Point Writer in user-preferences." );

    // The writer obtains the new osg file from Sinterpoint and writes it out
    if ( host == writer )
    {
      int length;

      // Share the state
      _sinterAppData->_state = _sinterFileState;

      if ( _sinterFileState == RECEIVING )
      {
        // Keep grabbing data from SinterPoint and writing it out
        if ( !_sinterFile.is_open() ){
          _sinterFile.open ( _prefs->sinterPointTmpFile().c_str() );
        }
        _sinterMutex.acquire();
        length = _sinterFileData.length();
        if ( length!=0 )
        {
          // Share the data across the cluster
          _sinterAppData->_data = _sinterFileData;
          
          // Write the data to a file
          _sinterFile.write ( _sinterFileData.c_str(), length );
          _sinterFileData.clear();
        }
        _sinterMutex.release();
      }

      else  if ( _sinterFileState == DONE )
      {
        // Grab the last data chunk, if needed, write it out then close
        length = _sinterFileData.length();
        if ( length!=0 )
        {
          // Share the data across the cluster
          _sinterAppData->_data = _sinterFileData;
          
          // Write the data to a file
          _sinterFile.write ( _sinterFileData.c_str(), length );
          _sinterFileData.clear();
        }
        _sinterFile.close();
      }
    }

    // Other machines simply receive ApplicationData and write it out
    else
    {
      // Get the data and state
      _sinterFileData = _sinterAppData->_data;
      _sinterFileState = static_cast<SinterFileState>(_sinterAppData->_state);
    }
  }

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Obtain a model across the network with SinterPoint - postFrame portion
//
///////////////////////////////////////////////////////////////////////////////

# if defined (USE_SINTERPOINT)

  void Application::_sinterReceiveModelPostFrame()
  {
    ErrorChecker ( 2346088801u, isAppThread(), CV::NOT_APP_THREAD );

    // If the machine name is the same as the writer...
    const std::string host    ( Usul::System::Host::name() );
    const std::string writer = _prefs->sinterPointWriter();

    // Make sure there is a writer-machine.
    ErrorChecker ( 2519309142u, !writer.empty(), 
      "No machine specified as the Sinter Point Writer in user-preferences." );

    // The writer will just load the file
    if ( host == writer )
    {
      if ( _sinterFileState == DONE ){
        // Load the completed file
        this->_loadModelFile ( _prefs->sinterPointTmpFile().c_str() );

        // Finished
        _sinterFileState = NOTHING;
      }
    }

    // Other machines simply receive ApplicationData and write it out
    else
    {
      int length;

      // Get the data and state
      _sinterFileData = _sinterAppData->_data;
      _sinterFileState = static_cast<SinterFileState>(_sinterAppData->_state);

      if ( _sinterFileState == RECEIVING ) {
        // Keep grabbing data from SinterPoint and writing it out
        if ( !_sinterFile.is_open() ){
          _sinterFile.open ( _prefs->sinterPointTmpFile().c_str() );
        }
        length = _sinterFileData.length();
        if ( length!=0 )
        {
          // Write the data to a file
          _sinterFile.write ( _sinterFileData.c_str(), length );
          _sinterFileData.clear();
        }
      }

      else if (_sinterFileState == DONE) {
        // Grab the last data chunk, if needed, write it out then close
        length = _sinterFileData.length();
        if ( length!=0 )
        {
          // Write the data to a file
          _sinterFile.write ( _sinterFileData.c_str(), length );
          _sinterFileData.clear();
        }
        _sinterFile.close();

        // Now load
        this->_loadModelFile ( _prefs->sinterPointTmpFile().c_str() );
      }
    }
  }

#endif



///////////////////////////////////////////////////////////////////////////////
//
//  SinterPoint callback to obtain a model across the network
//
///////////////////////////////////////////////////////////////////////////////

# if defined (USE_SINTERPOINT)

  int Application::_sinterCallback ( const char *data, const int size )
  {
    _sinterMutex.acquire();
    std::string msg = data;

    // Clean off the trailing characters SinterPoint tends to leave on
    msg.erase ( size, msg.size() );

    if ( msg=="EOFEOFEOF" )
    {
      _sinterFileState = DONE;
    }
    else
    {
      // Add the newline back on and append to the Data
      msg += "\n";
      _sinterFileData += msg;
      _sinterFileState = RECEIVING;
    }

    _sinterMutex.release();
  }

#endif


