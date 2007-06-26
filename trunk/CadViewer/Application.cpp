
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
#include "ConfigFiles.h"
#include "SceneFunctors.h"
#include "ScenePredicates.h"

#include "CadViewer/Functors/WandMatrix.h"
#include "CadViewer/Functors/WandRotation.h"
#include "CadViewer/Functors/ToolPair.h"

#include "CadViewer/Pick/Select.h"
#include "CadViewer/Pick/Seek.h"

#include "VRV/Interfaces/IParseRestart.h"
#include "VRV/Interfaces/IMenuRead.h"
#include "VRV/Interfaces/IMenuGet.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Components/Manager.h"
#include "Usul/Print/Vector.h"
#include "Usul/Print/Matrix.h"
#include "Usul/Math/Constants.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/System/Host.h"
#include "Usul/Functors/If.h"
#include "Usul/Predicates/UnaryPair.h"

#include "vrj/Kernel/Kernel.h"
#include "vrj/Display/Projection.h"

#include "VRV/Devices/ButtonGroup.h"
#include "VRV/Devices/TrackerDevice.h"

#include "osg/MatrixTransform"
#include "osg/AutoTransform"
#include "osg/Matrix"
#include "osg/LightSource"
#include "osg/Projection"
#include "osg/Version"
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
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application ( Args &args ) :
  BaseClass       ( vrj::Kernel::instance() ),
  _parser         ( new Parser ( args.begin(), args.end() ) ),
  _root           ( new osg::Group ),
  _navBranch      ( new osg::MatrixTransform ),
  _gridBranch     ( new osg::MatrixTransform ),
  _cursor         ( new osg::MatrixTransform ),
  _menuBranch     ( new osg::MatrixTransform ),
  _statusBranch   ( new osg::MatrixTransform ),
  _origin         ( new osg::Group ),
  _auxiliary      ( new osg::Group ),
  _textBranch     ( new osg::Projection ),
  _navigatorH     ( 0x0 ),
  _navigatorV     ( 0x0 ),
  _sceneTool      ( 0x0 ),
  _intersector    ( 0x0 ),
  _buttons        ( new VRV::Devices::ButtonGroup ),
  _tracker        ( new VRV::Devices::TrackerDevice ( "VJWand" ) ),
  _joystick       ( new VRV::Devices::JoystickDevice ( "VJAnalog0", "VJAnalog1" ) ),
  _analogTrim     ( 0, 0 ),
  _rotCenter      ( 0, 0, 0 ),
  _pickText       ( new OsgTools::Text ),
  _navText        ( new OsgTools::Text ),
  _frameText      ( new OsgTools::Text ),
  _msgText        ( new OsgTools::Text ),
  _vp             ( 0, 0, 200, 200 ),
  _flags          ( 0 ),
  _wandOffset     ( 0, 0, 0 ), // feet (used to be z=-4)
  _cursorMatrix   ( 0x0 ),
  _iVisibility    ( static_cast < CV::Interfaces::IVisibility* >    ( 0x0 ) ),
  _iSelection     ( static_cast < CV::Interfaces::ISelection* >     ( 0x0 ) ),
  _iMaterialStack ( static_cast < CV::Interfaces::IMaterialStack* > ( 0x0 ) ),
  _menu           ( new MenuKit::OSG::Menu() ),
  _statusBar      ( new MenuKit::OSG::Menu() ),
  _buttonMap      (),
  _prefs          ( new VRV::Prefs::Settings ),
  _home           ( osg::Matrixf::identity() ),
  _colorMap       (),
  _textures       ( true )
{
  ErrorChecker ( 1067097070u, 0 == _appThread );
  ErrorChecker ( 2970484549u, 0 == _mainThread );
  ErrorChecker ( 1074058928u, 0x0 != _parser.get() );
  ErrorChecker ( 1067094626u, _root.valid() );
  ErrorChecker ( 1067222084u, _navBranch.valid() );
  ErrorChecker ( 1067094628u, _gridBranch.valid() );
  ErrorChecker ( 1067094631u, _cursor.valid() );
  ErrorChecker ( 1325879383u, _menuBranch.valid() );
  ErrorChecker ( 3423749732u, _statusBranch.valid() );
  ErrorChecker ( 1068249416u, _origin.valid() );
  ErrorChecker ( 1069021589u, _auxiliary.valid() );
  ErrorChecker ( 1071446158u, _textBranch.valid() );
  ErrorChecker ( 1071551353u, 0x0 != _pickText.get() );
  ErrorChecker ( 1071551354u, 0x0 != _navText.get() );
  ErrorChecker ( 1071551355u, 0x0 != _frameText.get() );
  ErrorChecker ( 1071551356u, 0x0 != _msgText.get() );

  // Initialize the main thread.
  _mainThread = Usul::Threads::currentThreadId();

  // Read the user's preference file, if any.
  this->_readUserPreferences();

  // Hook up the branches.
  _root->addChild      ( _cursor.get()       );
  _root->addChild      ( _menuBranch.get()   );
  _root->addChild      ( _statusBranch.get() );
  _root->addChild      ( _origin.get()       );
  _root->addChild      ( _auxiliary.get()    );
  _root->addChild      ( _textBranch.get()   );
  _root->addChild      ( _navBranch.get()    );
  _navBranch->addChild ( this->models()      );
  _navBranch->addChild ( _gridBranch.get()   );

  // Name the branches.
  _root->setName         ( "_root"         );
  _navBranch->setName    ( "_navBranch"    );
  _gridBranch->setName   ( "_gridBranch"   );
  _cursor->setName       ( "_cursor"       );
  _menuBranch->setName   ( "_menuBranch"   );
  _statusBranch->setName ( "_statusBranch" );
  _origin->setName       ( "_origin"       );
  _auxiliary->setName    ( "_auxiliary"    );
  _textBranch->setName   ( "_textBranch"   );

  // Hook up the joystick callbacks.
  USUL_VALID_REF_POINTER(JoystickCB) jcb ( new JoystickCB ( this ) );
  _joystick->callback ( VRV::Devices::JOYSTICK_ENTERING_RIGHT, jcb.get() );
  _joystick->callback ( VRV::Devices::JOYSTICK_ENTERING_LEFT,  jcb.get() );
  _joystick->callback ( VRV::Devices::JOYSTICK_ENTERING_UP,    jcb.get() );
  _joystick->callback ( VRV::Devices::JOYSTICK_ENTERING_DOWN,  jcb.get() );

  // Have to load the config files now. Remove them from the arguments.
  Parser::Args configs = _parser->files ( ".jconf", true );
  this->_loadConfigFiles ( configs );

  // populate the color map.  TODO: Read from config file.
  _colorMap["red"]    = osg::Vec4 ( 1.0,0.0,0.0,1.0 );
  _colorMap["yellow"] = osg::Vec4 ( 1.0,1.0,0.0,1.0 );
  _colorMap["green"]  = osg::Vec4 ( 0.0,1.0,0.0,1.0 );
  _colorMap["teal"]   = osg::Vec4 ( 0.0,1.0,1.0,1.0 );
  _colorMap["blue"]   = osg::Vec4 ( 0.0,0.0,1.0,1.0 );
  _colorMap["white"]  = osg::Vec4 ( 1.0,1.0,1.0,1.0 );
  _colorMap["grey"]   = osg::Vec4 ( 0.5,0.5,0.5,1.0 );
  _colorMap["black"]  = osg::Vec4 ( 0.0,0.0,0.0,1.0 );

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
  catch ( ... )
  {
    std::cout << "Error 1082603859: "
              << "Unknown exception generated while initializing." 
              << std::endl;
    stop = true;
  }

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
  _buttons->add ( new VRV::Devices::ButtonDevice ( CV::BUTTON0, "VJButton0" ) );
  _buttons->add ( new VRV::Devices::ButtonDevice ( CV::BUTTON1, "VJButton1" ) );
  _buttons->add ( new VRV::Devices::ButtonDevice ( CV::BUTTON2, "VJButton2" ) );
  _buttons->add ( new VRV::Devices::ButtonDevice ( CV::BUTTON3, "VJButton3" ) );
  _buttons->add ( new VRV::Devices::ButtonDevice ( CV::BUTTON4, "VJButton4" ) );
  _buttons->add ( new VRV::Devices::ButtonDevice ( CV::BUTTON5, "VJButton5" ) );

  // Set up lights.
  this->_initLight();

  // Set the scene-viewer's scene.
  this->setSceneData ( _root.get() );

  // Parse the command-line arguments.
  this->_parseCommandLine();

  // Move the model-group so that the models are centered and visible.
  //this->viewAll ( this->models(), _prefs->viewAllScaleZ() );

  // Now that the models are centered, draw a grid in proportion to the size 
  // of the models.
  this->_initGrid ( this->models() );

  // Save the "home" position.
  this->_setHome();

  // Set a default cursor matrix functor.
  this->_setCursorMatrixFunctor ( new CV::Functors::WandMatrix ( this->_thisUnknown() ) );

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

  if( !this->_isHeadNode() )
    return;

  // Create a matrix-transform relative to the global coordinate system.
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  ErrorChecker ( 1071452071, mt.valid() );

  mt->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );
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

  // Get the bounding sphere of the node.
  const osg::BoundingSphere &bs = node->getBound();

  // Handle zero-sized bounding spheres.
  float r = ( bs.radius() <= 1e-6 ) ? 1 : bs.radius();

  // Clean up any old grids.
  _gridFunctors.clear();

  // Set the properties.
  for( int i = 0; i < _prefs->numGrids(); ++i )
  {
    OsgTools::Grid grid;
    grid.numBlocks ( _prefs->numGridBlocks( i ) );
    grid.size ( r * _prefs->gridScale( i ) );
    grid.color ( _prefs->gridColor( i ) );
    grid.fillColor ( _prefs->gridFillColor( i ) );
    Usul::Math::Matrix44f o;
    o.makeRotation ( _prefs->gridRotationAngleRad( static_cast < int > ( i ) ), _prefs->gridRotationVector( i ) );
    grid.orientation ( o );

    // Move the center so that it is below the bounding sphere of the node.
    osg::Vec3 c ( bs.center() );
    if(_prefs->offsetGrid( static_cast < int > ( i ) )) 
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
//  Initialize the light.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initLight()
{
  ErrorChecker ( 1067093691, isAppThread(), CV::NOT_APP_THREAD );

  osg::ref_ptr<osg::Light> light ( new osg::Light );
  osg::Vec3 ld;
  osg::Vec4 lp;

  OsgTools::Convert::vector<Usul::Math::Vec4f,osg::Vec4>( _prefs->lightPosition(), lp, 4 );
  OsgTools::Convert::vector<Usul::Math::Vec3f,osg::Vec3>( _prefs->lightDirection(), ld, 3 );

  light->setPosition( lp );
  light->setDirection( ld );

  this->addLight ( light.get() );
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
	osg::ref_ptr < osg::StateSet > ss ( _menuBranch->getOrCreateStateSet() );

  // The line above should always return a valid state set.  Check just to make sure.
  if( ss.valid() )
  {
	  ss->setRenderBinDetails ( 100, "RenderBin" );
	  ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );

    // take lighting off of the menu
    ss->setMode( GL_LIGHTING , osg::StateAttribute::OFF );

    // set the stateset
    _menuBranch->setStateSet( ss.get() );
  }

  // Fill the callback map.
  CV_REGISTER ( _quitCallback,     "exit" );
  CV_REGISTER ( _quitCallback,     "quit" );
  CV_REGISTER ( _hideSelected,     "hide_selected" );
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
  CV_REGISTER ( _vRotWandPosX,     "vertical_rotate_wand_clockwise_x" );
  CV_REGISTER ( _vRotGlobalPosX,   "vertical_rotate_global_clockwise_x" );
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
  CV_REGISTER ( _increaseSpeed,    "increase_speed" );
  CV_REGISTER ( _decreaseSpeed,    "decrease_speed" );
  //CV_REGISTER ( _saveView,         "save_camera_view" );

  // Get the component.
  VRV::Interfaces::IMenuRead::QueryPtr reader ( Usul::Components::Manager::instance().getInterface ( VRV::Interfaces::IMenuRead::IID ) );

  // Find the path to the config file.
  std::string filename ( CV::Config::filename ( "menu" ) );

  // Read the configuration file.
  if( reader.valid() )
    reader->readMenuFile ( filename, ValidUnknown ( this ) );

  // Set the menu.
  VRV::Interfaces::IMenuGet::QueryPtr menu ( reader );

  if( menu.valid() )
    _menu->menu ( menu->getMenu() );

  // Default settings, so that the menu has the correct toggle's checked.
  OsgTools::State::StateSet::setPolygonsFilled ( this->models(), false );
  OsgTools::State::StateSet::setPolygonsSmooth ( this->models() );
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
  osg::ref_ptr < osg::StateSet > ss ( _statusBranch->getOrCreateStateSet() );
  if( ss.valid() )
  {
    ss->setRenderBinDetails ( 100, "RenderBin" );
    ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );
  }
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

