
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

#ifndef _CV_APPLICATION_CLASS_H_
#define _CV_APPLICATION_CLASS_H_

#include "CadViewer/Declarations.h"
#include "CadViewer/Interfaces/IVisibility.h"
#include "CadViewer/Interfaces/IMaterialStack.h"
#include "CadViewer/Interfaces/ISelection.h"
#include "CadViewer/Interfaces/IAuxiliaryScene.h"
#include "CadViewer/Functors/Tool.h"
#include "CadViewer/Pick/Intersect.h"

#include "Usul/Pointers/Pointers.h"
#include "Usul/CommandLine/Parser.h"

#include "VRV/Core/Application.h"
#include "VRV/Functors/Matrix/MatrixFunctor.h"

#include "OsgTools/Grid.h"

#include <string>
#include <vector>
#include <list>
#include <fstream>

namespace Usul { namespace Commands { class Command; } }

namespace CV {

	
// The CadViewer application
class Application : public VRV::Core::Application,
                    public CV::Interfaces::IAuxiliaryScene
{
public:

  // Useful typedefs
  typedef VRV::Core::Application BaseClass;
  typedef std::list<std::string> Args;
  typedef Usul::Math::Matrix44f Matrix44f;

  // Constructor.
  Application ( Args &args );

  // Destructor.
  virtual ~Application();

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  CV::Interfaces::IAuxiliaryScene
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the auxiliary scene.
  virtual const osg::Group *    auxiliaryScene() const;
  virtual osg::Group *          auxiliaryScene();

  // Get/set the center of rotation.
  const osg::Vec3 &             rotationCenter() const { return _rotCenter; }
  void                          rotationCenter ( const osg::Vec3 & );

  // Is this the application or main thread?
  static bool                   isAppThread();
  static bool                   isMainThread();

  // Run the program.
  void                          run();

protected:

  // Typedefs needed below.
  typedef Usul::CommandLine::Parser                     Parser;
  typedef Parser::Args                                  ParserArgs;

  // Joystick callbacks.
  struct JoystickCB : public VRV::Devices::Callback
  {
    JoystickCB ( Application *app ) : _app ( app ){}
    virtual void operator () ( VRV::Devices::Message m, Usul::Base::Referenced *caller );
  private:
    Application *_app;
  };

  // The callbacks are friendly.
  friend struct JoystickCB;

  // Typedefs used internally.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Unknown::ValidQueryPtr ValidUnknown;

  // These are not implemented by design.
  Application();
  Application ( const Application &app );
  Application& operator = ( const Application &app );

  // Calculate the frame-rate.
  double                        _calculateFrameRate() const;

  // Generate a string from the integer.
  std::string                   _counter ( unsigned int num ) const;

  // get the color that corresponds to the string
  const osg::Vec4&              _getColor ( const std::string& s ) const;

  // Initialize.
  void                          _initGrid ( osg::Node *node );
  void                          _initLight();
  void                          _initText();
  void                          _initMenu();

  // Initialize the menus.
  void                          _initFileMenu     ( MenuKit::Menu* menu );
  void                          _initEditMenu     ( MenuKit::Menu* menu );
  void                          _initViewMenu     ( MenuKit::Menu* menu );
  void                          _initNavigateMenu ( MenuKit::Menu* menu );
  void                          _initToolsMenu    ( MenuKit::Menu* menu );
  void                          _initOptionsMenu  ( MenuKit::Menu* menu );
  void                          _initAnimateMenu  ( MenuKit::Menu* menu );

  // Create a button.
  MenuKit::Button*              _createButton    ( Usul::Commands::Command* command );
  MenuKit::Button*              _createButton    ( const std::string& name, MenuKit::Callback* );
  MenuKit::Button*              _createRadio     ( const std::string& name, MenuKit::Callback* );
  MenuKit::Button*              _createToggle    ( const std::string& name, MenuKit::Callback* );
  MenuKit::Button*              _createSeperator ( );

  // Handle the events, if any.
  bool                          _handleMenuEvent ( unsigned long id );
  bool                          _handleIntersectionEvent ( unsigned long id );
  bool                          _handleNavigationEvent ( unsigned long id );

  // Called by the kernel to initialize this instance.
  virtual void                  init();
  void                          _init();

  // Intersect if we are supposed to.
  void                          _intersect();

  // Navigate if we are supposed to.
  void                          _navigate();

  // Get the number of selected
  unsigned int                  _numSelected();

  // Parse the command-line arguments.
  void                          _parseCommandLine();

  // Called by the kernel before the frame.
  virtual void                  latePreFrame();
  void                          _latePreFrame();

  // Called by the kernel after the frame.
  virtual void                  postFrame();
  void                          _postFrame();

  // Seek to the intersected point, if any.
  void                          _seek();

  // Select/unselect the intersected node, if any.
  void                          _select();
  void                          _unselect();

  // Fill the given vector with the selected matrix-transforms.
  void                          _selected ( CV::Functors::Tool::Transforms &vt );

  // Set the current "camera" position as "home".
  void                          _setHome();

  // Return an unknown pointer to this instance.
  Unknown *                     _thisUnknown();

  // rebuild the grid in case the _gridFunctor's state has been updated
  void                          _rebuildGrid();

  // Update the scene holding text.
  void                          _update ( OsgTools::Text &, const std::string & );

  // Update.
  void                          _updateFrameRateDisplay();
  void                          _updateAnalogText();
  void                          _updateCursor();
  void                          _updateSceneTool();

  // Use the scene tool if we are supposed to.
  void                          _useSceneTool();

  // Write the scene to file.
  void                          _writeScene ( const std::string &filename, const osg::Node *node ) const;

  /// Update notify.
  virtual void                  _updateNotify ();

  /// The active document has changed.
  virtual void                  activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc );

