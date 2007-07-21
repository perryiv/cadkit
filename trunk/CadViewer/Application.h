
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
#include "CadViewer/Interfaces/IWandState.h"
#include "CadViewer/Interfaces/IJoystick.h"
#include "CadViewer/Functors/Tool.h"
#include "CadViewer/Pick/Intersect.h"

#include "Usul/Pointers/Pointers.h"
#include "Usul/CommandLine/Parser.h"

#include "VRV/Interfaces/IRequestRead.h"
#include "VRV/Interfaces/IButtonCallback.h"
#include "VRV/Interfaces/IMenuCallback.h"

#include "VRV/Core/Application.h"
#include "VRV/Functors/Matrix/MatrixFunctor.h"

#include "MenuKit/OSG/Menu.h"

#include "OsgTools/Grid.h"

#include <string>
#include <vector>
#include <list>
#include <fstream>


namespace CV {

	
// The CadViewer application
class Application : public VRV::Core::Application,
                    public CV::Interfaces::IAuxiliaryScene,
                    public CV::Interfaces::IWandStateFloat,
                    public CV::Interfaces::IJoystickFloat,
                    public VRV::Interfaces::IRequestRead,
                    public VRV::Interfaces::IButtonCallback,
                    public VRV::Interfaces::IMenuCallback
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

  /////////////////////////////////////////////////////////////////////////////
  //
  //  CV::Interfaces::IWandStateFloat
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the wand's position.
  virtual void                  wandPosition ( Usul::Math::Vec3f &p ) const;

  // Get the wand's rotation matrix.
  virtual void                  wandRotation ( Matrix44f &R ) const;

  // Get the wand's matrix.
  virtual void                  wandMatrix ( Matrix44f &M ) const;

  // Get/set the wand's offset.
  virtual void                  wandOffset ( Usul::Math::Vec3f &v ) const;
  virtual void                  wandOffset ( const Usul::Math::Vec3f &v );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  CV::Interfaces::IJoystickFloat
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the joystick value in the range [-1,1].
  virtual float                 joystickHorizontal() const;
  virtual float                 joystickVertical()   const;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  VRV::Interfaces::IRequestRead
  //
  /////////////////////////////////////////////////////////////////////////////

  // Read the model from the named source and position it using the matrix.
  virtual void                  requestRead ( const std::string &source, const Matrix44f &m );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  VRV::Interfaces::IButtonCallback
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set the button callback specified by the string.
  virtual void                  setCallback ( const std::string &id, MenuKit::Button *button );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  VRV::Interfaces::IMenuCallback
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set the menu callback specified by the string.
  virtual void                  setCallback ( const std::string &id, MenuKit::Menu *menu );

  /////////////////////////////////////////////////////////////////////////////

  // Get/set the center of rotation.
  const osg::Vec3 &             rotationCenter() const { return _rotCenter; }
  void                          rotationCenter ( const osg::Vec3 & );

  // Print the usage string.
  static void                   usage ( const std::string &exe, std::ostream &out );

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

  // Is this the head node?
  bool                          _isHeadNode() const;

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
  void                          _initStatusBar();

  // Called once for each display to initialize the OpenGL context.
  virtual void                  contextInit();

  // Handle the events, if any.
  bool                          _handleMenuEvent();
  bool                          _handleIntersectionEvent();
  bool                          _handleNavigationEvent( const unsigned long event_request = 0 );

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

  // Load the file(s).
  void                          _loadRestartFile ( const std::string &filename );

  // Called by the kernel before the frame.
  virtual void                  latePreFrame();
  void                          _latePreFrame();
  
  // Process the button states.
  void                          _processButtons();

  // Process the command queue.
  void                          _processCommands();

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

  // Set the cursor and its matrix functor.
  void                          _setCursor ( unsigned int );
  void                          _setCursorMatrixFunctor ( VRV::Functors::Matrix::MatrixFunctor * );

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
  void                          _updateStatusBar ( const std::string &text );
  void                          _updateSceneTool();

  // Use the scene tool if we are supposed to.
  void                          _useSceneTool();

  // Write the scene to file.
  void                          _writeScene ( const std::string &filename, const osg::Node *node ) const;

