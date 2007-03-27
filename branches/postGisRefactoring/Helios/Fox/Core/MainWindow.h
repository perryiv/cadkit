
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

#ifndef _OPEN_SCENE_GRAPH_FOX_BINDING_MAIN_WINDOW_H_
#define _OPEN_SCENE_GRAPH_FOX_BINDING_MAIN_WINDOW_H_

#include "Helios/Fox/Core/Export.h"

#include "Usul/Errors/Stack.h"
#include "Usul/Components/Manager.h"

#include "Usul/Interfaces/IClassFactory.h"
#include "Usul/Interfaces/INotify.h"
#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/IActiveDocument.h"
#include "Usul/Interfaces/IShadeModel.h"
#include "Usul/Interfaces/IPolygonMode.h"
#include "Usul/Interfaces/IDocumentSelect.h"

#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/ICancelButton.h"
#include "Usul/Interfaces/GUI/IFlushEvents.h"
#include "Usul/Interfaces/GUI/ISaveFileDialog.h"
#include "Usul/Interfaces/GUI/ILoadFileDialog.h"
#include "Usul/Interfaces/GUI/IReportErrors.h"
#include "Usul/Interfaces/GUI/IPreferencesManager.h"
#include "Usul/Interfaces/GUI/IUpdateTextWindow.h"
#include "Usul/Interfaces/GUI/IQuestion.h"

#include "Usul/Interfaces/Fox/IFoxMenuPane.h"
#include "Usul/Interfaces/Fox/IFoxTabBook.h"
#include "Usul/Interfaces/Fox/IFoxTabItem.h"
#include "Usul/Interfaces/Fox/IMenuBar.h"

#include "FoxTools/Headers/MainWindow.h"
#include "FoxTools/Headers/StatusBar.h"
#include "FoxTools/Headers/Label.h"
#include "FoxTools/Headers/Splitter.h"
#include "FoxTools/Headers/ProgressBar.h"
#include "FoxTools/Headers/RecentFiles.h"
#include "FoxTools/Headers/DockSite.h"
#include "FoxTools/Headers/GLCanvas.h"
#include "FoxTools/Menu/Bar.h"
#include "FoxTools/Menu/RecentFiles.h"

#include <string>
#include <map>
#include <list>
#include <set>
#include <vector>

namespace FX
{
  class FXButton; 
  class FXToolBarShell;
  class FXText;
  class FXTreeItem;
  class FXTabBook;
  class FXTabItem;
  class FXTreeList;
  class FXWindow;
}


namespace Usul 
{ 
  namespace Interfaces { struct IViewer; } 
  namespace Documents  { class Document; }
}