  /// Called when button is pressed.
  virtual void                  buttonPressNotify ( Usul::Interfaces::IUnknown * );

  /// Called when button is released.
  virtual void                  buttonReleaseNotify ( Usul::Interfaces::IUnknown * );

  // Button callbacks.
  void                          _defaultCallback  ( MenuKit::Message m, MenuKit::Item *item );
  void                          _hideSelected     ( MenuKit::Message m, MenuKit::Item *item );
  void                          _showAll          ( MenuKit::Message m, MenuKit::Item *item );
  void                          _unselectVisible  ( MenuKit::Message m, MenuKit::Item *item );
  void                          _exportSelected   ( MenuKit::Message m, MenuKit::Item *item );
  void                          _exportWorld      ( MenuKit::Message m, MenuKit::Item *item );
  void                          _exportWorldBinary ( MenuKit::Message m, MenuKit::Item *item );
  void                          _exportScene       ( MenuKit::Message m, MenuKit::Item *item );
  void                          _exportSceneBinary ( MenuKit::Message m, MenuKit::Item *item );
  void                          _quitCallback     ( MenuKit::Message m, MenuKit::Item *item );
  void                          _statusBarVis     ( MenuKit::Message m, MenuKit::Item *item );
  void                          _gridVisibility   ( MenuKit::Message m, MenuKit::Item *item );
  void                          _hTransWandPosX   ( MenuKit::Message m, MenuKit::Item *item );
  void                          _hTransWandNegX   ( MenuKit::Message m, MenuKit::Item *item );
  void                          _vTransWandPosY   ( MenuKit::Message m, MenuKit::Item *item );
  void                          _vTransWandPosZ   ( MenuKit::Message m, MenuKit::Item *item );
  void                          _vTransWandNegZ   ( MenuKit::Message m, MenuKit::Item *item );
  void                          _hTransGlobalPosX ( MenuKit::Message m, MenuKit::Item *item );
  void                          _hTransGlobalNegX ( MenuKit::Message m, MenuKit::Item *item );
  void                          _vTransGlobalPosY ( MenuKit::Message m, MenuKit::Item *item );
  void                          _vTransGlobalPosZ ( MenuKit::Message m, MenuKit::Item *item );
  void                          _vTransGlobalNegZ ( MenuKit::Message m, MenuKit::Item *item );
  void                          _hRotWandPosY     ( MenuKit::Message m, MenuKit::Item *item );
  void                          _hRotGlobalPosY   ( MenuKit::Message m, MenuKit::Item *item );
  void                          _vRotWandPosX     ( MenuKit::Message m, MenuKit::Item *item );
  void                          _vRotGlobalPosX   ( MenuKit::Message m, MenuKit::Item *item );
  void                          _vScaleWorld      ( MenuKit::Message m, MenuKit::Item *item );
  void                          _vScaleSelected   ( MenuKit::Message m, MenuKit::Item *item );
  void                          _wMoveSelLocal    ( MenuKit::Message m, MenuKit::Item *item );
  void                          _wMoveTopLocal    ( MenuKit::Message m, MenuKit::Item *item );
  void                          _raySelector      ( MenuKit::Message m, MenuKit::Item *item );
  void                          _normScene        ( MenuKit::Message m, MenuKit::Item *item );
  void                          _normSelected     ( MenuKit::Message m, MenuKit::Item *item );
  void                          _polysSmooth      ( MenuKit::Message m, MenuKit::Item *item );
  void                          _polysFlat        ( MenuKit::Message m, MenuKit::Item *item );
  void                          _polysWireframe   ( MenuKit::Message m, MenuKit::Item *item );
  void                          _polysPoints      ( MenuKit::Message m, MenuKit::Item *item );
  void                          _polysTexture     ( MenuKit::Message m, MenuKit::Item *item );
  void                          _setAnalogTrim    ( MenuKit::Message m, MenuKit::Item *item );
  void                          _viewHome         ( MenuKit::Message m, MenuKit::Item *item );
  void                          _viewWorld        ( MenuKit::Message m, MenuKit::Item *item );
  void                          _viewScene        ( MenuKit::Message m, MenuKit::Item *item );
  void                          _viewSaved        ( MenuKit::Message m, MenuKit::Item *item );
  void                          _setAsHome        ( MenuKit::Message m, MenuKit::Item *item );
  void                          _saveView         ( MenuKit::Message m, MenuKit::Item *item );
  void                          _resizeGrid       ( MenuKit::Message m, MenuKit::Item *item );
  void                          _backgroundColor  ( MenuKit::Message m, MenuKit::Item *item );
  void                          _gridColor        ( MenuKit::Message m, MenuKit::Item *item );
  void                          _selectionColor   ( MenuKit::Message m, MenuKit::Item *item );
  void                          _resetClipping            ( MenuKit::Message m, MenuKit::Item *item );
  void                          _flyMode                  ( MenuKit::Message m, MenuKit::Item *item );
  void                          _walkMode                 ( MenuKit::Message m, MenuKit::Item *item );
  void                          _poleNav                  ( MenuKit::Message m, MenuKit::Item *item );
  void                          _gotoViewFront            ( MenuKit::Message m, MenuKit::Item *item );
  void                          _gotoViewBack             ( MenuKit::Message m, MenuKit::Item *item );
  void                          _gotoViewTop              ( MenuKit::Message m, MenuKit::Item *item );
  void                          _gotoViewBottom           ( MenuKit::Message m, MenuKit::Item *item );
  void                          _gotoViewRight            ( MenuKit::Message m, MenuKit::Item *item );
  void                          _gotoViewLeft             ( MenuKit::Message m, MenuKit::Item *item );
  void                          _rotateWorld              ( MenuKit::Message m, MenuKit::Item *item );
  void                          _dropToFloor              ( MenuKit::Message m, MenuKit::Item *item );
  void                          _increaseSpeed            ( MenuKit::Message m, MenuKit::Item *item );
  void                          _decreaseSpeed            ( MenuKit::Message m, MenuKit::Item *item );
  void                          _increaseSpeedTen         ( MenuKit::Message m, MenuKit::Item *item );
  void                          _decreaseSpeedTen         ( MenuKit::Message m, MenuKit::Item *item );
  void                          _exportImage              ( MenuKit::Message m, MenuKit::Item *item );
  void                          _renderPassesOne          ( MenuKit::Message m, MenuKit::Item *item );
  void                          _renderPassesThree        ( MenuKit::Message m, MenuKit::Item *item );
  void                          _renderPassesNine         ( MenuKit::Message m, MenuKit::Item *item );
  void                          _renderPassesTweleve      ( MenuKit::Message m, MenuKit::Item *item );
  void                          _dumpFrames               ( MenuKit::Message m, MenuKit::Item *item );
  void                          _toggleMenuSceneHideShow  ( MenuKit::Message m, MenuKit::Item *item );
  void                          _appendCurrentCamera      ( MenuKit::Message m, MenuKit::Item *item );
  void                          _toggleStartAnimation     ( MenuKit::Message m, MenuKit::Item *item );
  void                          _clearAnimation           ( MenuKit::Message m, MenuKit::Item *item );
  void                          _animationSteps20         ( MenuKit::Message m, MenuKit::Item *item );
  void                          _animationSteps50         ( MenuKit::Message m, MenuKit::Item *item );
  void                          _animationSteps100        ( MenuKit::Message m, MenuKit::Item *item );
  void                          _animationStepsDouble     ( MenuKit::Message m, MenuKit::Item *item );
  void                          _animationStepsHalf       ( MenuKit::Message m, MenuKit::Item *item );

