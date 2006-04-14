
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Main window.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Fox/Core/Precompiled.h"
#include "Helios/Fox/Core/MainWindow.h"
#include "Helios/Fox/Core/Preferences.h"

#include "FoxTools/Errors/ErrorChecker.h"
#include "FoxTools/App/Application.h"
#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Functions/Enable.h"
#include "FoxTools/Functions/Check.h"
#include "FoxTools/Functions/App.h"
#include "FoxTools/Functions/GLVisual.h"
#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Dialogs/FileSelection.h"
#include "FoxTools/Dialogs/Message.h"
#include "FoxTools/Adaptors/Wait.h"
#include "FoxTools/Menu/Button.h"
#include "FoxTools/Menu/RecentFiles.h"
#include "FoxTools/Menu/Separator.h"
#include "FoxTools/Menu/CheckButton.h"
#include "FoxTools/Menu/RadioButton.h"
#include "FoxTools/ToolBar/Factory.h"
#include "FoxTools/ToolBar/Button.h"
#include "FoxTools/ToolBar/Bar.h"
#include "FoxTools/ToolBar/Separator.h"
#include "FoxTools/ToolBar/ToggleButton.h"
#include "FoxTools/Registry/Registry.h"
#include "FoxTools/Headers/ToolTip.h"
#include "FoxTools/Headers/ToolBarShell.h"
#include "FoxTools/Headers/MenuBar.h"
#include "FoxTools/Headers/ToolBarGrip.h"
#include "FoxTools/Headers/StatusLine.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/Separator.h"
#include "FoxTools/Headers/MenuPane.h"
#include "FoxTools/Headers/MenuTitle.h"
#include "FoxTools/Headers/MenuCommand.h"
#include "FoxTools/Headers/MenuSeparator.h"
#include "FoxTools/Headers/MenuCascade.h"
#include "FoxTools/Headers/MenuRadio.h"
#include "FoxTools/Headers/MenuCheck.h"
#include "FoxTools/Headers/MessageBox.h"
#include "FoxTools/Headers/TabBook.h"
#include "FoxTools/Headers/File.h"
#include "FoxTools/Headers/TreeList.h"
#include "FoxTools/Headers/TabItem.h"
#include "FoxTools/Headers/GLVisual.h"
#include "FoxTools/Headers/Text.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/DockSite.h"

#include "Usul/Documents/Document.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Cast/Cast.h"
#include "Usul/System/Screen.h"
#include "Usul/System/Memory.h"
#include "Usul/System/Clock.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Scope/Reset.h"
#include "Usul/CommandLine/Parser.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Components/Object.h"
#include "Usul/State/Busy.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Properties/Attribute.h"
#include "Usul/File/Contents.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Exceptions/Canceled.h"
#include "Usul/IO/Redirect.h"
#include "Usul/File/Temp.h"
#include "Usul/Shared/Preferences.h"
#include "Usul/Registry/Constants.h"
#include "Usul/App/Controller.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/IMenuEntry.h"
#include "Usul/Interfaces/IFoxRadioButton.h"
#include "Usul/Interfaces/IFoxToggleButton.h"
#include "Usul/Interfaces/IFoxSubMenu.h"
#include "Usul/Interfaces/IFoxToolbar.h"
#include "Usul/Interfaces/IFoxAddTab.h"
#include "Usul/Interfaces/INotifyClose.h"
#include "Usul/Interfaces/IFoxDockBar.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/INewDocumentCreate.h"
#include "Usul/Interfaces/IOsgFoxView.h"
#include "Usul/Interfaces/IInitNewDocument.h"
#include "Usul/Interfaces/ICamera.h"
#include "Usul/Interfaces/ICenterOfRotation.h"
#include "Usul/Resources/ProgressBar.h"
#include "Usul/Resources/StatusBar.h"
#include "Usul/Resources/EventQueue.h"
#include "Usul/Resources/ReportErrors.h"
#include "Usul/Resources/CancelButton.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Resources/MenuBar.h"

#include <ctime>
#include <limits>
#include <iterator>

using namespace OsgFox::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs
//
///////////////////////////////////////////////////////////////////////////////

typedef FoxTools::Menu::Group MenuGroup;
typedef FoxTools::Menu::Button MenuButton;
typedef FoxTools::Menu::RecentFiles MenuRecent;
typedef FoxTools::Menu::CheckButton MenuCheck;
typedef FoxTools::Menu::RadioButton MenuRadio;
typedef Usul::Properties::Attribute< Usul::Interfaces::IUnknown* > UAttribute;
typedef Usul::Properties::Attribute< unsigned int > UIntAttribute;
typedef Usul::Properties::Attribute< float > FloatAttribute;
typedef FoxTools::ToolBar::Bar ToolBar;
typedef FoxTools::ToolBar::Button TBButton;
typedef FoxTools::ToolBar::ToggleButton ToggleButton;
typedef FoxTools::App::Application Application;


///////////////////////////////////////////////////////////////////////////////
//
//  Redirect output to a file.
//
///////////////////////////////////////////////////////////////////////////////