namespace Helios {
namespace Core {

class OSG_FOX_CORE_EXPORT MainWindow : public FX::FXMainWindow,
                                       public Usul::Interfaces::INotifyString,
                                       public Usul::Interfaces::IProgressBar,
                                       public Usul::Interfaces::IStatusBar,
                                       public Usul::Interfaces::IActiveView,
                                       public Usul::Interfaces::ISaveFileDialog,
                                       public Usul::Interfaces::ILoadFileDialog,
                                       public Usul::Interfaces::ICancelButton,
                                       public Usul::Interfaces::IReportErrors,
                                       public Usul::Interfaces::IFoxTabItem,
                                       public Usul::Interfaces::IPreferencesManager,
                                       public Usul::Interfaces::IFlushEvents,
                                       public Usul::Interfaces::IUpdateTextWindow,
                                       public Usul::Interfaces::IFoxTabBook,
                                       public Usul::Interfaces::IActiveDocument,
                                       public Usul::Interfaces::IQuestion,
                                       public Usul::Interfaces::IMenuBar,
                                       public Usul::Interfaces::IDocumentSelect
{
public:

  // Useful typedefs.
  typedef FX::FXMainWindow                      BaseClass;
  typedef Usul::Errors::Stack                   ErrorStack;
  typedef std::pair<std::string,std::string>    Filter;
  typedef std::vector<Filter>                   Filters;
  typedef Usul::Interfaces::ILoadFileDialog     ILoadFileDialog;
  typedef Usul::Interfaces::ISaveFileDialog     ISaveFileDialog;
  typedef ILoadFileDialog::Filenames            Filenames;
  typedef Usul::Interfaces::IUnknown            IUnknown;
  typedef Usul::Components::Manager::UnknownSet PluginSet;
  typedef PluginSet::iterator                   PluginItr;
  typedef PluginSet::const_iterator             ConstPluginItr;
  typedef Usul::Components::Manager             PluginManager;
  typedef Usul::Documents::Document             Document;
  typedef FoxTools::Menu::Bar                   MenuBar;
  typedef Usul::Interfaces::IViewer             IViewer;
  typedef Usul::Interfaces::IPolygonMode        IPolygonMode;
  typedef Usul::Interfaces::IShadeModel         IShadeModel;
  

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Message ids.
  enum 
  {
    ID_ABOUT_DIALOG = BaseClass::ID_LAST,
    ID_DOCUMENT_NEW,
    ID_DOCUMENT_OPEN,
    ID_DOCUMENT_INSERT,
    ID_DOCUMENT_SAVE,
    ID_DOCUMENT_SAVE_AS,
    ID_DOCUMENT_EXPORT,
    ID_RECENT_FILE,
    ID_EXIT,
    ID_CAMERA_FIT,
    ID_CAMERA_RESET,
    ID_CAMERA_FRONT,
    ID_CAMERA_BACK,
    ID_CAMERA_LEFT,
    ID_CAMERA_RIGHT,
    ID_CAMERA_TOP,
    ID_CAMERA_BOTTOM,
    ID_POLYGONS_FILLED,
    ID_POLYGONS_WIRE,
    ID_POLYGONS_HIDDEN,
    ID_POLYGONS_POINTS,
    ID_POLYGONS_AS_IS,
    ID_SHADING_SMOOTH,
    ID_SHADING_FLAT,
    ID_SHADING_AS_IS,
    ID_EDIT_BACKGROUND,
    ID_DEFAULT_BACKGROUND,
    ID_EDIT_FLIP_NORMALS,
    ID_STATUS_TEXT,
    ID_PREFERENCES,
    ID_LOAD_ANIMATION,
    ID_SAVE_ANIMATION,
    ID_TOOL_COMMAND,
    ID_CANCEL_BUTTON,
    ID_MODE_NAVIGATION,
    ID_MODE_PICK,
    ID_BOUNDING_BOX,
    ID_BOUNDING_SPHERE,
    ID_TOOLBAR_TOGGLE,
    ID_FULL_SCREEN,
    ID_NEW_WINDOW,
    ID_LIGHTING,
    ID_TWO_SIDED_LIGHTING,
    ID_TEXT_WINDOW,
    ID_DISPLAY_LISTS,
    ID_ALLOW_SPINNING,
    ID_LOW_LODS,
    ID_HIGH_LODS,
    ID_HORIZONTAL_SPLITTER,
    ID_AXES,
    ID_STEREO,
    ID_STEREO_MODE,
    ID_EYE_DISTANCE,
    ID_LIGHTS,
    ID_SCENE_STAGE,
    ID_CENTEROFROTATION,
    ID_LAST
  };

  // Get this pointer as an unknown
  Usul::Interfaces::IUnknown*           asUnknown();


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IFoxTabItem
  //
  /////////////////////////////////////////////////////////////////////////////

  // Add/remove/clear/get the tabs.
  void                                  addTab    ( const std::string &name );
  void                                  deleteTab ( const std::string &name );
  void                                  deleteTabs();
  FX::FXTabItem *                       tabItem   ( const std::string &name );
  const FX::FXTabItem *                 tabItem   ( const std::string &name ) const;

  void                                  tabWindow ( const std::string &name, FX::FXWindow *window );
  FX::FXWindow *                        tabWindow ( const std::string &name );
  const FX::FXWindow *                  tabWindow ( const std::string &name ) const;

  // Return the active view, which may be null.
  virtual IViewer *                     activeView();

  // Create the window.
  virtual void                          create();

  // Create the default GUI.
  virtual void                          createDefaultGUI();

  // Is it created?
  bool                                  isCreated() const;

  // Load any command line files.
  void                                  loadCommandLineFiles();

  // Messages handlers.
  long                                  onMotion                   ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onKeyPress                 ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onResize                   ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onHorizSplitterChanged     ( FX::FXObject *, FX::FXSelector, void * );

  // Command messages.
  long                                  onCommandDocumentNew       ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandDocumentOpen      ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandDocumentInsert    ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandDocumentSave      ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandDocumentSaveAs    ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandExport            ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandRecentFile        ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandClose             ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandRestore           ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandAbout             ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandExit              ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandDropFile          ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandTool              ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandCancel            ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandBoundingBox       ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandBoundingSphere    ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandToolbarToggle     ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandFullScreen        ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandFlipNormals       ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandNewWindow         ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandAllowSpinning     ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandDisplayLists      ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandTwoSidedLighting  ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandLighting          ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandModeNavigation    ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandModePick          ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandPreferences       ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandEditBackground    ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandDefaultBackground ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandCameraFit         ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandCameraReset       ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandCameraFront       ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandCameraBack        ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandCameraLeft        ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandCameraRight       ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandCameraTop         ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandCameraBottom      ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandPolygonsFilled    ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandPolygonsWire      ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandPolygonsHidden    ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandPolygonsPoints    ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandPolygonsAsIs      ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandShadingSmooth     ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandShadingFlat       ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandShadingAsIs       ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandLowLods           ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandHighLods          ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandAxes              ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandStereo            ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandStereoMode        ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandEyeDistance       ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandLights            ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandSceneStage        ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onCommandCenterOfRotation  ( FX::FXObject *, FX::FXSelector, void * );

  // Update messages.
  long                                  onUpdateNotBusy            ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateStatusBar          ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateTool               ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateBoundingBox        ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateBoundingSphere     ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateToolbarToggle      ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateFullScreen         ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateNeedingModified    ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateFlipNormals        ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateTextWindow         ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateAllowSpinning      ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateNeedingDocument    ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateDisplayLists       ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateTwoSidedLighting   ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateLighting           ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdatePick               ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateNavigation         ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateNeedingIBackground ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateNeedingICamera     ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdatePolygonsFilled     ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdatePolygonsWire       ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdatePolygonsHidden     ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdatePolygonsPoints     ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdatePolygonsAsIs       ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateShadingSmooth      ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateShadingFlat        ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateShadingAsIs        ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateEditBackground     ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateLowLods            ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateHighLods           ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateAxes               ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateStereo             ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateStereoMode         ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateEyeDistance        ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateLights             ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateSceneStage         ( FX::FXObject *, FX::FXSelector, void * );
  long                                  onUpdateCenterOfRotation   ( FX::FXObject *, FX::FXSelector, void * );

  void                                  outputFile ( const std::string &of ) { _output = of; }
  const std::string &                   outputFile() const { return _output; }

  // Usul::Interfaces::IReportErrors
  void                                  reportErrors ( unsigned int options, bool clear );

  // Get the status bar
  const FX::FXStatusBar *               statusBar() const { return _statusBar; }
  FX::FXStatusBar *                     statusBar()       { return _statusBar; }

  // Update the model label based on current mode
  void                                  updateModeLabel();

  // Get the menu bar.
  const MenuBar &                       menuBar() const { return *_menuBar; }
  MenuBar &                             menuBar()       { return *_menuBar; }

  // Get the splitters.
  const FX::FXSplitter *                splitterHorizontal() const { return _hSplitter.first; }
  FX::FXSplitter *                      splitterHorizontal()       { return _hSplitter.first; }
  const FX::FXSplitter *                splitterVertical()   const { return _vSplitter.first; }
  FX::FXSplitter *                      splitterVertical()         { return _vSplitter.first; }

  /// Get the active document
  virtual Document*                     activeDocument();

protected:

  MainWindow ( 
    FX::FXApp *app,
    const FX::FXString &name,
    FX::FXIcon *icon = 0x0,
    FX::FXIcon *mi = 0x0,
    FX::FXuint options = FX::DECOR_ALL );
  MainWindow();
  virtual ~MainWindow();

  void                                  _addCommand  ( FoxTools::Menu::Group *pane, Usul::Interfaces::IUnknown *unknown );
  template < class MenuType > void      _addMenuType ( FoxTools::Menu::Group *pane, Usul::Interfaces::IUnknown *unknown );

  virtual bool                          _exiting();

  void                                  _flush();
  virtual void                          _fullScreenStart();
  virtual void                          _fullScreenEnd();
  virtual void                          _fullScreenToggle();

  void                                  _hideTabWindow();
  void                                  _showTabWindow();

  void                                  _hideTextWindow();
  void                                  _showTextWindow();

  void                                  _initStatusBar();
  void                                  _initFileMenu();
  void                                  _initEditMenu();
  void                                  _initViewMenu();
  void                                  _initPluginMenus();
  void                                  _initPluginMenu( FoxTools::Menu::Group *menuPane, const PluginSet& plugins );
  void                                  _initPluginToolbar();
  void                                  _initToolbars();
  void                                  _initStandardToolbar();
  void                                  _initViewToolbar();
  void                                  _initModeToolbar();
  void                                  _initToolsMenu();
  void                                  _initHelpMenu();
  void                                  _initTabControl();

  virtual void                          _openDocument ( const std::string &filename );
  virtual void                          _openDocuments ( const Filenames &names );
  virtual void                          _onCommandOpen();
  virtual void                          _onCommandExport();
  void                                  _onCommandCamera      ( unsigned int option );
  void                                  _onCommandPolygonMode ( IPolygonMode::Mode );
  void                                  _onCommandShadeModel  ( IShadeModel::Mode );
  void                                  _onUpdatePolygonMode  ( FX::FXObject *sender, IPolygonMode::Mode );

  virtual void                          _writeToRegistry() const;

  ///  Usul::Interfaces::IActiveView
  virtual Usul::Interfaces::IUnknown*   getActiveView();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::INotifyString
  //
  /////////////////////////////////////////////////////////////////////////////
  
  /// Send the message.
  virtual void                          notify ( Usul::Interfaces::IUnknown *caller, const std::string &message );

  /// Usul::Interfaces::IFoxTabBook
  virtual FX::FXTabBook *       tabBook  (  );
  virtual const FX::FXTabBook * tabBook  (  ) const;

  /// Usul::Interfaces::IActiveDocument
  virtual Usul::Interfaces::IUnknown* getActiveDocument();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IProgressBar
  //
  /////////////////////////////////////////////////////////////////////////////
  
  // Show the progress bar.
  virtual void                          showProgressBar();

  // Set the total of progress bar.
  virtual void                          setTotalProgressBar ( unsigned int value );

  // Update the progress bar.
  virtual void                          updateProgressBar ( unsigned int value );

  // Hide the progress bar.
  virtual void                          hideProgressBar();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IStatusBar
  //
  /////////////////////////////////////////////////////////////////////////////
  
  // Get the text of the status bar.
  virtual const std::string &           getStatusBarText ( ) { return _statusText; }

  // Set the status bar text.
  virtual void                          setStatusBarText ( const std::string &text, bool force );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::ISaveFileDialog
  //
  /////////////////////////////////////////////////////////////////////////////
  
  virtual ISaveFileDialog::FileResult   getSaveFileName ( const std::string &title = "Save", const Filters &filters = Filters() );
  virtual ISaveFileDialog::FilesResult  getSaveFileNames ( const std::string &title = "Save", const Filters &filters = Filters() );
  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::ILoadFileDialog
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual ILoadFileDialog::FileResult   getLoadFileName  ( const std::string &title = "Load", const Filters &filters = Filters() );
  virtual ILoadFileDialog::FilesResult  getLoadFileNames ( const std::string &title = "Load", const Filters &filters = Filters() );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::ICancelButton
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void                          showCancelButton();
  virtual void                          hideCancelButton();

  ///  Usul::Interfaces::IPreferenceManager
  virtual Usul::Interfaces::IUnknown*   getPreferences ( );

  ///  Usul::Interfaces::IFlushEvents
  virtual void                          flushEventQueue();

  ///  Usul::Interfaces::IUpdateTextWindow
  virtual void                          updateTextWindow ( bool force );

  /// Usul::Interfaces::IQuestion
  virtual std::string                   question ( const std::string &buttons,  const std::string &title, const std::string &text );

  /// Usul::Interfaces::IMenuBar
  virtual FoxTools::Menu::Bar*          getMenuBar ();

  /// Usul::Interfaces::IDocumentSelect
  virtual Usul::Documents::Document*            selectDocument ( const Documents& );

private:

  typedef std::map < std::string, FX::FXWindow * >        Windows;
  typedef std::auto_ptr < FX::FXToolBar >                 ToolBarPtr;
  typedef std::pair < FX::FXProgressBar *, unsigned int > ProgressBar;
  typedef std::auto_ptr < FX::FXButton >                  ButtonPtr;
  typedef FoxTools::Menu::RecentFiles                     RecentFiles;
  typedef RecentFiles::ValidAccessRefPtr                  RecentFilesPtr;

  typedef std::pair < FX::FXTabItem *, FX::FXWindow * >   TabItem;
  typedef std::map < std::string, TabItem >               TabItems;
  typedef MenuBar::ValidAccessRefPtr                      MenuBarPtr;
  typedef std::pair < FX::FXSplitter *, unsigned int >    Splitter;

  MenuBarPtr            _menuBar;
  FX::FXStatusBar *     _statusBar;
  FX::FXLabel *         _memoryLabel;
  ProgressBar           _progressBar;
  ButtonPtr             _cancelButton;
  RecentFilesPtr        _recentFiles;
  std::string           _statusText;
  Splitter              _hSplitter;
  Splitter              _vSplitter;
  FX::FXTabBook *       _tabBook;
  TabItems              _tabItems;
  unsigned long         _refCount;
  bool                  _fullScreen;
  bool                  _wasMaximized;
  std::string           _output;
  FX::FXText *          _textWindow;

  FXDECLARE_ABSTRACT ( MainWindow );
};

} // namespace Core
} // namespace Helios


#endif //_OPEN_SCENE_GRAPH_FOX_BINDING_MAIN_WINDOW_H_
