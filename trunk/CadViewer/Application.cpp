
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

#include "VRV/Functors/Wand/WandMatrix.h"
#include "VRV/Functors/Wand/WandRotation.h"
#include "CadViewer/Functors/ToolPair.h"

#include "CadViewer/Pick/Select.h"
#include "CadViewer/Pick/Seek.h"

#include "Usul/App/Application.h"
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
#include "Usul/File/Path.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/ICommandList.h"
#include "Usul/Interfaces/IButtonID.h"

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

#include "boost/filesystem/operations.hpp"
#include "Usul/File/Boost.h"

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
  BaseClass       ( ),
  _parser         ( new Parser ( args.begin(), args.end() ) ),
  _gridBranch     ( new osg::MatrixTransform ),
  _statusBranch   ( new osg::MatrixTransform ),
  _origin         ( new osg::Group ),
  _auxiliary      ( new osg::Group ),
  _navigatorH     ( 0x0 ),
  _navigatorV     ( 0x0 ),
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
  _menu           ( new MenuKit::OSG::Menu() ),
  _statusBar      ( new MenuKit::OSG::Menu() ),
  _home           ( osg::Matrixf::identity() ),
  _colorMap       (),
  _textures       ( true )
{
  ErrorChecker ( 1067097070u, 0 == _appThread );
  ErrorChecker ( 2970484549u, 0 == _mainThread );
  ErrorChecker ( 1074058928u, 0x0 != _parser.get() );
  ErrorChecker ( 1067094628u, _gridBranch.valid() );
  ErrorChecker ( 3423749732u, _statusBranch.valid() );
  ErrorChecker ( 1068249416u, _origin.valid() );
  ErrorChecker ( 1069021589u, _auxiliary.valid() );
  ErrorChecker ( 1071551353u, 0x0 != _pickText.get() );
  ErrorChecker ( 1071551354u, 0x0 != _navText.get() );
  ErrorChecker ( 1071551355u, 0x0 != _frameText.get() );
  ErrorChecker ( 1071551356u, 0x0 != _msgText.get() );

  // Initialize the main thread.
  _mainThread = Usul::Threads::currentThreadId();

  // Read the user's preference file, if any.
  this->_readUserPreferences();

  // Hook up the branches.
  this->_sceneRoot()->addChild      ( _statusBranch.get() );
  this->_sceneRoot()->addChild      ( _origin.get()       );
  this->_sceneRoot()->addChild      ( _auxiliary.get()    );
  //_navBranch->addChild ( _gridBranch.get()   );

  // Name the branches.
  _gridBranch->setName   ( "_gridBranch"   );
  _statusBranch->setName ( "_statusBranch" );
  _origin->setName       ( "_origin"       );
  _auxiliary->setName    ( "_auxiliary"    );

  // Hook up the joystick callbacks.
  JoystickCB::RefPtr jcb ( new JoystickCB ( this ) );
  this->joystick()->callback ( VRV::Devices::JOYSTICK_ENTERING_RIGHT, jcb.get() );
  this->joystick()->callback ( VRV::Devices::JOYSTICK_ENTERING_LEFT,  jcb.get() );
  this->joystick()->callback ( VRV::Devices::JOYSTICK_ENTERING_UP,    jcb.get() );
  this->joystick()->callback ( VRV::Devices::JOYSTICK_ENTERING_DOWN,  jcb.get() );

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

  // Initialize the application thread.
  _appThread = Usul::Threads::currentThreadId();

  // Call the base class's function first.
  BaseClass::init();
  
  // Set the global GL_NORMALIZE flag.
  this->normalize ( this->preferences()->normalizeVertexNormalsGlobal() );
  
  // Set the background color.
  const Preferences::Vec4f &bc = this->preferences()->backgroundColor();
  this->setBackgroundColor ( osg::Vec4 ( bc[0], bc[1], bc[2], bc[3] ) );

  // Set up lights.
  this->_initLight();

  // Parse the command-line arguments.
  this->_parseCommandLine();

  // Save the "home" position.
  this->_setHome();

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
#if 0
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
//  Initialize the light.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initLight()
{
  ErrorChecker ( 1067093691, isAppThread(), CV::NOT_APP_THREAD );

  osg::ref_ptr<osg::Light> light ( new osg::Light );
  osg::Vec3 ld;
  osg::Vec4 lp;
  osg::Vec4 ambient;

  OsgTools::Convert::vector<Usul::Math::Vec4f,osg::Vec4>( this->preferences()->lightPosition(), lp, 4 );
  OsgTools::Convert::vector<Usul::Math::Vec3f,osg::Vec3>( this->preferences()->lightDirection(), ld, 3 );
  OsgTools::Convert::vector<Usul::Math::Vec4f,osg::Vec4>( this->preferences()->ambientLightColor(), ambient, 4 );

  light->setPosition( lp );
  light->setDirection( ld );
  light->setAmbient ( ambient );

  this->addLight ( light.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the menu.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initMenu()
{
  
  typedef VRV::Prefs::Settings::Color Color;

  osg::Vec4 bgNormal,   bgHighlight,   bgDisabled;
  osg::Vec4 textNormal, textHighlight, textDisabled;
  
  // Set menu's background and text colors from preferences.xml stored in VRV::Prefs::Settings
  OsgTools::Convert::vector< Color, osg::Vec4 >( this->preferences()->menuBgColorNorm(),   bgNormal,      4 );
  OsgTools::Convert::vector< Color, osg::Vec4 >( this->preferences()->menuBgColorHLght(),  bgHighlight,   4 );
  OsgTools::Convert::vector< Color, osg::Vec4 >( this->preferences()->menuBgColorDsabl(),  bgDisabled,    4 );
  OsgTools::Convert::vector< Color, osg::Vec4 >( this->preferences()->menuTxtColorNorm(),  textNormal,    4 );
  OsgTools::Convert::vector< Color, osg::Vec4 >( this->preferences()->menuTxtColorHLght(), textHighlight, 4 );
  OsgTools::Convert::vector< Color, osg::Vec4 >( this->preferences()->menuTxtColorDsabl(), textDisabled,  4 );

  MenuPtr osgMenu ( new MenuKit::OSG::Menu );

  osgMenu->skin()->bg_color_normal      ( bgNormal );
  osgMenu->skin()->bg_color_highlight   ( bgHighlight );
  osgMenu->skin()->bg_color_disabled    ( bgDisabled );
  osgMenu->skin()->text_color_normal    ( textNormal );
  osgMenu->skin()->text_color_highlight ( textHighlight );
  osgMenu->skin()->text_color_disabled  ( textDisabled );

  // Need to guard changing items in the scene because we may be rendering.

  // Set the menu scene.
  osg::Matrixf mm;
  OsgTools::Convert::matrix ( this->preferences()->menuMatrix(), mm );
  osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );
  mt->setMatrix ( mm );
  mt->setName ( "MenuBranch" );

  if ( true == this->_isHeadNode() )
  {
    osgMenu->scene ( mt.get() );
  }

  // Get the state set.
  osg::ref_ptr < osg::StateSet > ss ( mt->getOrCreateStateSet() );

  // take lighting off of the menu
  ss->setMode( GL_LIGHTING , osg::StateAttribute::OFF );

  this->projectionGroupRemove ( "VRV_MENU" );
  osg::ref_ptr < osg::Group > group ( this->projectionGroupGet ( "VRV_MENU" ) );
  group->addChild ( mt.get( ) );

  // Make the menu.
  MenuKit::Menu::Ptr menu ( new MenuKit::Menu );
  menu->layout ( MenuKit::Menu::HORIZONTAL );

  // The file menu.
  {
    MenuKit::Menu::Ptr file ( new MenuKit::Menu );
    file->layout ( MenuKit::Menu::VERTICAL );
    file->text ( "File" );
    this->_initFileMenu ( file.get() );
    menu->append ( file.get() );
  }

  // The edit menu.
  {
    MenuKit::Menu::Ptr edit ( new MenuKit::Menu );
    edit->layout ( MenuKit::Menu::VERTICAL );
    edit->text ( "Edit" );
    this->_initEditMenu ( edit.get() );
    menu->append ( edit.get() );
  }

  // The view menu.
  {
    MenuKit::Menu::Ptr view ( new MenuKit::Menu );
    view->layout ( MenuKit::Menu::VERTICAL );
    view->text ( "View" );
    this->_initViewMenu ( view.get() );
    menu->append ( view.get() );
  }

  // The navigate menu.
  {
    MenuKit::Menu::Ptr navigate ( new MenuKit::Menu );
    navigate->layout ( MenuKit::Menu::VERTICAL );
    navigate->text ( "Navigate" );
    this->_initNavigateMenu ( navigate.get() );
    menu->append ( navigate.get() );
  }

  // The options menu.
  {
    MenuKit::Menu::Ptr options ( new MenuKit::Menu );
    options->layout ( MenuKit::Menu::VERTICAL );
    options->text ( "Options" );
    this->_initOptionsMenu ( options.get() );
    menu->append ( options.get() );
  }

  // The tools menu.
  {
    MenuKit::Menu::Ptr tools ( new MenuKit::Menu );
    tools->layout ( MenuKit::Menu::VERTICAL );
    tools->text ( "Tools" );
    this->_initToolsMenu ( tools.get() );
    menu->append ( tools.get() );
  }

  // Set the menu.
  osgMenu->menu ( menu.get() );

  // Swap the menu.
  {
    Guard guard ( this->mutex() );
    _menu = osgMenu.get();
  }

  // Default settings, so that the menu has the correct toggle's checked.
  OsgTools::State::StateSet::setPolygonsFilled ( this->models(), false );
  OsgTools::State::StateSet::setPolygonsSmooth ( this->models() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the file menu.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initFileMenu     ( MenuKit::Menu* menu )
{
  // The export sub-menu.
  {
    MenuKit::Menu::Ptr exportMenu ( new MenuKit::Menu );
    exportMenu->layout ( MenuKit::Menu::VERTICAL );
    exportMenu->text ( "Export" );
    menu->append ( exportMenu.get() );

    exportMenu->append ( this->_createButton ( "Image", MenuKit::memFunCB2 ( this, &Application::_exportImage ) ) );
    exportMenu->append ( this->_createButton ( "Models ASCII", MenuKit::memFunCB2 ( this, &Application::_exportWorld ) ) );
    exportMenu->append ( this->_createButton ( "Models Binary", MenuKit::memFunCB2 ( this, &Application::_exportWorldBinary ) ) );
    exportMenu->append ( this->_createButton ( "Scene ASCII", MenuKit::memFunCB2 ( this, &Application::_exportScene ) ) );
    exportMenu->append ( this->_createButton ( "Scene Binary", MenuKit::memFunCB2 ( this, &Application::_exportSceneBinary ) ) );
  }

  menu->append ( this->_createSeperator () );
  menu->append ( this->_createButton ( "Exit", MenuKit::memFunCB2 ( this, &Application::_quitCallback ) ) );
  
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the edit menu.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initEditMenu     ( MenuKit::Menu* menu )
{
  /*CV_REGISTER ( _hideSelected,     "hide_selected" );
  CV_REGISTER ( _showAll,          "show_all" );
  CV_REGISTER ( _unselectVisible,  "unselect_visible" );

  
  CV_REGISTER ( _vScaleWorld,      "vertical_scale_world" );
  CV_REGISTER ( _vScaleSelected,   "vertical_scale_selected" );
  CV_REGISTER ( _wMoveSelLocal,    "move_selected_local_wand" );
  CV_REGISTER ( _wMoveTopLocal,    "move_world_local_wand" );
  CV_REGISTER ( _raySelector,      "ray_selection" );*/
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the view menu.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initViewMenu ( MenuKit::Menu* menu )
{
  menu->append ( this->_createToggle ( "Frame Dump", MenuKit::memFunCB2 ( this, &Application::_dumpFrames ) ) );
  menu->append ( this->_createButton ( "Reset Clipping", MenuKit::memFunCB2 ( this, &Application::_resetClipping ) ) );
  menu->append ( this->_createSeperator () );

  // Rendering passes menu.
  {
    MenuKit::Menu::Ptr passes ( new MenuKit::Menu );
    passes->layout ( MenuKit::Menu::VERTICAL );
    passes->text ( "Rendering Passes" );
    menu->append ( passes.get() );

    passes->append ( this->_createButton ( "1", MenuKit::memFunCB2 ( this, &Application::_renderPassesOne ) ) );
    passes->append ( this->_createButton ( "3", MenuKit::memFunCB2 ( this, &Application::_renderPassesThree ) ) );
    passes->append ( this->_createButton ( "9", MenuKit::memFunCB2 ( this, &Application::_renderPassesNine ) ) );
    passes->append ( this->_createButton ( "12", MenuKit::memFunCB2 ( this, &Application::_renderPassesTweleve ) ) );
  }

  // Goto menu.
  {
    MenuKit::Menu::Ptr gotoMenu ( new MenuKit::Menu );
    gotoMenu->layout ( MenuKit::Menu::VERTICAL );
    gotoMenu->text ( "Goto" );
    menu->append ( gotoMenu.get() );

    gotoMenu->append ( this->_createButton ( "Home", MenuKit::memFunCB2 ( this, &Application::_viewHome ) ) );
    gotoMenu->append ( this->_createButton ( "Front",  MenuKit::memFunCB2 ( this, &Application::_gotoViewFront ) ) );
    gotoMenu->append ( this->_createButton ( "Back",   MenuKit::memFunCB2 ( this, &Application::_gotoViewBack ) ) );
    gotoMenu->append ( this->_createButton ( "Top",    MenuKit::memFunCB2 ( this, &Application::_gotoViewTop ) ) );
    gotoMenu->append ( this->_createButton ( "Bottom", MenuKit::memFunCB2 ( this, &Application::_gotoViewBottom ) ) );
    gotoMenu->append ( this->_createButton ( "Left",   MenuKit::memFunCB2 ( this, &Application::_gotoViewRight ) ) );
    gotoMenu->append ( this->_createButton ( "Right",  MenuKit::memFunCB2 ( this, &Application::_gotoViewLeft ) ) );
  }

  //CV_REGISTER ( _setAsHome,        "set_as_home_view" );

  // Polygons menu.
  {
    MenuKit::Menu::Ptr polygons ( new MenuKit::Menu );
    polygons->layout ( MenuKit::Menu::VERTICAL );
    polygons->text ( "Polygons" );
    menu->append ( polygons.get() );

    polygons->append ( this->_createRadio ( "Smooth",    MenuKit::memFunCB2 ( this, &Application::_polysSmooth ) ) );
    polygons->append ( this->_createRadio ( "Flat",      MenuKit::memFunCB2 ( this, &Application::_polysFlat ) ) );
    polygons->append ( this->_createRadio ( "Wireframe", MenuKit::memFunCB2 ( this, &Application::_polysWireframe ) ) );
    polygons->append ( this->_createRadio ( "Points",    MenuKit::memFunCB2 ( this, &Application::_polysPoints ) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the navigate menu.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initNavigateMenu ( MenuKit::Menu* menu )
{
  // Favorites menu
  {
    MenuKit::Menu::Ptr favorites ( new MenuKit::Menu );
    favorites->layout ( MenuKit::Menu::VERTICAL );
    favorites->text ( "Favorites" );
    menu->append ( favorites.get() );

    favorites->append ( this->_createRadio ( "Fly", MenuKit::memFunCB2 ( this, &Application::_hvTransWandXZ ) ) );
    favorites->append ( this->_createRadio ( "Walk", MenuKit::memFunCB2 ( this, &Application::_hvTransGlobalXZ ) ) );
    favorites->append ( this->_createRadio ( "Pole", MenuKit::memFunCB2 ( this, &Application::_poleNav ) ) );
  }

  // Horizontal joystick menu
  {
    MenuKit::Menu::Ptr horizontal ( new MenuKit::Menu );
    horizontal->layout ( MenuKit::Menu::VERTICAL );
    horizontal->text ( "Horizontal Joystick" );
    menu->append ( horizontal.get() );

    horizontal->append ( this->_createRadio ( "Translate Wand +X", MenuKit::memFunCB2 ( this, &Application::_hTransWandPosX ) ) );
    horizontal->append ( this->_createRadio ( "Translate Wand -X", MenuKit::memFunCB2 ( this, &Application::_hTransWandNegX ) ) );

    horizontal->append ( this->_createSeperator () );

    horizontal->append ( this->_createRadio ( "Translate Global +X", MenuKit::memFunCB2 ( this, &Application::_hTransGlobalPosX ) ) );
    horizontal->append ( this->_createRadio ( "Translate Global -X", MenuKit::memFunCB2 ( this, &Application::_hTransGlobalNegX ) ) );

    horizontal->append ( this->_createSeperator () );

    horizontal->append ( this->_createRadio ( "Rotate Wand Y", MenuKit::memFunCB2 ( this, &Application::_hRotWandPosY ) ) );
    horizontal->append ( this->_createRadio ( "Rotate Global Y", MenuKit::memFunCB2 ( this, &Application::_hRotGlobalPosY ) ) );
  }

  // Vertical joystick menu
  {
    MenuKit::Menu::Ptr vertical ( new MenuKit::Menu );
    vertical->layout ( MenuKit::Menu::VERTICAL );
    vertical->text ( "Vertical Joystick" );
    menu->append ( vertical.get() );

    vertical->append ( this->_createRadio ( "Translate Wand +Z", MenuKit::memFunCB2 ( this, &Application::_vTransWandPosZ ) ) );
    vertical->append ( this->_createRadio ( "Translate Wand -Z", MenuKit::memFunCB2 ( this, &Application::_vTransWandNegZ ) ) );

    vertical->append ( this->_createSeperator () );

    vertical->append ( this->_createRadio ( "Translate Global +Z", MenuKit::memFunCB2 ( this, &Application::_vTransGlobalPosZ ) ) );
    vertical->append ( this->_createRadio ( "Translate Global -Z", MenuKit::memFunCB2 ( this, &Application::_vTransGlobalNegZ ) ) );

    vertical->append ( this->_createSeperator () );

    vertical->append ( this->_createRadio ( "Translate Wand Y", MenuKit::memFunCB2 ( this, &Application::_vTransWandPosY ) ) );
    vertical->append ( this->_createRadio ( "Translate Global Y", MenuKit::memFunCB2 ( this, &Application::_vTransGlobalPosY ) ) );

    vertical->append ( this->_createSeperator () );

    vertical->append ( this->_createRadio ( "Rotate Wand X", MenuKit::memFunCB2 ( this, &Application::_vRotWandPosX ) ) );
    vertical->append ( this->_createRadio ( "Rotate Global X", MenuKit::memFunCB2 ( this, &Application::_vRotGlobalPosX ) ) );
  }

  menu->append ( this->_createSeperator () );
  menu->append ( this->_createButton ( "Translate Speed x 10", MenuKit::memFunCB2 ( this, &Application::_increaseSpeedTen ) ) );
  menu->append ( this->_createButton ( "Translate Speed x 2", MenuKit::memFunCB2 ( this, &Application::_increaseSpeed ) ) );
  menu->append ( this->_createButton ( "Translate Speed / 2", MenuKit::memFunCB2 ( this, &Application::_decreaseSpeed ) ) );
  menu->append ( this->_createButton ( "Translate Speed / 10", MenuKit::memFunCB2 ( this, &Application::_decreaseSpeedTen ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the Tools menu.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initToolsMenu ( MenuKit::Menu* menu )
{
  typedef Usul::Interfaces::ICommandList::CommandList Commands;
  Usul::Interfaces::ICommandList::QueryPtr commandList ( Usul::Documents::Manager::instance().active() );

  if ( commandList.valid() )
  {
    Commands commands ( commandList->getCommandList() );
    for ( Commands::iterator iter = commands.begin(); iter != commands.end(); ++iter )
    {
      menu->append ( this->_createButton ( (*iter).get() ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the options menu.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initOptionsMenu  ( MenuKit::Menu* menu )
{
  // Create a sub-menu for background color
  MenuKit::Menu::Ptr background ( new MenuKit::Menu );
  background->layout ( MenuKit::Menu::VERTICAL );
  background->text ( "Background Color" );
  menu->append ( background.get() );

  // Add a button for each of our colors.
  for ( ColorMap::const_iterator iter = _colorMap.begin(); iter != _colorMap.end(); ++iter )
    background->append ( this->_createButton ( iter->first, MenuKit::memFunCB2 ( this, &Application::_backgroundColor ) ) );

  /*CV_REGISTER ( _selectionColor,   "selection_color" );*/

  menu->append ( this->_createButton ( "Calibrate Joystick", MenuKit::memFunCB2 ( this, &Application::_setAnalogTrim ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Callback to execute a command.
//
///////////////////////////////////////////////////////////////////////////////

namespace CV 
{
  namespace Detail
  {
    class CommandCallback : public MenuKit::Callback
    {
    public:
      typedef MenuKit::Callback BaseClass;

      CommandCallback ( Usul::Commands::Command * command ) : BaseClass (), _command ( command )
      {
      }

      virtual void operator () ( MenuKit::Message m, MenuKit::Item *item )
      {
        _command->execute( 0x0 );
      }

    private:
      Usul::Commands::Command::RefPtr _command;
    };
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Create a button.
//
///////////////////////////////////////////////////////////////////////////////

MenuKit::Button* Application::_createButton ( Usul::Commands::Command* command )
{
  MenuKit::Button* button ( new MenuKit::Button );

  // Set the text.
  button->text ( command->text() );

  // Set the callback.
  button->callback ( MenuKit::MESSAGE_SELECTED, new Detail::CommandCallback ( command ) );

  return button;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a button.
//
///////////////////////////////////////////////////////////////////////////////

MenuKit::Button* Application::_createButton ( const std::string& text, MenuKit::Callback* cb )
{
  // Raw pointer.  Need to switch to MenuKit to Usul::Base::Referenced.
  MenuKit::Button* button ( new MenuKit::Button );

  // Set the text.
  button->text ( text );

  // Hook up the default callback.
  MenuKit::Callback::Ptr dcb ( MenuKit::memFunCB2 ( this, &Application::_defaultCallback ) );
  button->callback ( MenuKit::MESSAGE_FOCUS_ON, dcb );
  
  // Set the callbacks.
  button->callback ( MenuKit::MESSAGE_SELECTED, cb );
  button->callback ( MenuKit::MESSAGE_UPDATE,   cb );

  return button;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a radio button.
//
///////////////////////////////////////////////////////////////////////////////

MenuKit::Button* Application::_createRadio ( const std::string& name, MenuKit::Callback* cb )
{
  MenuKit::Button* button ( this->_createButton ( name, cb ) );
  button->radio ( true );
  return button;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a toggle button.
//
///////////////////////////////////////////////////////////////////////////////

MenuKit::Button* Application::_createToggle    ( const std::string& name, MenuKit::Callback* cb )
{
  MenuKit::Button* button ( this->_createButton ( name, cb ) );
  button->toggle ( true );
  return button;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a seperator.
//
///////////////////////////////////////////////////////////////////////////////

MenuKit::Button* Application::_createSeperator ( )
{
  MenuKit::Button* button ( new MenuKit::Button );
  button->separator ( true );
  return button;
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
  OsgTools::Convert::matrix ( this->preferences()->statusBarMatrix(), sbm );
  _statusBranch->setMatrix ( sbm );
  _statusBar->scene ( _statusBranch.get() );

  // Set the status-bar's properties.
  _statusBar->menu()->append ( new MenuKit::Button );
  _statusBar->menu()->expanded ( this->preferences()->statusBarVisibleAtStartup() );
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

  // The filenames to load.
  Filenames filenames;

  // Find all directories.
  Parser::Args directories;

  // Extract the files with the given extension.
  Usul::Algorithms::extract ( Usul::File::IsDirectory(),
                              _parser->args(),
                              directories,
                              true );

  for ( Parser::Args::iterator iter = directories.begin(); iter != directories.end(); ++ iter )
  {
    // Find the files that we can load.
    Usul::File::findFiles ( *iter, "osg", filenames );
    Usul::File::findFiles ( *iter, "ive", filenames );
  }

  // Extract the model files and remove them from the remaining arguments.
  Parser::Args models = _parser->files ( true );

  // Reserve enough room.
  filenames.reserve ( filenames.size() + models.size() );

  // Copy the model files into our list to load.
  std::copy ( models.begin(), models.end(), std::back_inserter( filenames ) );

  // Load the model files.
  this->_loadModelFiles ( filenames );
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
  
  // Call the base class's function.
  BaseClass::latePreFrame();

  // Update the frame-rate's text.
  this->_updateFrameRateDisplay();

  // Update the analog-input text.
  this->_updateAnalogText();

  // Use the scene-tool if we are supposed to.
  this->_useSceneTool();

  // Navigate if we are supposed to.
  this->_navigate();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the button states and apply to the menu.
//  Returns true if the event was handled.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::_handleMenuEvent( unsigned long id )
{
  ErrorChecker ( 1071559313u, isAppThread(), CV::NOT_APP_THREAD );

  // First see if you are supposed to show or hide it. Always do this first.
  if ( COMMAND_MENU_TOGGLE == id )
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
  switch ( id )
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

bool Application::_handleIntersectionEvent( unsigned long id )
{
  ErrorChecker ( 2588614392u, isAppThread(), CV::NOT_APP_THREAD );

  if(!_intersector) return false;

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
  MenuKit::Message nav_message = MenuKit::MESSAGE_SELECTED;   // simulate message/item from MenuKit
  MenuKit::Item *nav_item (0x0);                              // NULL b/c it's never needed
  const unsigned long walkID ( 1084438120u );                 // comes from AppCallback.cpp
  bool handled ( false );
  
  if(_intersector) return false;                              // skip this code if we're in an intersection mode

  unsigned long mode ( id );                                  // Button release event

  if ( id )                                                   // if mode NOT specified with function call ...
    mode = id;                                                // ... get information from buttons

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
  const std::string &writer ( this->preferences()->fileWriterMachineName() );

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
  _home = this->_navigationMatrix();
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
  return Usul::System::Host::name() == this->preferences()->headNodeMachineName();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export the next frame.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_exportImage ( MenuKit::Message m, MenuKit::Item *item )
{
  switch ( m )
  {
    case MenuKit::MESSAGE_SELECTED:
      this->exportNextFrame ();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Only notify if the menu is not shown..
//
///////////////////////////////////////////////////////////////////////////////

void Application::_updateNotify()
{
  if ( true == _menu.valid() && false == _menu->isVisible() )
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
      case CV::BUTTON0: std::cout << CV::BUTTON0 << " Button 0 pressed (YELLOW)"   << std::endl; break;
      case CV::BUTTON1: std::cout << CV::BUTTON1 << " Button 1 pressed (RED)"      << std::endl; break;
      case CV::BUTTON2: std::cout << CV::BUTTON2 << " Button 2 pressed (GREEN)"    << std::endl; break;
      case CV::BUTTON3: std::cout << CV::BUTTON3 << " Button 3 pressed (BLUE)"     << std::endl; break;
      case CV::BUTTON4: std::cout << CV::BUTTON4 << " Button 4 pressed (JOYSTICK)" << std::endl; break;
      case CV::BUTTON5: std::cout << CV::BUTTON5 << " Button 5 pressed (TRIGGER)"  << std::endl; break;
    }

    // Let the menu process first.
    bool menuHandled ( this->_handleMenuEvent( id ) );
    
    // Always set the mask.
    unsigned int mask ( _menu->isVisible() ? 0 : 0xffffffff );
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
