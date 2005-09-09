
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
#include "CadViewer/Interfaces/IApplication.h"
#include "CadViewer/Interfaces/IVisibility.h"
#include "CadViewer/Interfaces/IMaterialStack.h"
#include "CadViewer/Interfaces/ISelection.h"
#include "CadViewer/Interfaces/IAuxiliaryScene.h"
#include "CadViewer/Interfaces/INavigationScene.h"
#include "CadViewer/Interfaces/IModelsScene.h"
#include "CadViewer/Interfaces/IFrameInfo.h"
#include "CadViewer/Interfaces/IWorldInfo.h"
#include "CadViewer/Interfaces/IMatrixMultiply.h"
#include "CadViewer/Interfaces/IWandState.h"
#include "CadViewer/Interfaces/IJoystick.h"
#include "CadViewer/Interfaces/IClippingDistance.h"
#include "CadViewer/Functors/MatrixFunctor.h"
#include "CadViewer/Functors/Tool.h"
#include "CadViewer/Pick/Intersect.h"

#include "VRV/Interfaces/IRequestRead.h"
#include "VRV/Interfaces/IButtonCallback.h"
#include "VRV/Interfaces/IMenuCallback.h"
#include "VRV/Prefs/Settings.h"

#include "Collision/Interfaces/ICollider.h"

#include "Usul/Pointers/Pointers.h"

#include "osgVRJ/Application.h"

#include "vrjGA/JoystickDevice.h"

#include "MenuKit/OSG/Menu.h"

#include "OsgTools/Grid.h"

#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>

#if defined (USE_SINTERPOINT)
# include "SinterAppData.h"
# include "sys/time.h"
#endif