  // Button callbacks.
  void                          _defaultCallback  ( MenuKit::Message m, MenuKit::Item *item );
  void                          _hideSelected     ( MenuKit::Message m, MenuKit::Item *item );
  void                          _showAll          ( MenuKit::Message m, MenuKit::Item *item );
  void                          _unselectVisible  ( MenuKit::Message m, MenuKit::Item *item );
  void                          _exportSelected   ( MenuKit::Message m, MenuKit::Item *item );
  void                          _exportWorld      ( MenuKit::Message m, MenuKit::Item *item );
  void                          _exportScene      ( MenuKit::Message m, MenuKit::Item *item );
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
  void                          _resetClipping    ( MenuKit::Message m, MenuKit::Item *item );
  void                          _hvTransWandXZ    ( MenuKit::Message m, MenuKit::Item *item );
  void                          _hvTransGlobalXZ  ( MenuKit::Message m, MenuKit::Item *item );
  void                          _poleNav          ( MenuKit::Message m, MenuKit::Item *item );
  void                          _gotoViewFront    ( MenuKit::Message m, MenuKit::Item *item );
  void                          _gotoViewBack     ( MenuKit::Message m, MenuKit::Item *item );
  void                          _gotoViewTop      ( MenuKit::Message m, MenuKit::Item *item );
  void                          _gotoViewBottom   ( MenuKit::Message m, MenuKit::Item *item );
  void                          _gotoViewRight    ( MenuKit::Message m, MenuKit::Item *item );
  void                          _gotoViewLeft     ( MenuKit::Message m, MenuKit::Item *item );
  void                          _rotateWorld      ( MenuKit::Message m, MenuKit::Item *item );
  void                          _dropToFloor      ( MenuKit::Message m, MenuKit::Item *item );
  void                          _increaseSpeed    ( MenuKit::Message m, MenuKit::Item *item );
  void                          _decreaseSpeed    ( MenuKit::Message m, MenuKit::Item *item );
  void                          _increaseSpeedTen ( MenuKit::Message m, MenuKit::Item *item );
  void                          _decreaseSpeedTen ( MenuKit::Message m, MenuKit::Item *item );
  void                          _exportImage      ( MenuKit::Message m, MenuKit::Item *item );
  void                          _renderPassesOne      ( MenuKit::Message m, MenuKit::Item *item );
  void                          _renderPassesThree    ( MenuKit::Message m, MenuKit::Item *item );
  void                          _renderPassesNine     ( MenuKit::Message m, MenuKit::Item *item );
  void                          _renderPassesTweleve  ( MenuKit::Message m, MenuKit::Item *item );
  

  // For readability.
  typedef unsigned long                                 ThreadId;
  typedef std::auto_ptr<Parser>                         ParserPtr;
  typedef osg::ref_ptr<osg::MatrixTransform>            MatTransPtr;
  typedef osg::ref_ptr<osg::Group>                      GroupPtr;
  typedef osg::ref_ptr<osg::Node>                       NodePtr;
  typedef osg::ref_ptr<osg::Projection>                 ProjectPtr;
  typedef VRV::Functors::BaseFunctor::RefPtr            FunctorPtr;
  typedef std::auto_ptr<OsgTools::Text>                 TextPtr;
  typedef VRV::Functors::Matrix::MatrixFunctor::RefPtr  MatrixFunctorPtr;
  typedef Interfaces::IVisibility::QueryPtr             IVisibilityPtr;
  typedef Interfaces::ISelection::QueryPtr              ISelectionPtr;
  typedef Interfaces::IMaterialStack::QueryPtr          IMaterialStackPtr;
  typedef USUL_VALID_REF_POINTER(MenuKit::OSG::Menu)    MenuPtr;
  typedef std::map<std::string,MenuKit::Callback::Ptr>  ButtonMap;
  typedef std::map<std::string,osg::Vec4>               ColorMap;

  // Data members.
  static ThreadId   _appThread;
  static ThreadId   _mainThread;
  ParserPtr         _parser;
  MatTransPtr       _gridBranch;
  MatTransPtr       _cursor;
  MatTransPtr       _menuBranch;
  MatTransPtr       _statusBranch;
  GroupPtr          _origin;
  GroupPtr          _auxiliary;
  ProjectPtr        _textBranch;
  FunctorPtr        _navigatorH;
  FunctorPtr        _navigatorV;
  FunctorPtr        _sceneTool;
  FunctorPtr        _intersector;
  osg::Vec2         _analogTrim;
  osg::Vec3         _rotCenter;
  TextPtr           _pickText;
  TextPtr           _navText;
  TextPtr           _frameText;
  TextPtr           _msgText;
  unsigned int      _flags;
  Usul::Math::Vec3f _wandOffset;
  MatrixFunctorPtr  _cursorMatrix;
  IVisibilityPtr    _iVisibility;
  ISelectionPtr     _iSelection;
  IMaterialStackPtr _iMaterialStack;
  MenuPtr           _menu;
  MenuPtr           _statusBar;
  ButtonMap         _buttonMap;
  osg::Matrixf      _home;
  ColorMap          _colorMap;
  std::vector<OsgTools::Grid> _gridFunctors;
  bool              _textures;
};


}; // namespace CV


#endif // _CV_APPLICATION_CLASS_H_