void Application::latePreFrame()
{
  // For safety.
  try
  {
    this->_latePreFrame();
  }

  // Catch exceptions.
  catch ( const std::exception &e )
  {
    this->_update ( *_msgText, "Error 1999395957: Application::preFrame(): exception: " + std::string ( e.what() ) );
  }
  // Catch exceptions.
  catch ( ... )
  {
    this->_update ( *_msgText, "Error 2847458960: Application::preFrame(): unknown exception." );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the frame is drawn.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_latePreFrame()
{
  ErrorChecker ( 1067093580, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 1067222382, _cursor.valid() );
  
  // Call the base class's function.
  BaseClass::latePreFrame();
  
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
  const unsigned long walkID ( 1084438120u );                 // comes from AppCallback.cpp
  bool handled ( false );
  
  if(_intersector) return false;                              // skip this code if we're in an intersection mode

  unsigned long mode ( _buttons->released() );                // Button release event

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
//  Callback for the joystick.
//
///////////////////////////////////////////////////////////////////////////////

void Application::JoystickCB::operator() ( VRV::Devices::Message m, Usul::Base::Referenced * )
{
  ErrorChecker ( 1915253659u, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 4165917933u, 0x0 != _app );

  MenuKit::OSG::Menu *menu = _app->_menu.get();
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

  // Initialize the text if we need to. We cannot call this sooner because 
  // contextInit() has to be called first.
  if ( false == Usul::Bits::has ( _flags, Detail::_TEXT_IS_INITIALIZED ) )
  {
    osg::ref_ptr < osg::FrameStamp > framestamp ( this->getFrameStamp() );
    if ( 0x0 != framestamp.get() && this->getFrameStamp()->getFrameNumber() > 10 )
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

void Application::_setCursor ( unsigned int state )
{
  ErrorChecker ( 1072722817u, isAppThread(), CV::NOT_APP_THREAD );

  OsgTools::Axes axes;
  axes.length ( 0.5f );
  axes.lineWidth ( 0.05f * axes.length() );
  axes.state ( state );

  OsgTools::Group::removeAllChildren ( _cursor.get() );
  _cursor->addChild ( axes() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the cursor's matrix functor.
//
///////////////////////////////////////////////////////////////////////////////
//
void Application::_setCursorMatrixFunctor ( VRV::Functors::Matrix::MatrixFunctor *f )
{
  _cursorMatrix = f;
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
  Reader::QueryPtr reader ( Usul::Components::Manager::instance().getInterface ( Reader::IID ) );

  if( reader.valid() )
  {
    // User feedback.
    this->_update ( *_msgText, "Reading file: " + filename );

    // Read the file.
    reader->parseRestartFile ( filename, ValidUnknown ( this ) );

    // User feedback.
    this->_update ( *_msgText, "Done reading: " + filename );
  }
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
    // Is this function ever called?
    USUL_ASSERT ( false );
    // Read the model.
    //this->_readModel ( filename, matrix );
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
  return dynamic_cast < const osg::Group * > ( this->models() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the models scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Application::modelsScene()
{
  return dynamic_cast < osg::Group * > ( this->models() );
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

float Application::frameTime() const
{
  return this->_getFrameTime();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the cursor.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_updateCursor()
{
  ErrorChecker ( 1074207542u, isAppThread(), CV::NOT_APP_THREAD );

  // If we have a matrix functor...
  if ( _cursorMatrix.valid() )
  {
    // Update the internal matrix.
    (*_cursorMatrix)();

    // Set the cursor's matrix.
    osg::Matrixf M;
    OsgTools::Convert::matrix ( _cursorMatrix->matrix(), M );
    _cursor->setMatrix ( M );
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
  default:
    return BaseClass::queryInterface ( iid );
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
//  Is this machine the head node?
//
///////////////////////////////////////////////////////////////////////////////

bool Application::_isHeadNode() const
{
#ifdef _MSC_VER
  return true;
#else
  return Usul::System::Host::name() == _prefs->headNodeMachineName();
#endif
}
