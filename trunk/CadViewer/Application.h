
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

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Functors/Interaction/Matrix/MatrixFunctor.h"

#include "VRV/Core/Application.h"

#include "OsgTools/Grid.h"

#include "MenuKit/MemFunCallback.h"

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
  typedef Usul::Math::Matrix44f Matrix44f;

  typedef void (Application::*VoidFunction) ();
  typedef void (Application::*BoolFunction) ( bool );
  typedef bool (Application::*CheckFunction) () const;
  typedef Usul::Adaptors::MemberFunction < Application*, VoidFunction >   ExecuteFunctor;
  typedef MenuKit::MemFunCallbackReturn < Application*, CheckFunction >   CheckFunctor;
  typedef Usul::Adaptors::MemberFunction < Application*, BoolFunction >   BoolFunctor;
  typedef MenuKit::BasicCommand < ExecuteFunctor >                        BasicCommand;
  typedef MenuKit::CheckCommand < BoolFunctor, CheckFunctor >             CheckCommand;

  // Constructor.
  Application ( );

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

  // Quit the program
  void                          quit ();

  // Export functions.
  void                          exportWorld ();
  void                          exportWorldBinary ();
  void                          exportScene ();
  void                          exportSceneBinary ();

  // View functions.
  void                          viewWorld ();
  void                          viewScene ();
protected:
  
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
  Application ( const Application &app );
  Application& operator = ( const Application &app );

  // Calculate the frame-rate.
  double                        _calculateFrameRate() const;

  // Generate a string from the integer.
  std::string                   _counter ( unsigned int num ) const;

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

  void                          _increaseSpeed ();
  void                          _decreaseSpeed ();
  void                          _increaseSpeedTen ();
  void                          _decreaseSpeedTen ();

  void                          _animationSteps20 ( bool );
  bool                          _animationSteps20 () const;

  void                          _animationSteps50 ( bool );
  bool                          _animationSteps50 () const;

  void                          _animationSteps100 ( bool );
  bool                          _animationSteps100 () const;

  void                          _animationStepsDouble ( );
  void                          _animationStepsHalf ( );

  // For readability.
  typedef unsigned long                                             ThreadId;
  typedef osg::ref_ptr<osg::MatrixTransform>                        MatTransPtr;
  typedef osg::ref_ptr<osg::Group>                                  GroupPtr;
  typedef osg::ref_ptr<osg::Node>                                   NodePtr;
  typedef osg::ref_ptr<osg::Projection>                             ProjectPtr;
  typedef Usul::Functors::Interaction::Common::BaseFunctor::RefPtr  FunctorPtr;
  typedef std::auto_ptr<OsgTools::Text>                             TextPtr;
  typedef Interfaces::IVisibility::QueryPtr                         IVisibilityPtr;
  typedef Interfaces::ISelection::QueryPtr                          ISelectionPtr;
  typedef Interfaces::IMaterialStack::QueryPtr                      IMaterialStackPtr;
  typedef std::map<std::string, Usul::Math::Vec4f >                ColorMap;

  // Data members.
  static ThreadId   _appThread;
  static ThreadId   _mainThread;
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
  ColorMap          _colorMap;
  std::vector<OsgTools::Grid> _gridFunctors;
  bool              _textures;
};


}; // namespace CV


#endif // _CV_APPLICATION_CLASS_H_