namespace OsgFox
{
  namespace Core
  {
    namespace Detail
    {
      Usul::File::Temp output;
      Usul::IO::Redirect redirect ( output.name(), false, false );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( MainWindow ) WindowMap[] = 
{
  //          Message Type     ID                                    Message Handler.
  FXMAPFUNC ( FX::SEL_MOTION,  0,                                    MainWindow::onMotion                   ),
  FXMAPFUNC ( FX::SEL_KEYPRESS,0,                                    MainWindow::onKeyPress                 ),
  FXMAPFUNC ( FX::SEL_CLOSE,   0,                                    MainWindow::onCommandClose             ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_RESTORE,               MainWindow::onCommandRestore           ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_ABOUT_DIALOG,          MainWindow::onCommandAbout             ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_DOCUMENT_NEW,          MainWindow::onCommandDocumentNew       ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_DOCUMENT_NEW,          MainWindow::onUpdateNotBusy            ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_DOCUMENT_OPEN,         MainWindow::onCommandDocumentOpen      ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_DOCUMENT_OPEN,         MainWindow::onUpdateNotBusy            ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_DOCUMENT_INSERT,       MainWindow::onCommandDocumentInsert    ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_DOCUMENT_INSERT,       MainWindow::onUpdateNeedingDocument    ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_DOCUMENT_SAVE,         MainWindow::onCommandDocumentSave      ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_DOCUMENT_SAVE,         MainWindow::onUpdateNeedingDocument    ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_DOCUMENT_SAVE_AS,      MainWindow::onCommandDocumentSaveAs    ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_DOCUMENT_SAVE_AS,      MainWindow::onUpdateNeedingDocument    ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_DOCUMENT_EXPORT,       MainWindow::onCommandExport            ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_DOCUMENT_EXPORT,       MainWindow::onUpdateNeedingDocument    ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_RECENT_FILE,           MainWindow::onCommandRecentFile        ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_EXIT,                  MainWindow::onCommandExit              ),
  FXMAPFUNC ( FX::SEL_COMMAND, Application::ID_DROP_FILE,            MainWindow::onCommandDropFile          ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_STATUS_TEXT,           MainWindow::onUpdateStatusBar          ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_TOOL_COMMAND,          MainWindow::onCommandTool              ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_TOOL_COMMAND,          MainWindow::onUpdateTool               ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_CANCEL_BUTTON,         MainWindow::onCommandCancel            ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_BOUNDING_BOX,          MainWindow::onCommandBoundingBox       ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_BOUNDING_BOX,          MainWindow::onUpdateBoundingBox        ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_BOUNDING_SPHERE,       MainWindow::onCommandBoundingSphere    ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_BOUNDING_SPHERE,       MainWindow::onUpdateBoundingSphere     ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_TOOLBAR_TOGGLE,        MainWindow::onCommandToolbarToggle     ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_TOOLBAR_TOGGLE,        MainWindow::onUpdateToolbarToggle      ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_FULL_SCREEN,           MainWindow::onCommandFullScreen        ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_FULL_SCREEN,           MainWindow::onUpdateFullScreen         ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_DOCUMENT_SAVE,         MainWindow::onUpdateNeedingModified    ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_EDIT_FLIP_NORMALS,     MainWindow::onUpdateFlipNormals        ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_EDIT_FLIP_NORMALS,     MainWindow::onCommandFlipNormals       ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_TEXT_WINDOW,           MainWindow::onUpdateTextWindow         ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_NEW_WINDOW,            MainWindow::onCommandNewWindow         ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_ALLOW_SPINNING,        MainWindow::onUpdateAllowSpinning      ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_ALLOW_SPINNING,        MainWindow::onCommandAllowSpinning     ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_DISPLAY_LISTS,         MainWindow::onCommandDisplayLists      ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_DISPLAY_LISTS,         MainWindow::onUpdateDisplayLists       ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_TWO_SIDED_LIGHTING,    MainWindow::onCommandTwoSidedLighting  ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_TWO_SIDED_LIGHTING,    MainWindow::onUpdateTwoSidedLighting   ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_LIGHTING,              MainWindow::onCommandLighting          ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_LIGHTING,              MainWindow::onUpdateLighting           ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_MODE_NAVIGATION,       MainWindow::onCommandModeNavigation    ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_MODE_NAVIGATION,       MainWindow::onUpdateNavigation         ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_MODE_PICK,             MainWindow::onCommandModePick          ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_MODE_PICK,             MainWindow::onUpdatePick               ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_PREFERENCES,           MainWindow::onCommandPreferences       ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_EDIT_BACKGROUND,       MainWindow::onUpdateNeedingIBackground ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_EDIT_BACKGROUND,       MainWindow::onCommandEditBackground    ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_DEFAULT_BACKGROUND,    MainWindow::onUpdateNeedingIBackground ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_DEFAULT_BACKGROUND,    MainWindow::onCommandDefaultBackground ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_CAMERA_FIT,            MainWindow::onCommandCameraFit         ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_CAMERA_FIT,            MainWindow::onUpdateNeedingICamera     ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_CAMERA_RESET,          MainWindow::onCommandCameraReset       ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_CAMERA_RESET,          MainWindow::onUpdateNeedingICamera     ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_CAMERA_FRONT,          MainWindow::onCommandCameraFront       ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_CAMERA_FRONT,          MainWindow::onUpdateNeedingICamera     ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_CAMERA_BACK,           MainWindow::onCommandCameraBack        ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_CAMERA_BACK,           MainWindow::onUpdateNeedingICamera     ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_CAMERA_LEFT,           MainWindow::onCommandCameraLeft        ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_CAMERA_LEFT,           MainWindow::onUpdateNeedingICamera     ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_CAMERA_RIGHT,          MainWindow::onCommandCameraRight       ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_CAMERA_RIGHT,          MainWindow::onUpdateNeedingICamera     ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_CAMERA_TOP,            MainWindow::onCommandCameraTop         ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_CAMERA_TOP,            MainWindow::onUpdateNeedingICamera     ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_CAMERA_BOTTOM,         MainWindow::onCommandCameraBottom      ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_CAMERA_BOTTOM,         MainWindow::onUpdateNeedingICamera     ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_POLYGONS_FILLED,       MainWindow::onUpdatePolygonsFilled     ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_POLYGONS_FILLED,       MainWindow::onCommandPolygonsFilled    ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_POLYGONS_WIRE,         MainWindow::onUpdatePolygonsWire       ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_POLYGONS_WIRE,         MainWindow::onCommandPolygonsWire      ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_POLYGONS_HIDDEN,       MainWindow::onUpdatePolygonsHidden     ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_POLYGONS_HIDDEN,       MainWindow::onCommandPolygonsHidden    ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_POLYGONS_POINTS,       MainWindow::onUpdatePolygonsPoints     ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_POLYGONS_POINTS,       MainWindow::onCommandPolygonsPoints    ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_POLYGONS_AS_IS,        MainWindow::onUpdatePolygonsAsIs       ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_POLYGONS_AS_IS,        MainWindow::onCommandPolygonsAsIs      ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_SHADING_SMOOTH,        MainWindow::onUpdateShadingSmooth      ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_SHADING_SMOOTH,        MainWindow::onCommandShadingSmooth     ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_SHADING_FLAT,          MainWindow::onUpdateShadingFlat        ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_SHADING_FLAT,          MainWindow::onCommandShadingFlat       ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_SHADING_AS_IS,         MainWindow::onUpdateShadingAsIs        ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_SHADING_AS_IS,         MainWindow::onCommandShadingAsIs       ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_LOW_LODS,              MainWindow::onUpdateLowLods            ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_LOW_LODS,              MainWindow::onCommandLowLods           ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_HIGH_LODS,             MainWindow::onUpdateHighLods           ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_HIGH_LODS,             MainWindow::onCommandHighLods          ),
  FXMAPFUNC ( FX::SEL_CONFIGURE, 0,                                  MainWindow::onResize                   ),
  FXMAPFUNC ( FX::SEL_CHANGED, MainWindow::ID_HORIZONTAL_SPLITTER,   MainWindow::onHorizSplitterChanged     ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_AXES,                  MainWindow::onUpdateAxes               ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_AXES,                  MainWindow::onCommandAxes              ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_STEREO,                MainWindow::onCommandStereo            ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_STEREO,                MainWindow::onUpdateStereo             ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_EYE_DISTANCE,          MainWindow::onCommandEyeDistance       ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_EYE_DISTANCE,          MainWindow::onUpdateEyeDistance        ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_STEREO_MODE,           MainWindow::onCommandStereoMode        ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_STEREO_MODE,           MainWindow::onUpdateStereoMode         ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_LIGHTS,                MainWindow::onUpdateLights             ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_LIGHTS,                MainWindow::onCommandLights            ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_SCENE_STAGE,           MainWindow::onCommandSceneStage        ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_SCENE_STAGE,           MainWindow::onUpdateSceneStage         ),
  FXMAPFUNC ( FX::SEL_COMMAND, MainWindow::ID_CENTEROFROTATION,      MainWindow::onCommandCenterOfRotation  ),
  FXMAPFUNC ( FX::SEL_UPDATE,  MainWindow::ID_CENTEROFROTATION,      MainWindow::onUpdateCenterOfRotation   ),
};

FOX_TOOLS_IMPLEMENT_ABSTRACT ( MainWindow, MainWindow::BaseClass, WindowMap, ARRAYNUMBER ( WindowMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Initializer list.
//
///////////////////////////////////////////////////////////////////////////////

#define INITIALIZER_LIST \
  _menuBar       ( 0x0 ),\
  _statusBar     ( 0x0 ),\
  _memoryLabel   ( 0x0 ),\
  _progressBar   ( 0x0, 0 ),\
  _cancelButton  ( 0x0 ),\
  _recentFiles   ( 0x0 ),\
  _statusText    ( "Idle" ),\
  _hSplitter     ( 0x0, 100 ),\
  _vSplitter     ( 0x0, 100 ),\
  _tabBook       ( 0x0 ),\
  _tabItems      (),\
  _refCount      ( 0 ),\
  _fullScreen    ( false ),\
  _wasMaximized  ( false ),\
  _output        (),\
  _textWindow    ( 0x0 )


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow() : BaseClass(), 
  INITIALIZER_LIST
{
  // If this static-assertion fails then make ISendMessage::ID_FIRST bigger.
  USUL_STATIC_ASSERT ( 
                      static_cast < unsigned int > ( Usul::Interfaces::ISendMessage::ID_FIRST ) > 
                      static_cast < unsigned int > ( MainWindow::ID_LAST ) );

  // Do not use this constructor.
  FOX_TOOLS_ERROR_CHECKER ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow ( 
  FX::FXApp *app,
  const FX::FXString &name,
  FX::FXIcon *icon,
  FX::FXIcon *mi,
  FX::FXuint options ) : 
  BaseClass ( app, name, icon, mi, options, 
              FoxTools::Registry::read ( Usul::Registry::Sections::MAIN_WINDOW, Usul::Registry::Keys::X,      (FXint) ( 0.1 * Usul::System::Screen::width()  ) ),
              FoxTools::Registry::read ( Usul::Registry::Sections::MAIN_WINDOW, Usul::Registry::Keys::Y,      (FXint) ( 0.1 * Usul::System::Screen::height() ) ),
              FoxTools::Registry::read ( Usul::Registry::Sections::MAIN_WINDOW, Usul::Registry::Keys::WIDTH,  (FXint) ( 0.9 * Usul::System::Screen::width()  ) ),
              FoxTools::Registry::read ( Usul::Registry::Sections::MAIN_WINDOW, Usul::Registry::Keys::HEIGHT, (FXint) ( 0.9 * Usul::System::Screen::height() ) ),
              0, 0, 0, 0, 0, 0 ),
  INITIALIZER_LIST
{
  // Feedback.
  std::cout << app->getAppName().text() << std::endl;

  // Set this pointer for convenient access elsewhere.
  FoxTools::Functions::mainWindow( this );

  // Set this sooner rather than later.
  OsgFox::Core::Preferences::owner ( this );

  // Set the resources
  Usul::Resources::cancelButton ( this );
  Usul::Resources::flushEvents  ( this );
  Usul::Resources::progressBar  ( this );
  Usul::Resources::reportErrors ( this );
  Usul::Resources::statusBar    ( this );
  Usul::Resources::textWindow   ( IUnknown::ValidQueryPtr ( this ) );
  Usul::Resources::menuBar      ( this );

  // Make a tool tip.
  new FXToolTip ( app, 0 );

  // The top site to dock
  FoxTools::ToolBar::Factory::instance().topDock();

  // Menu shell.
  FX::FXToolBarShell * menuShell ( new FX::FXToolBarShell ( this, FX::FRAME_RAISED ) );

  // Menu bar.
  _menuBar = new FoxTools::Menu::Bar ( FoxTools::ToolBar::Factory::instance().topDock()->dockSite(), menuShell );
  
  // Build now so menu bar is above toolbars
  _menuBar->build();

  // Add some plugin extensions.
  #ifdef _DEBUG
  PluginManager::instance().addPluginExtension ( "plugd" );
  #else
  PluginManager::instance().addPluginExtension ( "plug" );
  #endif

  // Flag that allows spinning.
  Usul::Shared::Preferences &prefs ( Usul::Shared::Preferences::instance() );
  const bool spinning ( FoxTools::Registry::read ( Usul::Registry::Sections::VIEWER_SETTINGS, Usul::Registry::Keys::ALLOW_SPIN, false ) );
  prefs.setBool ( Usul::Registry::Keys::ALLOW_SPIN, spinning );

  // Flag that specifies application-wide display list usage.
  const bool displayLists ( FoxTools::Registry::read ( Usul::Registry::Sections::VIEWER_SETTINGS, Usul::Registry::Keys::DISPLAY_LISTS, true ) );
  prefs.setBool ( Usul::Registry::Keys::DISPLAY_LISTS, displayLists );

  // Flag that specifies application-wide low lod usage.
  const bool lowlods ( FoxTools::Registry::read ( Usul::Registry::Sections::VIEWER_SETTINGS, Usul::Registry::Keys::LOW_LODS, true ) );
  prefs.setBool ( Usul::Registry::Keys::LOW_LODS, lowlods );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
  // Unset this pointer.
  FoxTools::Functions::mainWindow ( 0x0 );

  // Delete the menu-bar now so that FOX doesn't try later.
  _recentFiles->clear();
  _menuBar->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the window is created.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::create()
{
  // Call the base class's function.
  BaseClass::create();

  // Enable windows drag-and-drop.
  #ifdef _WIN32
  ::DragAcceptFiles ( reinterpret_cast < HWND > ( this->id() ), TRUE );
  #endif

#if 0

  // Was trying to put a canvas into a toolbar... not much luck.
  this->addToolbar ( "Temp1" );
  FX::FXToolBar* tempToolBar ( this->toolbar ( "Temp1" ) );
  tempToolBar->create();
  const std::string tf0 ( "E:/perry/models/kelvin/ive/FishingReel.ive" );
  Usul::Documents::Document::ValidRefPtr td0 ( this->createDocument ( tf0 ) );
  td0->open ( tf0, IUnknown::ValidQueryPtr ( this ) );
  OsgFox::View* tv0 ( new OsgFox::View ( td0, tempToolBar, FoxTools::Functions::visual() ) );
  tv0->create();
  tv0->resize ( 32, 32 );
  tv0->writeImageFile ( "E:/perry/models/kelvin/ive/FishingReel.jpg" );

#endif

  // Now that the text window is created, set this file name.
  this->outputFile ( Detail::output.name() );

  // Set the horizontal splitter.
  if( _hSplitter.first )
    _hSplitter.first->setSplit ( 1, _hSplitter.second );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the default gui.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::createDefaultGUI()
{
  // Load all plugins.
  Usul::App::Controller::instance().loadPlugins();
 
  // Initialize the status bar.
  this->_initStatusBar();

  // Sites where to dock
  FoxTools::ToolBar::Factory::instance().bottomDock();
  FoxTools::ToolBar::Factory::instance().leftDock();
  FoxTools::ToolBar::Factory::instance().rightDock();

  // Initialize the regular toolbars.
  this->_initToolbars();

  // Find all plugins that add a toolbar button or a dockable toolbar.
  this->_initPluginToolbar();

  // Initialize menus.
  this->_initFileMenu();
  this->_initEditMenu();
  this->_initViewMenu();
  this->_initPluginMenus();
  this->_initToolsMenu();
  this->_initHelpMenu();

  // The horizontal splitter.
  unsigned int layout ( FX::LAYOUT_SIDE_TOP | FX::FRAME_NONE | FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y | FX::SPLITTER_TRACKING | FX::SPLITTER_VERTICAL | FX::SPLITTER_REVERSED );
  _hSplitter.first = new FX::FXSplitter ( this, this, ID_HORIZONTAL_SPLITTER, layout );
  _hSplitter.second = FoxTools::Registry::read ( Usul::Registry::Sections::HORIZONTAL_SPLITTER, Usul::Registry::Keys::SPLIT, _hSplitter.second );

  // Make the vertical splitter.
  layout = FX::LAYOUT_SIDE_TOP | FX::FRAME_NONE | FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y | FX::SPLITTER_TRACKING | FX::SPLITTER_HORIZONTAL;
  _vSplitter.first = new FX::FXSplitter ( _hSplitter.first, layout );

  // Build tab control.
  this->_initTabControl();

  // Make the text-output window.
  layout = FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y | FX::FRAME_THICK | FX::FRAME_SUNKEN;
  FX::FXHorizontalFrame *hf ( new FX::FXHorizontalFrame ( _hSplitter.first, layout, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ) );
  layout = FX::TEXT_WORDWRAP | FX::TEXT_SHOWACTIVE | FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y;
  _textWindow = new FX::FXText ( hf, this, ID_TEXT_WINDOW, layout );

  // Build the toolbars.
  FoxTools::ToolBar::Factory::instance().build();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load command-line files.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::loadCommandLineFiles()
{
  // Typedefs.
  typedef Usul::CommandLine::Parser Parser;
  typedef Usul::CommandLine::Arguments Arguments;

  // Parse the command-line.
  Arguments::Args args ( Arguments::instance().args() );
  Parser parser ( args.begin(), args.end() );

  // Extract existing files. Remove them from the arguments.
  Parser::Args files = parser.files ( true );

  // Return if there are no files to load.
  if( files.empty() )
    return;

  // Open files.
  Filenames names ( files.begin() + 1, files.end() );
  this->_openDocuments ( names );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the file menu.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initFileMenu()
{
  MenuGroup::ValidRefPtr file ( new MenuGroup ( "&File" ) );

  // Get the plugins that create a new document
  PluginSet plugins ( Usul::Components::Manager::instance().getInterfaces ( Usul::Interfaces::INewDocumentCreate::IID ) );

  // If we have some...
  if( !plugins.empty() )
  {
    // Create the new menu
    MenuGroup::ValidRefPtr newMenu ( new MenuGroup ( "&New" ) );
    
    // Loop through our plugins
    for( PluginSet::iterator i = plugins.begin(); i != plugins.end(); ++i )
    {
      // Make the button
      Usul::Interfaces::INewDocumentCreate::ValidQueryPtr create ( i->get() );
      MenuButton::ValidRefPtr button ( new MenuButton ( create->documentTypeName(), "", "Create a new document.", this, MainWindow::ID_DOCUMENT_NEW    ) );

      // Set the user-data.
      UAttribute::ValidRefPtr attribute ( new UAttribute ( i->get() ) );
      button->userData ( attribute );
      
      // Add to new menu
      newMenu->append ( button );
    }

    // Add new to the file menu
    file->append ( newMenu );
  }
    file->append ( new MenuButton ( "&Open...",    "Ctl-O", "Open an existing document.",   this, MainWindow::ID_DOCUMENT_OPEN    ) );
    file->append ( new MenuButton ( "&Insert...",  "Ctl-I", "Insert an existing document.", this, MainWindow::ID_DOCUMENT_INSERT  ) );
    file->append ( new MenuButton ( "&Save",       "",      "Save the current document.",   this, MainWindow::ID_DOCUMENT_SAVE    ) );
    file->append ( new MenuButton ( "Save &As...", "",      "Save document to a new file.", this, MainWindow::ID_DOCUMENT_SAVE_AS ) );
    file->append ( new MenuButton ( "&Export...",  "",      "Export the scene.",            this, MainWindow::ID_DOCUMENT_EXPORT  ) );
    MenuGroup::ValidRefPtr recent ( new MenuGroup ( "Recent Files" ) );
      _recentFiles = new MenuRecent ( "&Clear Recent Files", this, MainWindow::ID_RECENT_FILE );
      recent->append ( _recentFiles );
    file->append ( recent );
    file->append ( new FoxTools::Menu::Separator );
    file->append ( new MenuButton ( "&Exit", "", "Exit the application.", this, MainWindow::ID_EXIT ) );
  _menuBar->append ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the edit menu.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initEditMenu()
{
  MenuGroup::ValidRefPtr edit ( new MenuGroup ( "&Edit" ) );
    edit->append ( new MenuButton ( "&Background...",      "", "Edit the background.",    this, MainWindow::ID_EDIT_BACKGROUND    ) );
    edit->append ( new MenuButton ( "&Default Background", "", "Set default background.", this, MainWindow::ID_DEFAULT_BACKGROUND ) );
    edit->append ( new FoxTools::Menu::Separator );
    edit->append ( new MenuButton ( "&Flip Normals", "", "Flip the direction of all normal vectors.", this, MainWindow::ID_EDIT_FLIP_NORMALS ) );
    edit->append ( new FoxTools::Menu::Separator );
    MenuGroup::ValidRefPtr mode ( new MenuGroup ( "Mode" ) );
      mode->append ( new MenuRadio ( "Navigation", "N", "Enter Navigation Mode", this, MainWindow::ID_MODE_NAVIGATION ) );
      mode->append ( new MenuRadio ( "Pick",       "P", "Enter Pick Mode",       this, MainWindow::ID_MODE_PICK       ) );
    edit->append ( mode );
    edit->append ( new FoxTools::Menu::Separator );
    edit->append ( new MenuButton ( "&Preferences", "", "Change preferences", this, MainWindow::ID_PREFERENCES ) );
 _menuBar->append ( edit );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the view menu.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initViewMenu()
{
  MenuGroup::ValidRefPtr view ( new MenuGroup ( "&View" ) );
    MenuGroup::ValidRefPtr camera ( new MenuGroup ( "Camera" ) );
      camera->append ( new MenuButton ( "&Fit",   "", "Make all geometry visible.", this, MainWindow::ID_CAMERA_FIT   ) );
      camera->append ( new MenuButton ( "&Reset", "", "Reset the camera.",          this, MainWindow::ID_CAMERA_RESET ) );
      camera->append ( new FoxTools::Menu::Separator );
      camera->append ( new MenuButton ( "&Front",  "", "Look at the front.",  this, MainWindow::ID_CAMERA_FRONT  ) );
      camera->append ( new MenuButton ( "&Back",   "", "Look at the back.",   this, MainWindow::ID_CAMERA_BACK   ) );
      camera->append ( new MenuButton ( "&Left",   "", "Look at the left.",   this, MainWindow::ID_CAMERA_LEFT   ) );
      camera->append ( new MenuButton ( "&Right",  "", "Look at the right.",  this, MainWindow::ID_CAMERA_RIGHT  ) );
      camera->append ( new MenuButton ( "&Top",    "", "Look at the top.",    this, MainWindow::ID_CAMERA_TOP    ) );
      camera->append ( new MenuButton ( "&Bottom", "", "Look at the bottom.", this, MainWindow::ID_CAMERA_BOTTOM ) );
    view->append ( camera );
    MenuGroup::ValidRefPtr polygons ( new MenuGroup ( "Polygons" ) );
      polygons->append ( new MenuRadio ( "&Filled",       "", "Draw polygons filled.",            this, MainWindow::ID_POLYGONS_FILLED ) );
      polygons->append ( new MenuRadio ( "&Wire Frame",   "", "Draw polygons as wire-frame.",     this, MainWindow::ID_POLYGONS_WIRE   ) );
      polygons->append ( new MenuRadio ( "&Hidden Lines", "", "Draw polygons with hidden lines.", this, MainWindow::ID_POLYGONS_HIDDEN ) );
      polygons->append ( new MenuRadio ( "&Points",       "", "Draw polygons as points.",         this, MainWindow::ID_POLYGONS_POINTS ) );
      polygons->append ( new MenuRadio ( "As &Is",        "", "Draw polygons as they are.",       this, MainWindow::ID_POLYGONS_AS_IS  ) );
    view->append ( polygons );
    MenuGroup::ValidRefPtr shading ( new MenuGroup ( "Shading" ) );
      shading->append ( new MenuRadio ( "&Smooth", "", "Draw polygons with smooth shading.",   this, MainWindow::ID_SHADING_SMOOTH ) );
      shading->append ( new MenuRadio ( "&Flat",   "", "Draw polygons with flat shading.",     this, MainWindow::ID_SHADING_FLAT   ) );
      shading->append ( new MenuRadio ( "&As &Is", "", "Draw polygons with original shading.", this, MainWindow::ID_SHADING_AS_IS  ) );
    view->append ( shading );
    MenuGroup::ValidRefPtr bounding ( new MenuGroup ( "Bounding" ) );
      bounding->append ( new MenuCheck ( "&Box",    "", "Show/hide the bounding box.",    this, MainWindow::ID_BOUNDING_BOX    ) );
      bounding->append ( new MenuCheck ( "&Sphere", "", "Show/hide the bounding sphere.", this, MainWindow::ID_BOUNDING_SPHERE ) );
    view->append ( bounding );
    MenuGroup::ValidRefPtr toolbar ( new MenuGroup ( "Toolbar" ) );
    typedef FoxTools::ToolBar::Factory Factory;
    for ( Factory::ConstIterator i = Factory::instance().begin(); i != Factory::instance().end(); ++i )
      toolbar->append ( new MenuCheck ( i->first, this, MainWindow::ID_TOOLBAR_TOGGLE ) );
    view->append ( toolbar );
    MenuGroup::ValidRefPtr stereo ( new MenuGroup ( "Stereo" ) );
    stereo->append ( new MenuCheck ( "&Stereo",             "", "Turn on/off stereo.",                       this, MainWindow::ID_STEREO             ) );

    //Add possible stereo modes.
    MenuGroup::ValidRefPtr stereoModes ( new MenuGroup ( "Modes" ) );

    typedef Usul::App::Controller::StereoModes StereoModes;
    StereoModes possibleStereoModes( Usul::App::Controller::instance().stereoModes() );
    
    for ( StereoModes::const_iterator i = possibleStereoModes.begin(); i != possibleStereoModes.end(); ++i )
    {
      MenuRadio::ValidRefPtr button ( new MenuRadio ( i->second, "", "Set the stereo mode.", this, MainWindow::ID_STEREO_MODE ) );
      button->userData ( new UIntAttribute ( i->first ) );
      stereoModes->append( button );
    }
    
    // Add Eye distance values.
    MenuGroup::ValidRefPtr eyeDistance ( new MenuGroup ( "Eye Distance" ) );
    const unsigned int numValues ( 10 );
    for( unsigned int i = 0; i < numValues; ++i )
    {
      float value ( 0.01f + ( (float)i/ ( 100 ) ) );

      char buf[80];
      ::sprintf( buf, "%.2f", value );

      MenuRadio::ValidRefPtr button ( new MenuRadio ( buf, "", "Set the eye distance.", this, MainWindow::ID_EYE_DISTANCE ) );
      button->userData ( new FloatAttribute ( value ) );
      eyeDistance->append ( button );
    }
    stereo->append ( stereoModes );
    stereo->append ( eyeDistance );
    view->append ( stereo );
    view->append ( new MenuCheck ( "&Full Screeen",       "", "Toggle Full Screen Mode.",                  this, MainWindow::ID_FULL_SCREEN        ) );
    view->append ( new MenuCheck ( "&Two Sided Lighting", "", "Turn on/off two sided lighting.",           this, MainWindow::ID_TWO_SIDED_LIGHTING ) );
    view->append ( new MenuCheck ( "Allow &Spin",         "", "Turn on/off spinning geometry.",            this, MainWindow::ID_ALLOW_SPINNING     ) );
    view->append ( new MenuCheck ( "&Use Low Lods",       "", "Turn on/off low levels of detail.",         this, MainWindow::ID_LOW_LODS           ) );
    view->append ( new MenuCheck ( "&Use High Lods",      "", "Turn on/off high levels of detail.",        this, MainWindow::ID_HIGH_LODS          ) );
    view->append ( new MenuCheck ( "&Display Lists",      "", "Turn on/off display list usage.",           this, MainWindow::ID_DISPLAY_LISTS      ) );
    view->append ( new MenuCheck ( "Axes",                "", "Hide/Show Axes.",                           this, MainWindow::ID_AXES               ) );
    view->append ( new MenuCheck ( "Lights",              "", "Hide/Show Lights.",                         this, MainWindow::ID_LIGHTS             ) );
    view->append ( new MenuCheck ( "Stage",               "", "Hide/Show Scene Stage.",                    this, MainWindow::ID_SCENE_STAGE        ) );
    view->append ( new MenuCheck ( "Center of Rotation",  "", "Hide/Show Center of Rotation.",             this, MainWindow::ID_CENTEROFROTATION   ) );
  _menuBar->append ( view );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the toolbars.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initToolbars()
{
  // The standard tool-bar
  this->_initStandardToolbar();

  // The view tool-bar
  this->_initViewToolbar();

  // The mode tool-bar
  this->_initModeToolbar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the standard toolbar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initStandardToolbar()
{
  // Typedefs
  typedef FoxTools::ToolBar::Factory Factory;
  typedef FoxTools::Icons::Factory Icons;

  // The standard tool-bar
  ToolBar::ValidRefPtr standardBar ( Factory::instance().create( "Standard", Factory::TOP ) );

  // Standard tool-bar buttons.
  standardBar->append( new TBButton ( "Open", "Open an existing document.", Icons::ICON_OPEN,     this, MainWindow::ID_DOCUMENT_OPEN ) );
  standardBar->append( new TBButton ( "Save", "Save the active document.",  Icons::ICON_SAVE,     this, MainWindow::ID_DOCUMENT_SAVE ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the view toolbar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initViewToolbar()
{
  // Typedefs
  typedef FoxTools::ToolBar::Factory Factory;
  typedef FoxTools::Icons::Factory Icons;

  // The view tool-bar
  ToolBar::ValidRefPtr viewBar ( Factory::instance().create( "View", Factory::TOP ) );

  // View tool-bar buttons
  viewBar->append ( new TBButton ( "Fit", "Move the camera such that all the geometry is visible.", Icons::ICON_EYE,     this, MainWindow::ID_CAMERA_FIT ) );
  viewBar->append ( new TBButton ( "Reset", "Reset the camera position and orientation.",           Icons::ICON_HOME,    this, MainWindow::ID_CAMERA_RESET ) );
  viewBar->append ( new FoxTools::ToolBar::Separator ( ) );
  viewBar->append ( new TBButton ( "Front", " Look at the geometry's front.",   Icons::ICON_FRONT,  this, MainWindow::ID_CAMERA_FRONT  ) );
  viewBar->append ( new TBButton ( "Back",   "Look at the geometry's back.",    Icons::ICON_BACK,   this, MainWindow::ID_CAMERA_BACK   ) );
  viewBar->append ( new TBButton ( "Left",   "Look at the geometry's left.",    Icons::ICON_LEFT,   this, MainWindow::ID_CAMERA_LEFT   ) );
  viewBar->append ( new TBButton ( "Right",  "Look at the geometry's right.",   Icons::ICON_RIGHT,  this, MainWindow::ID_CAMERA_RIGHT  ) );
  viewBar->append ( new TBButton ( "Top",    "Look at the geometry's top.",     Icons::ICON_TOP,    this, MainWindow::ID_CAMERA_TOP    ) );
  viewBar->append ( new TBButton ( "Bottom", "Look at the geometry's bottom.",  Icons::ICON_BOTTOM, this, MainWindow::ID_CAMERA_BOTTOM ) );
  viewBar->append ( new FoxTools::ToolBar::Separator ( ) );
  viewBar->append ( new ToggleButton ( "Filled",       "Draw polygons filled.",            Icons::ICON_POLYGONS_FILLED, this, MainWindow::ID_POLYGONS_FILLED ) );
  viewBar->append ( new ToggleButton ( "Wire Frame",   "Draw polygons as wire-frame.",     Icons::ICON_POLYGONS_WIRE,   this, MainWindow::ID_POLYGONS_WIRE   ) );
  viewBar->append ( new ToggleButton ( "Hidden Lines", "Draw polygons with hidden lines.", Icons::ICON_POLYGONS_HIDDEN, this, MainWindow::ID_POLYGONS_HIDDEN ) );
  viewBar->append ( new ToggleButton ( "Points",       "Draw polygons as points.",         Icons::ICON_POLYGONS_POINTS, this, MainWindow::ID_POLYGONS_POINTS ) );
  viewBar->append ( new FoxTools::ToolBar::Separator ( ) );
  viewBar->append ( new ToggleButton ( "Lighting",           "Toggle lighting.",           Icons::ICON_LIGHTING_OFF,           Icons::ICON_LIGHTING_ON,           this, MainWindow::ID_LIGHTING           ) );
  viewBar->append ( new ToggleButton ( "Two sided Lighting", "Toggle two sided lighting.", Icons::ICON_LIGHTING_TWO_SIDED_OFF, Icons::ICON_LIGHTING_TWO_SIDED_ON, this, MainWindow::ID_TWO_SIDED_LIGHTING ) );
  viewBar->append ( new FoxTools::ToolBar::Separator ( ) );
  viewBar->append ( new ToggleButton ( "Display Lists", "Toggle Display Lists.",  Icons::ICON_LIGHTNING_BOLT, this, MainWindow::ID_DISPLAY_LISTS ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the mode toolbar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initModeToolbar()
{
  // Typedefs
  typedef FoxTools::ToolBar::Factory Factory;
  typedef FoxTools::Icons::Factory Icons;

  // The mode tool-bar
  ToolBar::ValidRefPtr modeBar ( Factory::instance().create( "Mode", Factory::TOP ) );
  
  // Add buttons for modes.
  modeBar->append (  new ToggleButton ( "Navigation Mode", "Turn on navigation mode", Icons::ICON_HAND,  this, MainWindow::ID_MODE_NAVIGATION ) );
  modeBar->append (  new ToggleButton ( "Pick Mode",       "Turn on pick mode",       Icons::ICON_ARROW, this, MainWindow::ID_MODE_PICK       ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the plugin toolbar buttons.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initPluginToolbar()
{
  // This as an IUnknown
  Usul::Interfaces::IUnknown::ValidQueryPtr me ( this );

  typedef Usul::Interfaces::IFoxToolbar Interface;

  // Get the plugins
  PluginSet plugins ( PluginManager::instance().getInterfaces ( Interface::IID ) );

  //If no plugins, don't create dock toolbar
  if( !plugins.empty() )
  {
    // The tool tool-bar
    ToolBar* toolBar ( FoxTools::ToolBar::Factory::instance().create( "Tool", FoxTools::ToolBar::Factory::TOP ) );

    //Loop through the IFoxToolbars
    for ( PluginItr i = plugins.begin(); i != plugins.end(); ++i )
    {
      //We know it will be valid
      Interface::ValidQueryPtr button ( (*i).get() );
      button->addButtons ( me.get(), toolBar );
    }
  }

  plugins = PluginManager::instance().getInterfaces( Usul::Interfaces::IFoxDockBar::IID );

  //Loop through the IFoxDockBars
  for ( PluginItr i = plugins.begin(); i != plugins.end(); ++i )
  {
    //We know it will be valid
    //Static cast needed to make g++ happy
    Usul::Interfaces::IFoxDockBar::ValidQueryPtr dockbar( static_cast< Usul::Interfaces::IFoxDockBar* > ( (*i).get() ) );
    dockbar->addDockBar ( me.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the plugin menus.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initPluginMenus()
{
  typedef Usul::Interfaces::IFoxMenuPane Interface;

  //Get the plugins
  PluginSet plugins ( PluginManager::instance().getInterfaces( Interface::IID ) );

  //this as an IUnknown
  Usul::Interfaces::IUnknown::ValidQueryPtr me ( this->asUnknown() );

  //Loop through the IFoxMenuPanes
  for ( PluginSet::iterator i = plugins.begin(); i != plugins.end(); ++i )
  {
    Usul::Interfaces::IFoxMenuPane::QueryPtr menu ( (*i).get() );
    if ( menu.valid() )
      menu->buildMenu ( me.get(), _menuBar );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the tools menu.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initToolsMenu()
{
  // Return now if there are no plugins.
  if ( PluginManager::instance().empty() )
    return;

  MenuGroup::ValidRefPtr tools ( new MenuGroup ( "&Tools" ) );
  
  //Get the plugins
  PluginSet plugins ( PluginManager::instance().unknowns() );

  typedef std::map< std::string, PluginSet > PluginGroups;
  PluginGroups pluginGroups;

  // Loop through the plugins and get thier group
  // "" Is the top level group ( no group ).
  for ( PluginItr i = plugins.begin(); i != plugins.end(); ++i )
  {
    // Does this plugin add a menu?
    Usul::Interfaces::IMenuEntry::QueryPtr entry ( i->get() );

    // If it's valid put it in the correct group
    if( entry.valid() )
    {
      std::string group ( entry->menuGroup() );
      pluginGroups[ group ].insert( i->get() );
    }
    else
      pluginGroups[ "" ].insert( i->get() );
  }

  // Build the top level.  i.e. plugins with no grouping
  PluginSet topLevel ( pluginGroups[""] );

  // Add the menu
  this->_initPluginMenu( tools, topLevel );

  // Remove this plugin set
  pluginGroups.erase("");

  // Go through the rest of the plugin sets
  for( PluginGroups::const_iterator i = pluginGroups.begin(); i != pluginGroups.end(); ++ i )
  {
    MenuGroup* subMenu ( new MenuGroup ( i->first.c_str() ) );

    // Add the menu
    this->_initPluginMenu( subMenu, i->second );

    tools->append( subMenu );
  }


  // It's possible to get here and have the menu be empty. This may happen 
  // if every plugin fails to add itself to the menu.
  if ( !tools->empty() )
  {
    _menuBar->append( tools );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the menu for the plugin set.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initPluginMenu( FoxTools::Menu::Group *menuPane, const PluginSet& plugins )
{
  // Sets for different groups
  PluginSet radios, toggles, subMenus, rest;
  
  // Loop through the plugin list and sort by type of menu option.
  for ( ConstPluginItr i = plugins.begin(); i != plugins.end(); ++i )
  {
    if ( i->get()->queryInterface ( Usul::Interfaces::IFoxRadioButton::IID ) )
      radios.insert ( (*i).get() );
    else if ( i->get()->queryInterface ( Usul::Interfaces::IFoxToggleButton::IID ) )
      toggles.insert ( (*i).get() );
    else if ( i->get()->queryInterface ( Usul::Interfaces::IFoxSubMenu::IID ) )
      subMenus.insert ( (*i).get() );
    else
      rest.insert ( *i );
  }

#if 0
  if ( !radios.empty() )
  {
    // Add commands with radio buttons.
    for ( ConstPluginItr i = radios.begin(); i != radios.end(); ++i )
      this->_addMenuType < FX::FXMenuRadio > ( menuPane, (*i).get() );
    new FX::FXMenuSeparator ( menuPane );
  }
  if ( !toggles.empty() )
  {
    // Add commands with toggle buttons.
    for ( ConstPluginItr i = toggles.begin(); i != toggles.end(); ++i )
      this->_addMenuType < FX::FXMenuCheck > ( menuPane, (*i).get() );
    new FX::FXMenuSeparator ( menuPane );
  }
#endif
  if ( !subMenus.empty() )
  {
    // Add commands with sub menus.
    for ( ConstPluginItr i = subMenus.begin(); i != subMenus.end(); ++i)
    { 
      Usul::Interfaces::IFoxSubMenu::ValidQueryPtr foxSubMenu ( (*i).get() );
      foxSubMenu->buildSubMenu( this->asUnknown(), menuPane ); 
    }
    menuPane->append ( new FoxTools::Menu::Separator );
  }

  // Add the rest of the commands.
  for ( ConstPluginItr i = rest.begin(); i != rest.end(); ++i )
    this->_addCommand ( menuPane, (*i).get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the help menu.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initHelpMenu()
{
  MenuGroup::ValidRefPtr help ( new MenuGroup ( "&Help" ) );
    help->append ( new MenuButton ( "&About...", this, MainWindow::ID_ABOUT_DIALOG ) );
 _menuBar->append ( help );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "About" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandAbout ( FX::FXObject *, FX::FXSelector, void * )
{
  std::string message ( Usul::App::Controller::instance().aboutMessage() );

  // Show dialog.
  FoxTools::Dialogs::Message::info ( "OK", "About", message );

  std::cout << message << std::endl;

  // Handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is it created?
//
///////////////////////////////////////////////////////////////////////////////

bool MainWindow::isCreated() const
{
  return ( 0x0 != this->id() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open recent file.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandRecentFile ( FX::FXObject *, FX::FXSelector, void *ptr )
{
  std::string filename ( USUL_UNSAFE_CAST ( FX::FXchar *, ptr ) );
  this->_openDocument ( filename );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open dropped file.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandDropFile ( FX::FXObject *, FX::FXSelector, void *ptr )
{
  std::string filename ( USUL_UNSAFE_CAST ( FX::FXchar *, ptr ) );
  this->_openDocument ( filename );
  _recentFiles->appendFile ( filename );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the status-bar needs to be updated.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateStatusBar ( FX::FXObject *, FX::FXSelector, void *ptr )
{
  const std::string &text = this->getStatusBarText();
  FX::FXStatusLine *statusLine = this->statusBar()->getStatusLine();
  statusLine->setText ( text.c_str() );

  _memoryLabel->setText ( Usul::System::Memory::formatPhysical().c_str() );

  // Handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when one of the "Tool" buttons is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandTool ( FX::FXObject *object, FX::FXSelector selector, void *data )
{
  // Get the menu command.
  FX::FXMenuCommand *mc = SAFE_CAST_FOX ( FX::FXMenuCommand, object );
  if ( 0x0 == mc )
    return 0;

  // Get the command.
  MenuButton::RefPtr command ( reinterpret_cast < MenuButton *  > ( mc->getUserData() ) );
  UAttribute::RefPtr attribute ( reinterpret_cast < UAttribute* > ( command->userData() ) );
  Usul::Interfaces::ICommand::ValidQueryPtr ic ( attribute->value() );
  ic->execute( this->asUnknown() );

  // Handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when one of the "Tool" buttons needs to be updated.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateTool ( FX::FXObject *object, FX::FXSelector selector, void *data )
{
  // Enable or disable the window.
  FoxTools::Functions::enable ( 0x0 != this->activeView(), object );

  //Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send the message.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::notify ( IUnknown *caller, const std::string &message )
{
  if ( !message.empty() )
    FoxTools::Dialogs::Message::info ( "OK", "Info", message );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Open" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandDocumentOpen ( FX::FXObject *, FX::FXSelector, void * )
{
  //Set busy to true, make sure it resets
  Usul::State::Busy busy;

  // Open some files.
  this->_onCommandOpen();

  // Message handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Insert" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandDocumentInsert ( FX::FXObject *, FX::FXSelector, void * )
{
  Usul::App::Controller::instance().documentInsert( this->asUnknown() );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Save As" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandDocumentSaveAs ( FX::FXObject *, FX::FXSelector, void * )
{
  Usul::App::Controller::instance().documentSaveAs ( this->asUnknown() );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Save" button is pressed.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandDocumentSave ( FX::FXObject *sender, FX::FXSelector, void * )
{
  Usul::App::Controller::instance().documentSave ( this->asUnknown() );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open given documents. Punt iff user cancels, otherwise report the error.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_openDocuments ( const Filenames &names )
{
  // Clear the error stack.
  Usul::Errors::Stack::instance().clear();

  // Loop through the files.
  for ( Filenames::const_iterator i = names.begin(); i != names.end(); ++i )
  {
    // Get the file.
    std::string file ( *i );

    // Safely...
    try
    {
      // Read the file.
      this->_openDocument ( file );

      // Save in the recent-file list.
      if ( _recentFiles.valid() )
        _recentFiles->appendFile ( file );
    }

    // Did the user cancel?
    catch ( const Usul::Exceptions::Canceled & )
    {
      throw;
    }

    // Catch standard exceptions.
    catch ( const std::exception &e )
    {
      Usul::Errors::Stack::instance().push ( e.what() );
      Usul::Errors::Stack::instance().push ( "Error 4211594406: Standard exception caught when opening file: " + file );
    }

    // Catch all other exceptions.
    catch ( ... )
    {
      Usul::Errors::Stack::instance().push ( "Error 2431206823: Unknown exception caught when opening file: " + file );
    }

    // If there is anything on the error stack...
    if ( false == Usul::Errors::Stack::instance().empty() )
    {
      // Make the message string.
      std::ostringstream out;
      out << "The following "
          << ( ( Usul::Errors::Stack::instance().size() > 1 ) ? "errors were" : "error was" )
          << " encountered when attempting to load: "
          << file
          << "\n\n"
          << Usul::Errors::Stack::instance().format ( "    " );

      // How many files left to load?
      const unsigned int remaining ( std::distance ( i, names.end() ) - 1 );

      // If we have more to open...
      if ( remaining > 0 )
      {
        // Append above message with appropriate question.
        out << "\n\n"
            << remaining
            << ( ( remaining > 1 ) ? " files remain" : " file remains" ) 
            << " to be loaded. Continue?";

        // Prompt the user.
        FoxTools::Dialogs::Message dialog ( "ask_user_about_loading_remaining_files", true, false );
        dialog.text ( "Error!", out.str() );
        dialog.icon ( FoxTools::Icons::Factory::ICON_QUESTION );
        dialog.buttons ( "Yes", "No" );
        if ( "No" == dialog.run ( 0x0 ) )
          return;
      }

      // Otherwise, this was the last file.
      else
      {
        // Let the user know.
        FoxTools::Dialogs::Message::error ( "OK", "Error!", out.str() );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open a new window.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_openDocument ( const std::string &filename )
{
  Usul::App::Controller::instance().documentOpen ( filename, this->asUnknown() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Open" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_onCommandOpen()
{
  // Get appropriate filters.
  Filters filters ( Usul::Documents::Manager::instance().filtersOpen() );

  // Ask for file names.
  ILoadFileDialog::FilesResult result ( this->getLoadFileNames ( "Open", filters ) );

  // Open files.
  this->_openDocuments ( result.first );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Export" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_onCommandExport()
{
  // For convenience.
  typedef FoxTools::Dialogs::FileSelection FileDialog;
  typedef FileDialog::FileResult Result;

  Filters filters;
  Usul::App::Controller::instance().filtersExport ( filters );

  // Ask for file name.
  Result result ( FileDialog::askForFileName ( FileDialog::SAVE, "Export", filters, 0x0, true ) );
  const std::string &filename = result.first;

  // Make sure there is a file.
  if ( filename.empty() )
    return;

  // The wait-cursor.
  FoxTools::Adaptors::Wait wait;

  // Export the file.
  Usul::App::Controller::instance().documentExport( filename, result.second.first, this->asUnknown() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show dialog to get file name.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ILoadFileDialog::FileResult MainWindow::getLoadFileName ( const std::string &title, const Filters &filters )
{
  typedef FoxTools::Dialogs::FileSelection FileDialog;
  FileDialog::FileResult result ( FileDialog::askForFileName ( FileDialog::OPEN, title, filters, 0x0, true ) );
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show dialog to get list of file names.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ILoadFileDialog::FilesResult MainWindow::getLoadFileNames ( const std::string &title, const Filters &filters )
{
  typedef FoxTools::Dialogs::FileSelection FileDialog;
  FileDialog::FilesResult result ( FileDialog::askForFileNames ( FileDialog::OPEN, title, filters, 0x0, true ) );
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show dialog to get file name.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::ISaveFileDialog::FileResult MainWindow::getSaveFileName ( const std::string &title, const Filters &filters )
{
  typedef FoxTools::Dialogs::FileSelection FileDialog;
  FileDialog::FileResult result ( FileDialog::askForFileName ( FileDialog::SAVE, title, filters, 0x0, true ) );
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show dialog to get file names.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::ISaveFileDialog::FilesResult MainWindow::getSaveFileNames ( const std::string &title, const Filters &filters )
{
  typedef FoxTools::Dialogs::FileSelection FileDialog;
  FileDialog::FilesResult result ( FileDialog::askForFileNames ( FileDialog::SAVE, title, filters, 0x0, true ) );
  return result;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Window is being closed.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandClose ( FXObject *object, FXSelector id, void *data )
{
  // Delegate.
  if ( !this->_exiting() )
    return 1;

  // Call the base class's function.
  return BaseClass::onCmdClose ( object, id, data );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when File->Exit is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandExit ( FXObject *object, FXSelector id, void *data )
{
  // Delegate.
  if ( !this->_exiting() )
    return 1;

  // Call the base class's function.
  return BaseClass::onCmdClose ( object, id, data );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the program exits.
//
///////////////////////////////////////////////////////////////////////////////

bool MainWindow::_exiting()
{
  // Need scope here to make sure that me is unrefed before assert below
  {
    // Notify plugins of closing
    Usul::Interfaces::IUnknown::ValidQueryPtr me ( this->asUnknown() );

    PluginSet plugins ( PluginManager::instance().getInterfaces( Usul::Interfaces::INotifyClose::IID ) );
    for ( PluginItr i = plugins.begin(); i != plugins.end(); ++i )
    {
      Usul::Interfaces::INotifyClose::QueryPtr notify ( (*i).get() );
      if ( notify.valid() )
        if ( !notify->notifyClose ( me.get() ) )
          return false;
    }
  }

  // Save values in the registry.
  this->_writeToRegistry();

  typedef Usul::Documents::Manager::Documents Documents;
  
  // Get any remaining documents.  These documents aren't associated with any window.
  Documents &documents ( Usul::Documents::Manager::instance().documents() );

  // Set delegates to null.  This is needed so delegates are unreferenced and properly deleted.
  for ( Documents::iterator i = documents.begin(); i != documents.end(); ++i )
    (*i)->delegate ( 0x0 );

  // Clear documents.
  Usul::Documents::Manager::instance().documents().clear();

  // Release the preferences.
  OsgFox::Core::Preferences::releaseInstance();

  // Clear the menu before the plugins are released.
  _recentFiles->clear();
  _menuBar->clear();

  // Release the toolbars
  FoxTools::ToolBar::Factory::instance().release();

  // Release all our plugins
  PluginManager::instance().clear ( &std::cout );

  // Unset these pointers
  Usul::Resources::cancelButton ( 0x0 );
  Usul::Resources::flushEvents  ( 0x0 );
  Usul::Resources::progressBar  ( 0x0 );
  Usul::Resources::reportErrors ( 0x0 );
  Usul::Resources::statusBar    ( 0x0 );
  Usul::Resources::textWindow   ( 0x0 );
  Usul::Resources::menuBar      ( 0x0 );

  // Should be true.
  USUL_ASSERT ( 0 == _refCount );

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the necessary values to the registry.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_writeToRegistry() const
{
  // Is it maximized?
  FoxTools::Registry::write ( Usul::Registry::Sections::MAIN_WINDOW, Usul::Registry::Keys::MAXIMIZED, this->isMaximized() );

  // If we are not maximized...
  if ( !this->isMaximized() )
  {
    // Write the size and position to the registry.
    FX::FXint x ( Usul::Math::maximum ( ( this->getX() ), 0 ) );
    FX::FXint y ( Usul::Math::maximum ( ( this->getY() ), 0 ) );
    FX::FXint w ( this->getWidth() );
    FX::FXint h ( this->getHeight() );
    FoxTools::Registry::write ( Usul::Registry::Sections::MAIN_WINDOW, Usul::Registry::Keys::X,      x );
    FoxTools::Registry::write ( Usul::Registry::Sections::MAIN_WINDOW, Usul::Registry::Keys::Y,      y );
    FoxTools::Registry::write ( Usul::Registry::Sections::MAIN_WINDOW, Usul::Registry::Keys::WIDTH,  w );
    FoxTools::Registry::write ( Usul::Registry::Sections::MAIN_WINDOW, Usul::Registry::Keys::HEIGHT, h );
  }

  // The tab-book.
  if ( _tabBook )
    FoxTools::Registry::write ( Usul::Registry::Sections::TAB_BOOK, Usul::Registry::Keys::WIDTH, _tabBook->getWidth() );

  // The horizontal splitter.
  if ( _hSplitter.first )
    FoxTools::Registry::write ( Usul::Registry::Sections::HORIZONTAL_SPLITTER, Usul::Registry::Keys::SPLIT, _hSplitter.second );

  // Flag that allows spinning.
  Usul::Shared::Preferences &prefs ( Usul::Shared::Preferences::instance() );
  const bool spinning ( prefs.getBool ( Usul::Registry::Keys::ALLOW_SPIN ) );
  FoxTools::Registry::write ( Usul::Registry::Sections::VIEWER_SETTINGS, Usul::Registry::Keys::ALLOW_SPIN, spinning );

  // Flag that specifies application-wide display list usage.
  const bool displayLists ( prefs.getBool ( Usul::Registry::Keys::DISPLAY_LISTS ) );
  FoxTools::Registry::write ( Usul::Registry::Sections::VIEWER_SETTINGS, Usul::Registry::Keys::DISPLAY_LISTS, displayLists );

  // Tell preferences to write to registry.
  OsgFox::Core::Preferences::instance()->writeOutValues();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Report any errors.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::reportErrors ( unsigned int options, bool clear )
{
  // Handle nested calls.
  static bool here ( false );
  if ( here )
    return;
  Usul::Scope::Reset<bool> reset ( here, true, false );

  // Safely...
  try
  {
    // Handle no errors.
    if ( ErrorStack::instance().empty() )
      return;

    // Get the errors and format them nicely.
    std::string errors ( ErrorStack::instance().format() );

    // These have to be templates to make CC happy on SGI
    // Print to standard out.
    if ( Usul::Bits::has<unsigned int, unsigned int> ( options, Usul::Interfaces::IReportErrors::STANDARD_OUT ) ) 
      std::cout << errors << std::endl;

    // Print to standard error.
    if ( Usul::Bits::has<unsigned int, unsigned int> ( options, Usul::Interfaces::IReportErrors::STANDARD_ERROR ) )
      std::cerr << errors << std::endl;

    // Display errors on a dialog.
    if ( Usul::Bits::has<unsigned int, unsigned int> ( options, Usul::Interfaces::IReportErrors::ERROR_DIALOG ) )
      FoxTools::Dialogs::Message::error ( "OK", "Error", errors, true );

    // Clear the error stack if we should.
    if ( clear )
      ErrorStack::instance().clear();
  }

  // Eat all exceptions.
  catch ( ... )
  {
    std::cout << "Error 3341674440:" << std::endl;
    std::cout << "\tException caught while reporting errors." << std::endl;
    std::cout << "\tYou should restart the program." << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the command to the pane.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_addCommand ( FoxTools::Menu::Group *pane, Usul::Interfaces::IUnknown *unknown )
{
  // Get needed interfaces.
  Usul::Interfaces::ICommand::QueryPtr command ( unknown );
  Usul::Interfaces::IMenuEntry::QueryPtr name ( unknown );

  // Make sure the interfaces are supported.
  if ( !command.valid() || !name.valid() )
    return;

  // Add the button.
  MenuButton::ValidRefPtr button ( new MenuButton ( name->menuText(), name->hotKeyText(), name->statusBarText(), this, MainWindow::ID_TOOL_COMMAND ) );

  UAttribute::RefPtr ud ( new UAttribute ( command.get() ) );
  button->userData ( ud.get() );
  pane->append( button );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the either button of check box to the pane.
//
///////////////////////////////////////////////////////////////////////////////

template < class MenuType > 
void MainWindow::_addMenuType ( FoxTools::Menu::Group *pane, Usul::Interfaces::IUnknown *unknown )
{
  // Get needed interfaces.
  Usul::Interfaces::ICommand::QueryPtr command ( unknown );
  Usul::Interfaces::IMenuEntry::QueryPtr name ( unknown );

  // Make sure the interfaces are supported.
  if ( !command.valid() || !name.valid() )
    return;

#if 0
  // Add the button.
  MenuType::ValidRefPtr button ( new MenuType ( name->menuText(), name->hotKeyText(), name->statusBarText(), this, MainWindow::ID_TOOL_COMMAND ) );

  Attribute::RefPtr ud ( new Attribute ( command.get() ) );
  button->userData ( ud.get() );
  pane->append( button );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::ref()
{
  // Keep track for debugging purposes.
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::unref ( bool allowDeletion )
{
  // Keep track for debugging purposes.
  --_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *MainWindow::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::INotifyString::IID:
    return static_cast<Usul::Interfaces::INotifyString*>(this);
  case Usul::Interfaces::IProgressBar::IID:
    return static_cast<Usul::Interfaces::IProgressBar*>(this);
  case Usul::Interfaces::IStatusBar::IID:
    return static_cast<Usul::Interfaces::IStatusBar*>(this);
  case Usul::Interfaces::ISaveFileDialog::IID:
    return static_cast< Usul::Interfaces::ISaveFileDialog*> ( this );
  case Usul::Interfaces::ILoadFileDialog::IID:
    return static_cast< Usul::Interfaces::ILoadFileDialog*> ( this );
  case Usul::Interfaces::ICancelButton::IID:
    return static_cast< Usul::Interfaces::ICancelButton*> ( this );
  case Usul::Interfaces::IReportErrors::IID:
    return static_cast< Usul::Interfaces::IReportErrors*> (this);
  case Usul::Interfaces::IFoxTabItem::IID:
    return static_cast< Usul::Interfaces::IFoxTabItem* > (this);
  case Usul::Interfaces::IPreferencesManager::IID:
    return static_cast< Usul::Interfaces::IPreferencesManager* > ( this );
  case Usul::Interfaces::IActiveView::IID:
    return static_cast<Usul::Interfaces::IActiveView*>(this);
  case Usul::Interfaces::IFlushEvents::IID:
    return static_cast<Usul::Interfaces::IFlushEvents*>(this);
  case Usul::Interfaces::IUpdateTextWindow::IID:
    return static_cast<Usul::Interfaces::IUpdateTextWindow*>(this);
  case Usul::Interfaces::IFoxTabBook::IID:
    return static_cast< Usul::Interfaces::IFoxTabBook* > ( this );
  case Usul::Interfaces::IActiveDocument::IID:
    return static_cast < Usul::Interfaces::IActiveDocument* >  ( this );
  case Usul::Interfaces::IQuestion::IID:
    return static_cast < Usul::Interfaces::IQuestion* > ( this );
  case Usul::Interfaces::IMenuBar::IID:
    return static_cast < Usul::Interfaces::IMenuBar* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the tab item.
//
///////////////////////////////////////////////////////////////////////////////
 
void MainWindow::addTab ( const std::string &name )
{
  if( !_tabBook->shown() )
  {
    _tabBook->show();
  }

  std::auto_ptr<FX::FXTabItem> item ( new FX::FXTabItem  ( _tabBook, name.c_str(), 0x0 ) );
  _tabItems[name] = TabItems::mapped_type ( item.release(), 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the tab item.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::deleteTab ( const std::string &name )
{
  // Look for the tab.
  TabItems::iterator i = _tabItems.find ( name );
  if ( _tabItems.end() != i )
  {
    // Delete the tab and the accompanying window.
    delete i->second.first;
    delete i->second.second;

    // Remove this entry from the map.
    _tabItems.erase ( i );
  }
  if( _tabBook && _tabItems.empty() )
    _tabBook->hide();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete all the tab items.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::deleteTabs()
{
  // Loop through and delete all the FOX windows.
  for ( TabItems::iterator i = _tabItems.begin(); i != _tabItems.end(); ++i )
  {
    delete i->second.first;
    delete i->second.second;
  }

  // Clear the map.
  _tabItems.clear();
  _tabBook->hide();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tab item.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXTabItem *MainWindow::tabItem ( const std::string &name )
{
  TabItems::iterator i = _tabItems.find ( name );
  return ( _tabItems.end() != i ) ? i->second.first : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tab item.
//
///////////////////////////////////////////////////////////////////////////////

const FX::FXTabItem *MainWindow::tabItem ( const std::string &name ) const
{
  TabItems::const_iterator i = _tabItems.find ( name );
  return ( _tabItems.end() != i ) ? i->second.first : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tabbed window.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXWindow *MainWindow::tabWindow ( const std::string &name )
{
  TabItems::iterator i = _tabItems.find ( name );
  return ( _tabItems.end() != i ) ? i->second.second : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tabbed window.
//
///////////////////////////////////////////////////////////////////////////////

const FX::FXWindow *MainWindow::tabWindow ( const std::string &name ) const
{
  TabItems::const_iterator i = _tabItems.find ( name );
  return ( _tabItems.end() != i ) ? i->second.second : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the tabbed window.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::tabWindow ( const std::string &name, FX::FXWindow *window )
{
  // Look for the window.
  TabItems::iterator i = _tabItems.find ( name );
  if ( _tabItems.end() == i )
  {
    std::ostringstream message;
    message << "Error 3309480910: failed to find tab item: " << name << '.';
    message << "\n\tFirst call addTab() with the same string.";
    throw std::runtime_error ( message.str() );
  }

  // Set the window.
  i->second.second = window;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::showProgressBar()
{
  // If we have a progress bar...
  if( _progressBar.first )
  {
    if ( 0 == _progressBar.second )
    {
      _progressBar.first->show();
      _progressBar.first->getParent()->layout();
      _progressBar.first->getParent()->repaint();
      this->_flush();
    }

    // Always increment.
    ++(_progressBar.second);
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the total of progress bar
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::setTotalProgressBar ( unsigned int value )
{
  // If we have a progress bar...
  if ( _progressBar.first )
    _progressBar.first->setTotal ( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::updateProgressBar ( unsigned int value )
{
  // If we have a progress bar...
  if( _progressBar.first )
  {
    _progressBar.first->setProgress ( value );
    this->forceRefresh();
    this->_flush();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the progress bar
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::hideProgressBar()
{
  // If we have a progress bar...
  if( _progressBar.first )
  {
    // Decrement if we can.
    if ( _progressBar.second > 0 )
    {
      --(_progressBar.second);
    }

    if ( 0 == _progressBar.second )
    {
      _progressBar.first->hide();
      _progressBar.first->getParent()->layout();
      _progressBar.first->getParent()->repaint();
      this->_flush();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Active View
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* MainWindow::getActiveView()
{
  return this->activeView();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar text
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::setStatusBarText ( const std::string &text, bool force ) 
{
  // Set the text.
  _statusText = text;

  // Force it if we should.
  if ( force )
  {
    // Not sure if this does the trick...
    _statusBar->getStatusLine()->setText ( text.c_str() );
    _statusBar->getParent()->layout();
    _statusBar->getParent()->repaint();
    this->_flush();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  When the cancel button is pressed
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandCancel ( FX::FXObject *, FX::FXSelector, void * )
{
  // If we are in nested event loops then we are most likely in a 
  // FX::FXApp::runWhileEvents(), so we should be able to safely throw this 
  // exception to cancel whatever is going on.
  Application *app ( SAFE_CAST_FOX ( Application, this->getApp() ) );
  if ( app && app->eventQueueLevel() > 1 )
    throw Usul::Exceptions::Canceled ( "User cancelled the operation" );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the cancel button
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::showCancelButton()
{
  _cancelButton->show();
  _cancelButton->getParent()->layout();
  _cancelButton->getParent()->repaint();
  this->_flush();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the cancel button
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::hideCancelButton()
{
  _cancelButton->hide();
  _cancelButton->getParent()->layout();
  _cancelButton->getParent()->repaint();
  this->_flush();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the mouse moves over the window.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onMotion ( FX::FXObject *object, FX::FXSelector selector, void *callData )
{
  // Was going to pipe the size of the maximized active view to the 
  // status-bar. However, this handler is not getting called.
  return BaseClass::onMotion ( object, selector, callData );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable if the active view has the bounding box showing.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateBoundingBox ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Set the check and enable/disable.
  FoxTools::Functions::enable ( Usul::App::Controller::instance().boundingBoxEnable(), object );
  FoxTools::Functions::check  ( Usul::App::Controller::instance().boundingBoxCheck(), object );

  // Handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable if the active view has the bounding sphere showing.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateBoundingSphere ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Set the check and enable/disable.
  FoxTools::Functions::enable ( Usul::App::Controller::instance().boundingSphereEnable(), object );
  FoxTools::Functions::check  ( Usul::App::Controller::instance().boundingSphereCheck (), object );

  // Handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn on/off the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandBoundingBox ( FX::FXObject *object, FX::FXSelector, void * )
{
  Usul::App::Controller::instance().boundingBoxToggle();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn on/off the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandBoundingSphere ( FX::FXObject *object, FX::FXSelector, void * )
{
  Usul::App::Controller::instance().boundingSphereToggle();
  return 1;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the tab control
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initTabControl()
{
  // The tab and tree controls.
  FX::FXint width ( FoxTools::Registry::read ( Usul::Registry::Sections::TAB_BOOK, Usul::Registry::Keys::WIDTH, (FXint) ( 0.2 * this->getWidth() ) ) );
  _tabBook = new FX::FXTabBook  ( _vSplitter.first, 0x0, 0 , FX::FRAME_SUNKEN | FX::TABBOOK_BOTTOMTABS | FX::PACK_UNIFORM_WIDTH | FX::PACK_UNIFORM_HEIGHT | FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y | FX::LAYOUT_RIGHT, 0, 0, width  );

  // MainWindow as an IUnknown
  Usul::Interfaces::IUnknown::ValidQueryPtr me ( this );

  // Get the plugins from the Plugin Manager
  PluginSet plugins ( PluginManager::instance().getInterfaces( Usul::Interfaces::IFoxAddTab::IID ) );

  // Loop through IFoxAddTabs
  for ( PluginSet::iterator i = plugins.begin(); i != plugins.end(); ++i )
  {
    Usul::Interfaces::IFoxAddTab::ValidQueryPtr addTab ( (*i).get() );
    addTab->foxAddTab ( me.get() );
  }

  // If nobody added tabs, hide the tab book
  if ( _tabItems.empty() )
    _tabBook->hide();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the preferences as an Iunknown
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* MainWindow::getPreferences ( )
{
  return OsgFox::Core::Preferences::instance()->queryInterface( Usul::Interfaces::IUnknown::IID );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide/Show toolbar
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandToolbarToggle ( FX::FXObject *object, FX::FXSelector, void* )
{
  FX::FXMenuCheck *check = SAFE_CAST_FOX( FX::FXMenuCheck, object );
  if( !check )
    return 0;

  //Get the toolbar's name
  std::string name ( (char *) check->getText().text() );

  //Get the toolbar pointer
  ToolBar::ValidRefPtr toolBar ( FoxTools::ToolBar::Factory::instance().toolbar( name ) );
  
  //Toggle hide/show
  if( toolBar->shown() )
    toolBar->hide();
  else
    toolBar->show();

  // Handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the checks of shown toolbars
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateToolbarToggle ( FX::FXObject *object, FX::FXSelector, void* )
{
  FX::FXMenuCheck *check = SAFE_CAST_FOX( FX::FXMenuCheck, object );
  if( !check )
    return 0;

  //Get the name of the tool bar
  std::string name ( (char *) check->getText().text() );

  //Get the toolbar pointer
  ToolBar::ValidRefPtr toolBar ( FoxTools::ToolBar::Factory::instance().toolbar( name ) );

  check->setCheck( toolBar->shown() );

  // Handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle key press
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onKeyPress ( FX::FXObject *object, FX::FXSelector id, void *data )
{
  // Get the event.
  FOX_TOOLS_ERROR_CHECKER ( 0x0 != data );
  FX::FXEvent *event ( USUL_UNSAFE_CAST ( FX::FXEvent *, data ) );

  // Process the code.
  switch ( event->code )
  {
    case FX::KEY_F4:
      if ( event->state & FX::ALTMASK )
        return this->onCommandClose ( object, id, data );

    case FX::KEY_F11:
      this->_fullScreenToggle();
      return 1;
  }

  // Redirect to base class.
  return BaseClass::onKeyPress( object, id, data );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle full-screen mode.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_fullScreenToggle()
{
  if ( _fullScreen )
    this->_fullScreenEnd();
  else
    this->_fullScreenStart();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start full screen mode
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_fullScreenStart()
{
  // We are in full screen mode.
  _fullScreen = true;
  
  // Remember if we were already maximized.
  _wasMaximized = ( this->isMaximized() ? true : false );

  // Maximize the main window.
  this->maximize();

  // Hide necessary widgets.
  _statusBar->hide();
  
  FoxTools::ToolBar::Factory::instance().hide();
  
  this->_hideTabWindow();
  this->_hideTextWindow();

  // Make sure we update and redraw.
  this->layout();
  this->repaint();
  this->forceRefresh();
  this->_flush();
}


///////////////////////////////////////////////////////////////////////////////
//
//  End full screen mode
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_fullScreenEnd()
{
  // Stop full screen mode.
  _fullScreen = false;

  // Were we maximized before?
  if ( _wasMaximized )
    this->maximize();
  else
    this->restore();

  // Show widgets.
  _statusBar->show();
  
  FoxTools::ToolBar::Factory::instance().show();
  
  this->_showTabWindow();
  this->_showTextWindow();

  // Make sure we upate and redraw.
  this->layout();
  this->repaint();
  this->forceRefresh();
  this->_flush();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Restore the window.  TODO This isn't being called for some reason
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandRestore ( FX::FXObject *object, FX::FXSelector id, void *data )
{
  if( _fullScreen )
  {
    this->_fullScreenEnd();
    return 1;
  }

  return BaseClass::onCmdRestore( object, id, data );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle full screen mode
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandFullScreen ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Toggle full screen mode.
  this->_fullScreenToggle();

  // Handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the full screen check button in view menu
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateFullScreen ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Safely set the state.
  FoxTools::Functions::enable ( ( 0x0 != this->activeView() ), object );

  // Safely set the check.
  FoxTools::Functions::check ( _fullScreen, object );

  // Message handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Sees if active document has been modified.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateNeedingModified ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Safely set the state.
  FoxTools::Functions::enable ( this->activeDocument() && this->activeDocument()->modified() && !Usul::State::busy(), object );

  // Message is handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export the scene
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandExport ( FX::FXObject *, FX::FXSelector, void * )
{
  // Delegate.
  this->_onCommandExport();

  // Message is handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flush the event queue.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::flushEventQueue()
{
  this->getApp()->runWhileEvents();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Disable if busy
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateNotBusy ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::enable ( !Usul::State::busy(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Disable if no document, or if busy.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateNeedingDocument ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::enable ( this->activeDocument() && !Usul::State::busy(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Flip Normals" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandFlipNormals ( FX::FXObject *, FX::FXSelector, void * )
{
  Usul::App::Controller::instance().flipNormals( this->asUnknown() );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Flip Normals" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateFlipNormals ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::enable ( Usul::App::Controller::instance().flipNormalsEnable(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the tab-book.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXTabBook * MainWindow::tabBook()
{
  return _tabBook;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the tab-book.
//
///////////////////////////////////////////////////////////////////////////////

const FX::FXTabBook * MainWindow::tabBook() const
{
  return _tabBook;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the active docuemnt.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* MainWindow::getActiveDocument()
{
  return this->activeDocument()->queryInterface ( Usul::Interfaces::IUnknown::IID );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the text window needs to be updated.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateTextWindow ( FX::FXObject *, FX::FXSelector, void * )
{
  this->updateTextWindow ( false );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the text window.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::updateTextWindow ( bool force )
{
  // Handle file not set.  Or if we don't have a Text Window.
  if ( _output.empty() || 0x0 == _textWindow )
    return;

  // Don't allow this to throw because it may create an infinite loop.
  try
  {
    // Get the file contents.
    std::string text;
    Usul::File::contents ( _output, false, text );

    // If nothing changed...
    if ( text.size() == static_cast < std::string::size_type > ( _textWindow->getText().length() ) )
      return;

    // Set the text window.
    _textWindow->setText ( text.c_str() );

    // Move cursor to the end.
    _textWindow->setCursorPos ( text.size() - 1 );

    // Scroll to the end.
    _textWindow->makePositionVisible ( _textWindow->getCursorPos() );

    // Force it if we should.
    if ( force )
    {
      _textWindow->getParent()->layout();
      _textWindow->getParent()->repaint();
      this->_flush();
    }
  }

  // Catch all exceptions.
  catch ( ... )
  {
    std::ostringstream out;
    out << _textWindow->getText().text() << "\nError 3191148251: failed to access output file '" << _output;
    _textWindow->setText ( out.str().c_str() );
  }

  // Handled.
  return;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the text window
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_hideTextWindow()
{
  // Return now if already hidden
  if ( !_textWindow->shown() )
    return;

  _textWindow->hide();
  _hSplitter.first->setSplit ( 1, 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the text window
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_showTextWindow()
{
  // Return now if already shown
  if ( _textWindow->shown() )
    return;

  _textWindow->show();
  _hSplitter.first->setSplit ( 1, _hSplitter.second );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the tab window
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_hideTabWindow()
{
  // Return now if already hidden
  if( !_tabBook->shown() )
    return;

  _tabBook->hide();
  _vSplitter.second = _vSplitter.first->getSplit ( 0 );
  _vSplitter.first->setSplit ( 0, 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the tab window
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_showTabWindow()
{
    // Return now if already shown
  if( _tabBook->shown() )
    return;

  _tabBook->show();
  _vSplitter.first->setSplit ( 0, _vSplitter.second  );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a new document
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandDocumentNew ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the menu command.
  FX::FXMenuCommand *mc = SAFE_CAST_FOX ( FX::FXMenuCommand, object );
  if ( 0x0 == mc )
    return 0;

  // Get the command.
  MenuButton::ValidRefPtr command   ( reinterpret_cast < MenuButton * > ( mc->getUserData()   ) );
  UAttribute::ValidRefPtr attribute ( reinterpret_cast < UAttribute* >  ( command->userData() ) );
  
  Usul::App::Controller::instance().documentNew ( attribute->value(), this->asUnknown() );

  // Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get this pointer as an IUnknown
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* MainWindow::asUnknown()
{
  return this->queryInterface( Usul::Interfaces::IUnknown::IID );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open a new window with the current document
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandNewWindow ( FX::FXObject *, FX::FXSelector, void * )
{
  Usul::App::Controller::instance().newWindow( this->asUnknown() );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the throw button needs to be updated.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateAllowSpinning ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::check ( Usul::App::Controller::instance().spinCheck(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called to change the throw state.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandAllowSpinning ( FX::FXObject *, FX::FXSelector, void * )
{
  Usul::App::Controller::instance().spinToggle();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the low lods button needs to be updated.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateLowLods ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::check ( Usul::App::Controller::instance().lowLowsCheck(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called to change the low lods state.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandLowLods ( FX::FXObject *, FX::FXSelector, void * )
{
  Usul::App::Controller::instance().lowLods();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the high lods button needs to be updated.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateHighLods ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::check ( Usul::App::Controller::instance().highLodsCheck(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called to change the high lods state.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandHighLods ( FX::FXObject *, FX::FXSelector, void * )
{
  Usul::App::Controller::instance().highLods();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Display lists settings.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandDisplayLists ( FX::FXObject *, FX::FXSelector, void *data )
{
  Usul::App::Controller::instance().displayListsToggle();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update display lists button.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateDisplayLists ( FX::FXObject *object, FX::FXSelector, void *data )
{
  const bool state ( Usul::App::Controller::instance().displayLists() );
  FoxTools::Functions::check ( state, object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prompt a question dialog to the user
//
///////////////////////////////////////////////////////////////////////////////

std::string MainWindow::question ( const std::string &buttons,  const std::string &title, const std::string &text )
{
  return FoxTools::Dialogs::Message::question( buttons, title, text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle two sided lighting.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandTwoSidedLighting  ( FX::FXObject *object, FX::FXSelector, void *data )
{
  Usul::App::Controller::instance().lightingTwoSidedSet ( data ? true : false );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update two sided lighting check box.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateTwoSidedLighting  ( FX::FXObject *object, FX::FXSelector, void *data )
{
  FoxTools::Functions::enable ( Usul::App::Controller::instance().lightingTwoSidedEnable(), object );
  FoxTools::Functions::check  ( Usul::App::Controller::instance().lightingTwoSidedCheck(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle lighting.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandLighting  ( FX::FXObject *object, FX::FXSelector, void *data )
{
  Usul::App::Controller::instance().lightingSet( data ? true : false );
  return 1;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Update lighting check box.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateLighting  ( FX::FXObject *object, FX::FXSelector, void *data )
{
  FoxTools::Functions::enable ( Usul::App::Controller::instance().lightingEnable(), object );
  FoxTools::Functions::check (  Usul::App::Controller::instance().lightingCheck(), object );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the navigation mode
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandModeNavigation ( FX::FXObject *, FX::FXSelector, void * )
{
  Usul::App::Controller::instance().modeNavigatingSet();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the pick mode
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandModePick ( FX::FXObject *, FX::FXSelector, void * )
{
  Usul::App::Controller::instance().modePickingSet();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update navigation in the menu.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateNavigation ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::enable ( Usul::App::Controller::instance().modeNavigatingEnable(), object );
  FoxTools::Functions::check  ( Usul::App::Controller::instance().modeNavigatingCheck(), object );
  return 1;
}

  
///////////////////////////////////////////////////////////////////////////////
//
//  Update pick in the mode menu
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdatePick ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::enable ( Usul::App::Controller::instance().modePickingEnable(), object );
  FoxTools::Functions::check  ( Usul::App::Controller::instance().modePickingCheck(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Preferences" menu option is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandPreferences ( FX::FXObject *, FX::FXSelector, void * )
{
  OsgFox::Core::Preferences::instance()->showDialog();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Edit Background" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandEditBackground ( FX::FXObject *, FX::FXSelector, void * )
{
  Usul::App::Controller::instance().backgroundEdit();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Default Background" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandDefaultBackground ( FX::FXObject *, FX::FXSelector, void * )
{
  Usul::App::Controller::instance().backgroundDefault();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the active view implement IBackground
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateNeedingIBackground ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::enable ( Usul::App::Controller::instance().backgroundEnable(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the camera button is picked.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_onCommandCamera ( unsigned int option )
{
  Usul::App::Controller::instance().camera( option );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "View Fit" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandCameraFit ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandCamera ( Usul::Interfaces::ICamera::FIT );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "View Reset" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandCameraReset ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandCamera ( Usul::Interfaces::ICamera::RESET );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "View Front" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandCameraFront ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandCamera ( Usul::Interfaces::ICamera::FRONT );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "View Back" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandCameraBack ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandCamera ( Usul::Interfaces::ICamera::BACK );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "View Left" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandCameraLeft ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandCamera ( Usul::Interfaces::ICamera::LEFT );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "View Right" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandCameraRight ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandCamera ( Usul::Interfaces::ICamera::RIGHT );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "View Top" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandCameraTop ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandCamera ( Usul::Interfaces::ICamera::TOP );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "View Bottom" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandCameraBottom ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandCamera ( Usul::Interfaces::ICamera::BOTTOM );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the active view implements ICamera
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateNeedingICamera ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::enable ( Usul::App::Controller::instance().cameraEnable(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a polygon mode is selected.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_onCommandPolygonMode ( IPolygonMode::Mode mode )
{
  Usul::App::Controller::instance().setPolygonMode( mode );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a shading model is selected.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_onCommandShadeModel ( IShadeModel::Mode mode )
{
  Usul::App::Controller::instance().setShadeModel( mode );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the menu button needs to be updated.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_onUpdatePolygonMode ( FX::FXObject *sender, IPolygonMode::Mode mode )
{
  // Enable and check the polygon mode button.
  FoxTools::Functions::enable ( Usul::App::Controller::instance().polygonModeEnable(), sender );
  FoxTools::Functions::check ( Usul::App::Controller::instance().hasPolygonMode( mode ), sender );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons Filled" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandPolygonsFilled ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandPolygonMode ( IPolygonMode::FILLED );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Hidden Lines" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandPolygonsHidden ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandPolygonMode ( IPolygonMode::HIDDEN_LINES );

  // Handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons Wire Frame" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandPolygonsWire ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandPolygonMode ( IPolygonMode::WIRE_FRAME );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons Points" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandPolygonsPoints ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandPolygonMode ( IPolygonMode::POINTS );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Shading Smooth" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandShadingSmooth ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandShadeModel ( IShadeModel::SMOOTH );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Shading Flat" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandShadingFlat ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandShadeModel ( IShadeModel::FLAT );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons As Is" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandPolygonsAsIs ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandPolygonMode ( IPolygonMode::NONE );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Shading As Is" button is picked.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandShadingAsIs ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_onCommandShadeModel ( IShadeModel::NONE );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons Filled" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdatePolygonsFilled ( FX::FXObject *sender, FX::FXSelector, void * )
{
  this->_onUpdatePolygonMode ( sender, IPolygonMode::FILLED );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons Wire Frame" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdatePolygonsWire ( FX::FXObject *sender, FX::FXSelector, void * )
{
  this->_onUpdatePolygonMode ( sender, IPolygonMode::WIRE_FRAME );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons Hidden Lines" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdatePolygonsHidden ( FX::FXObject *sender, FX::FXSelector, void * )
{
  this->_onUpdatePolygonMode ( sender, IPolygonMode::HIDDEN_LINES );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons Points" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdatePolygonsPoints ( FX::FXObject *sender, FX::FXSelector, void * )
{
  this->_onUpdatePolygonMode ( sender, IPolygonMode::POINTS );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Shading Smooth" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateShadingSmooth ( FX::FXObject *sender, FX::FXSelector, void * )
{
  FoxTools::Functions::enable ( Usul::App::Controller::instance().shadeModelEnable(), sender );
  FoxTools::Functions::check ( Usul::App::Controller::instance().hasShadeModel( IShadeModel::SMOOTH ), sender );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Shading Flat" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateShadingFlat ( FX::FXObject *sender, FX::FXSelector, void * )
{
  FoxTools::Functions::enable ( Usul::App::Controller::instance().shadeModelEnable(), sender );
  FoxTools::Functions::check ( Usul::App::Controller::instance().hasShadeModel( IShadeModel::FLAT ), sender );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Polygons As Is" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdatePolygonsAsIs ( FX::FXObject *sender, FX::FXSelector, void * )
{
  this->_onUpdatePolygonMode ( sender, IPolygonMode::NONE );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the "Shading As Is" button is updated.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateShadingAsIs ( FX::FXObject *sender, FX::FXSelector, void * )
{
  FoxTools::Functions::enable ( Usul::App::Controller::instance().shadeModelEnable(), sender );
  FoxTools::Functions::check ( Usul::App::Controller::instance().hasShadeModel( IShadeModel::NONE ), sender );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the active document.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Documents::Document* MainWindow::activeDocument()
{
  return Usul::App::Controller::instance().activeDocument();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the FoxTools menu bar.
//
///////////////////////////////////////////////////////////////////////////////

FoxTools::Menu::Bar* MainWindow::getMenuBar()
{
  return _menuBar.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flush the event queue.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_flush()
{
  // Passing TRUE on unix calls XSync instead of XFlush. No effect on Windows.
  this->getApp()->flush ( TRUE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when this window is resized.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onResize ( FX::FXObject *object, FX::FXSelector selector, void *data )
{
  // Make sure the text-window is the proper size.
  if( _hSplitter.first )
    _hSplitter.first->setSplit ( 1, _hSplitter.second );

  // Call the base class's function.
  return BaseClass::onConfigure ( object, selector, data );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when this horizontal splitter is done being dragged.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onHorizSplitterChanged ( FX::FXObject *object, FX::FXSelector selector, void *data )
{
  // Grab the new height.
  _hSplitter.second = _hSplitter.first->getSplit ( 1 );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide/Show Axes.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandAxes ( FX::FXObject *object, FX::FXSelector selector, void *data )
{
  bool show ( data ? true : false );

  Usul::App::Controller::instance().axesShow ( show );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the axes button.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateAxes ( FX::FXObject *object, FX::FXSelector selector, void *data )
{
  FoxTools::Functions::enable ( Usul::App::Controller::instance().axesEnable(), object );
  FoxTools::Functions::check ( Usul::App::Controller::instance().axesCheck(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn on/off stereo mode
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandStereo ( FX::FXObject *object, FX::FXSelector, void *data )
{
  Usul::App::Controller::instance().stereoSet ( data ? true : false );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable/Disable stereo button
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateStereo ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::enable ( Usul::App::Controller::instance().stereoEnable(), object );
  FoxTools::Functions::check  ( Usul::App::Controller::instance().stereoCheck(),  object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the stereo mode.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandStereoMode ( FX::FXObject *object, FX::FXSelector, void *data )
{
  FX::FXMenuRadio *radio = SAFE_CAST_FOX ( FX::FXMenuRadio, object );
  if( 0x0 == radio )
    return 0;

  MenuRadio::ValidRefPtr menuRadio ( reinterpret_cast < MenuRadio* > ( radio->getUserData() ) );
  UIntAttribute::ValidRefPtr attribute ( reinterpret_cast < UIntAttribute* > ( menuRadio->userData() ) );

  Usul::App::Controller::instance().stereoModeSet ( attribute->value() );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the stereo mode menu.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateStereoMode ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXMenuRadio *radio = SAFE_CAST_FOX ( FX::FXMenuRadio, object );
  if( 0x0 == radio )
    return 0;

  MenuRadio::ValidRefPtr menuRadio ( reinterpret_cast < MenuRadio* > ( radio->getUserData() ) );
  UIntAttribute::ValidRefPtr attribute ( reinterpret_cast < UIntAttribute* > ( menuRadio->userData() ) );

  FoxTools::Functions::enable( Usul::App::Controller::instance().stereoEnable(), object );
  FoxTools::Functions::check( Usul::App::Controller::instance().stereoCheck( attribute->value() ), object );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the eye distance
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandEyeDistance ( FX::FXObject *object, FX::FXSelector, void *data )
{
  FX::FXMenuRadio *radio = SAFE_CAST_FOX ( FX::FXMenuRadio, object );
  if( 0x0 == radio )
    return 0;

  MenuRadio::ValidRefPtr menuRadio ( reinterpret_cast < MenuRadio* > ( radio->getUserData() ) );
  FloatAttribute::ValidRefPtr attribute ( reinterpret_cast < FloatAttribute* > ( menuRadio->userData() ) );

  Usul::App::Controller::instance().stereoEyeDistanceSet ( attribute->value() );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the eye distance.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateEyeDistance ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXMenuRadio *radio = SAFE_CAST_FOX ( FX::FXMenuRadio, object );
  if( 0x0 == radio )
    return 0;

  MenuRadio::ValidRefPtr menuRadio ( reinterpret_cast < MenuRadio* > ( radio->getUserData() ) );
  FloatAttribute::ValidRefPtr attribute ( reinterpret_cast < FloatAttribute* > ( menuRadio->userData() ) );

  FoxTools::Functions::enable( Usul::App::Controller::instance().stereoEnable(), object );
  FoxTools::Functions::check(  Usul::App::Controller::instance().stereoEyeDistanceCheck ( attribute->value() ), object );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the active view, which may be null.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::IViewer *MainWindow::activeView()
{
  return Usul::App::Controller::instance().activeView();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide/Show Axes.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandLights ( FX::FXObject *object, FX::FXSelector selector, void *data )
{
  Usul::App::Controller::instance().lightsShow ( data ? true : false );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the axes button.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateLights ( FX::FXObject *object, FX::FXSelector selector, void *data )
{
  FoxTools::Functions::enable ( Usul::App::Controller::instance().lightsEnable(), object );
  FoxTools::Functions::check ( Usul::App::Controller::instance().lightingCheck(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the status bar.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_initStatusBar()
{
  // Status bar.
  unsigned int layout ( LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | STATUSBAR_WITH_DRAGCORNER );
  _statusBar = new FX::FXStatusBar ( this, layout );
  _statusBar->getStatusLine()->setTarget ( this );
  _statusBar->getStatusLine()->setSelector ( ID_STATUS_TEXT );

  // Label for memory usage.
  layout = FX::LAYOUT_FILL_Y | FX::LAYOUT_RIGHT | FX::FRAME_SUNKEN;
  _memoryLabel = new FXLabel ( _statusBar, Usul::System::Memory::formatPhysical().c_str(), NULL, layout );

  // Cancel button.
  layout = FX::LAYOUT_RIGHT | FX::FRAME_RAISED | FX::FRAME_THICK;
  _cancelButton = ButtonPtr ( new FX::FXButton ( _statusBar, "Cancel", 0x0, this, MainWindow::ID_CANCEL_BUTTON, layout ) );
  _cancelButton->hide();

  // Progress bar.
  layout = FX::LAYOUT_RIGHT | FX::LAYOUT_FILL_X | FX::FRAME_SUNKEN | FX::FRAME_THICK | FX::PROGRESSBAR_PERCENTAGE;
  _progressBar.first = new FX::FXProgressBar ( _statusBar, 0x0, 0, layout, 0, 0, 120, 0 );
  _progressBar.first->hide();
  _progressBar.second = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene stage state.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onCommandSceneStage ( FX::FXObject *, FX::FXSelector, void *data )
{
  Usul::App::Controller::instance().sceneStage ( data ? true : false );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the scene stage button.
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateSceneStage ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::check ( Usul::App::Controller::instance().sceneStage(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the center of rotation.
//
///////////////////////////////////////////////////////////////////////////////
  
long MainWindow::onCommandCenterOfRotation  ( FX::FXObject *, FX::FXSelector, void *data )
{
  Usul::App::Controller::instance().centerOfRotationSet ( data ? true : false );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the center of rotation checked?
//
///////////////////////////////////////////////////////////////////////////////

long MainWindow::onUpdateCenterOfRotation   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::check ( Usul::App::Controller::instance().centerOfRotationCheck(), object );
  return 1;
}