namespace CV {

	
class Application : public osgVRJ::Application,
                    public CV::Interfaces::IApplication,
                    public CV::Interfaces::IAuxiliaryScene,
                    public CV::Interfaces::INavigationScene,
                    public CV::Interfaces::IModelsScene,
                    public CV::Interfaces::IFrameInfoFloat,
                    public CV::Interfaces::IWorldInfoFloat,
                    public CV::Interfaces::IMatrixMultiplyFloat,
                    public CV::Interfaces::IWandStateFloat,
                    public CV::Interfaces::IJoystickFloat,
                    public CV::Interfaces::IClippingDistanceFloat,
                    public VRV::Interfaces::IRequestRead,
                    public VRV::Interfaces::IButtonCallback,
                    public VRV::Interfaces::IMenuCallback
{
public:

  // Useful typedefs
  USUL_DECLARE_REF_POINTERS ( Application );
  typedef osgVRJ::Application BaseClass;
  typedef std::list<std::string> Args;
  typedef Usul::Math::Matrix44f Matrix44f;

  // Constructor.
  Application ( Args &args );

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
  //  CV::Interfaces::INavigationScene
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the navigation scene.
  virtual const osg::Group *    navigationScene() const;
  virtual osg::Group *          navigationScene();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  CV::Interfaces::ModelsScene
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the models scene.
  virtual const osg::Group *    modelsScene() const;
  virtual osg::Group *          modelsScene();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  CV::Interfaces::IFrameInfoFloat
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the duration of the last frame in seconds.
  virtual float                 frameTime() const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  CV::Interfaces::IWorldInfoFloat
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the radius of the "world".
  virtual float                 worldRadius() const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  CV::Interfaces::IMatrixMultiplyFloat
  //
  /////////////////////////////////////////////////////////////////////////////

  // Post-multiply the component's matrix by the given matrix.
  virtual void                  postMultiply ( const Matrix44f &M );

  // Pre-multiply the component's matrix by the given matrix.
  virtual void                  preMultiply ( const Matrix44f &M );

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
  //  CV::Interfaces::IClippingDistanceFloat
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Get/set the clipping distances.
  virtual void                  getClippingDistances ( float &nearDist, float &farDist ) const;
  virtual void                  setClippingDistances ( float nearDist, float farDist );

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

  // Joystick callbacks.
  struct JoystickCB : public vrjGA::Callback
  {
    JoystickCB ( Application *app ) : _app ( app ){}
    virtual void operator () ( vrjGA::Message m, vrjGA::Referenced *caller );
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

  // Destructor.
  virtual ~Application();

  // Calculate the frame-rate.
  double                        _calculateFrameRate() const;

  // Generate a string from the integer.
  std::string                   _counter ( unsigned int num ) const;

  // get the color that corresponds to the string
  const osg::Vec4&              _getColor ( const std::string& s ) const;

  // Get the elapsed time since the program started (in seconds).
  double                        _getElapsedTime() const;

  // Get the duration of the last frame in seconds.
  double                        _getFrameTime() const;

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
  bool                          _handleToolEvent();
  bool                          _handleCancelEvent();
  void                          _removeCursorChildren();

  // Called by the kernel to initialize this instance.
  virtual void                  init();
  void                          _init();

  // Intersect if we are supposed to.
  void                          _intersect();

  // Load the file(s).
  void                          _loadModelFile   ( const std::string &filename );
  void                          _loadModelStream ( std::stringstream &filestream );
  void                          _loadRestartFile ( const std::string &filename );
  void                          _loadConfigFiles ( const std::list<std::string> &configs );
  void                          _loadSimConfigs  ( std::string dir );
  void                          _loadSimConfigs();

  // Navigate if we are supposed to.
  void                          _navigate();

    // Get the number of selected
  unsigned int                 _numSelected();

  // Parse the command-line arguments.
  void                          _parseCommandLine();

  // Post-process the model loading.
  void                          _postProcessModelLoad ( const std::string &filename, osg::Node *model );

  // Called by the kernel before the frame.
  virtual void                  preFrame();
  void                          _preFrame();

  // Process the button states.
  void                          _processButtons();

  // Process the command queue.
  void                          _processCommands();

  // Called by the kernel after the frame.
  virtual void                  postFrame();
  void                          _postFrame();

  // Read the model and position it using the matrix.
  void                          _readModel ( const std::string &filename, const Matrix44f &matrix );
  void                          _streamModel ( std::stringstream &filestream, const Matrix44f &matrix );

  // Read the user's preferences.
  void                          _readUserPreferences();

  // Read the near and far clipping plane from file.
  void                          _readNearAndFarClippingPlanes();

  // Seek to the intersected point, if any.
  void                          _seek();

  // Select/unselect the intersected node, if any.
  void                          _select();
  void                          _unselect();

  // Fill the given vector with the selected matrix-transforms.
  void                          _selected ( CV::Functors::Tool::Transforms &vt );

  // Set the cursor and its matrix functor.
  //void                          _setCursor ( unsigned int );
  //void                          _setCursorMatrixFunctor ( CV::Functors::MatrixFunctor * );

  // Set the current "camera" position as "home".
  void                          _setHome();

  // Set the near and far clipping planes based on the scene.
  void                          _setNearAndFarClippingPlanes();

  // Return an unknown pointer to this instance.
  Unknown *                     _thisUnknown();

  // rebuild the grid in case the _gridFunctor's state has been updated
  void                          _rebuildGrid();

  // Update the scene holding text.
  void                          _update ( OsgTools::Text &, const std::string & );

  // Update.
  void                          _updateFrameTime();
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
  void                          _polysScribe      ( MenuKit::Message m, MenuKit::Item *item );
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

  // For readability.
  typedef unsigned long                                 ThreadId;
  typedef Usul::CommandLine::Parser                     Parser;
  typedef std::auto_ptr<Parser>                         ParserPtr;
  typedef osg::ref_ptr<osg::MatrixTransform>            MatTransPtr;
  typedef osg::ref_ptr<osg::Group>                      GroupPtr;
  typedef osg::ref_ptr<osg::Node>                       NodePtr;
  typedef osg::ref_ptr<osg::Projection>                 ProjectPtr;
  typedef CV::Functors::BaseFunctor::RefPtr             FunctorPtr;
  typedef USUL_VALID_REF_POINTER(vrjGA::ButtonGroup)    ButtonsPtr;
  typedef USUL_VALID_REF_POINTER(vrjGA::TrackerDevice)  TrackerPtr;
  typedef USUL_VALID_REF_POINTER(vrjGA::JoystickDevice) JoystickPtr;
  typedef std::auto_ptr<OsgTools::Text>                 TextPtr;
  typedef CV::Functors::MatrixFunctor::RefPtr           MatrixFunctorPtr;
  typedef Interfaces::IVisibility::ValidQueryPtr        IVisibilityPtr;
  typedef Interfaces::ISelection::ValidQueryPtr         ISelectionPtr;
  typedef Interfaces::IMaterialStack::ValidQueryPtr     IMaterialStackPtr;
  typedef Collision::Interfaces::ICollider              Collider;
  typedef Collider::ValidAccessQueryPtr                 ICollider;
  typedef USUL_VALID_REF_POINTER(MenuKit::OSG::Menu)    MenuPtr;
  typedef std::map<std::string,MenuKit::Callback::Ptr>  ButtonMap;
  typedef VRV::Prefs::Settings                          Preferences;
  typedef Preferences::ValidRefPtr                      PrefsPtr;
  typedef std::map<std::string,osg::Vec4>               ColorMap;

  // Data members.
  static ThreadId   _appThread;
  static ThreadId   _mainThread;
  ParserPtr         _parser;
  GroupPtr          _root;
  MatTransPtr       _navBranch;
  MatTransPtr       _models;
  MatTransPtr       _gridBranch;
  MatTransPtr       _cursor;
  MatTransPtr       _cursorActiveWithRot;
  MatTransPtr       _cursorRedWithRot;
  MatTransPtr       _cursorYellowWithRot;
  MatTransPtr       _cursorGreenWithRot;
  MatTransPtr       _cursorBlueWithRot;
  MatTransPtr       _cursorTriggerWithRot;
  MatTransPtr       _cursorActiveNoRot;
  MatTransPtr       _cursorRedNoRot;
  MatTransPtr       _cursorYellowNoRot;
  MatTransPtr       _cursorGreenNoRot;
  MatTransPtr       _cursorBlueNoRot;
  MatTransPtr       _cursorTriggerNoRot;
  NodePtr           _cursor_zoom;
  MatTransPtr       _menuBranch;
  MatTransPtr       _statusBranch;
  GroupPtr          _origin;
  GroupPtr          _auxiliary;
  ProjectPtr        _textBranch;
  FunctorPtr        _navigatorH;
  FunctorPtr        _navigatorV;
  FunctorPtr        _sceneTool;
  FunctorPtr        _intersector;
  ButtonsPtr        _buttons;
  TrackerPtr        _tracker;
  JoystickPtr       _joystick;
  osg::Vec2         _analogTrim;
  osg::Vec3         _rotCenter;
  TextPtr           _pickText;
  TextPtr           _navText;
  TextPtr           _frameText;
  TextPtr           _msgText;
  Usul::Math::Vec4i _vp;
  unsigned int      _flags;
  Usul::Math::Vec3f _wandOffset;
  double            _frameTime;
  MatrixFunctorPtr  _cursorMatrix;
  vpr::Mutex        _sceneMutex;
  IVisibilityPtr    _iVisibility;
  ISelectionPtr     _iSelection;
  IMaterialStackPtr _iMaterialStack;
  ICollider         _iCollider;
  osg::Vec2         _clipDist;
  MenuPtr           _menu;
  MenuPtr           _statusBar;
  ButtonMap         _buttonMap;
  PrefsPtr          _prefs;
  osg::Matrixf      _home;
  ColorMap          _colorMap;
  std::vector<OsgTools::Grid*> _gridFunctors;
  bool              _textures;
  GroupPtr          _scribeBranch;

# if defined (USE_SINTERPOINT)
    // Functions used for networked file loading with SinterPoint, if enabled
    void            _sinterPointInit();
    void            _sinterReceiveModelPreFrame();
    void            _sinterReceiveModelPostFrame();
    void            _sinterReceiveData(int size);

    // SinterPoint variables
    sinter::Receiver                    _sinterReceiver;
    std::stringstream                   _sinterStream;
    cluster::UserData< SinterAppData >  _sinterAppData;
    SinterFileState                     _sinterFileState;

    // Used to time sinterpoint loading
    double _getClockTime(){
      struct timeval t;
      gettimeofday(&t,NULL);
      return((double)t.tv_sec + (double)t.tv_usec * 0.000001);
    }
    double _sinterTime1, _sinterTime2;
# endif
};




}; // namespace CV


#endif // _CV_APPLICATION_CLASS_H_