  // For readability.
  typedef unsigned long                                 ThreadId;
  typedef std::auto_ptr<Parser>                         ParserPtr;
  typedef osg::ref_ptr<osg::MatrixTransform>            MatTransPtr;
  typedef osg::ref_ptr<osg::Group>                      GroupPtr;
  typedef osg::ref_ptr<osg::Node>                       NodePtr;
  typedef osg::ref_ptr<osg::Projection>                 ProjectPtr;
  typedef VRV::Functors::BaseFunctor::RefPtr            FunctorPtr;
  typedef std::auto_ptr<OsgTools::Text>                 TextPtr;
  typedef Interfaces::IVisibility::QueryPtr             IVisibilityPtr;
  typedef Interfaces::ISelection::QueryPtr              ISelectionPtr;
  typedef Interfaces::IMaterialStack::QueryPtr          IMaterialStackPtr;
  typedef std::map<std::string,osg::Vec4>               ColorMap;

  // Data members.
  static ThreadId   _appThread;
  static ThreadId   _mainThread;
  ParserPtr         _parser;
  MatTransPtr       _gridBranch;
  GroupPtr          _auxiliary;
  FunctorPtr        _sceneTool;
  FunctorPtr        _intersector;
  osg::Vec3         _rotCenter;
  TextPtr           _pickText;
  TextPtr           _navText;
  TextPtr           _frameText;
  TextPtr           _msgText;
  unsigned int      _flags;
  IVisibilityPtr    _iVisibility;
  ISelectionPtr     _iSelection;
  IMaterialStackPtr _iMaterialStack;
  osg::Matrixf      _home;
  ColorMap          _colorMap;
  std::vector<OsgTools::Grid> _gridFunctors;
  bool              _textures;
};


}; // namespace CV


#endif // _CV_APPLICATION_CLASS_H_
