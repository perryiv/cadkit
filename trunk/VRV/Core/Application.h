
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The application class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_APPLICATION_CLASS_H_
#define _VIRTUAL_REALITY_VIEWER_APPLICATION_CLASS_H_

#include "VRV/Core/Export.h"
#include "VRV/Core/Declarations.h"
#include "VRV/Core/Mutex.h"
#include "VRV/Core/Preferences.h"
#include "VRV/Core/Sphere.h"
#include "VRV/Interfaces/IModelAdd.h"
#include "VRV/Interfaces/IRequestRead.h"
#include "VRV/Interfaces/IThread.h"
#include "VRV/Interfaces/ICommandList.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/CommandLine/Parser.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Queue.h"
#include "Usul/Threads/Variable.h"
#include "Usul/Interfaces/ICommand.h"

#include "JSAL/Interfaces/IApplication.h"
#include "JSAL/Interfaces/IInit.h"
#include "JSAL/Interfaces/IContextInit.h"
#include "JSAL/Interfaces/IContextPreDraw.h"
#include "JSAL/Interfaces/IPreFrame.h"
#include "JSAL/Interfaces/IPostFrame.h"

#include "SAL/Interfaces/INode.h"
#include "SAL/Interfaces/IGroup.h"
#include "SAL/Interfaces/ITransform.h"

#include <iosfwd>
#include <queue>
#include <map>


namespace VRV {


class VRV_EXPORT Application : public Usul::Base::Referenced,
                               public JSAL::Interfaces::IInit,
                               public JSAL::Interfaces::IContextInit,
                               public JSAL::Interfaces::IContextPreDraw,
                               public JSAL::Interfaces::IPreFrame,
                               public JSAL::Interfaces::IPostFrame,
                               public VRV::Interfaces::IModelAdd,
                               public VRV::Interfaces::IRequestRead
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::CommandLine::Parser::Args Args;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Application );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Constructor.
  Application ( Args &args );

  // Is this the application or main thread?
  static bool               isAppThread();
  static bool               isMainThread();

  // Run the program.
  virtual void              run();

  // Print the usage string.
  static void               usage ( const std::string &exe, std::ostream &out );

protected:

  typedef VRV::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Math::Matrix44f Matrix;
  typedef Unknown::ValidQueryPtr ValidUnknown;
  typedef SAL::Interfaces::INode INode;
  typedef SAL::Interfaces::IGroup IGroup;
  typedef SAL::Interfaces::ITransformFloat ITransform;
  typedef INode::ValidQueryPtr ValidNode;
  typedef IGroup::ValidQueryPtr ValidGroup;
  typedef IGroup::ValidAccessQueryPtr ValidAccessGroup;
  typedef ITransform::ValidQueryPtr ValidTransform;
  typedef ITransform::ValidAccessQueryPtr ValidAccessTransform;
  typedef std::queue<VRV::Interfaces::IThread::ValidQueryPtr> JobContainer;
  USUL_DECLARE_QUEUE_CONFIG ( JobQueueConfig, Mutex, Guard, JobContainer );
  typedef Usul::Threads::Queue < JobQueueConfig > JobQueue;
  typedef Usul::Math::Vec2f Vec2f;
  typedef Usul::Math::Vec3f Vec3f;
  typedef Usul::Interfaces::ICommand ICommand;
  typedef std::map < unsigned long, ICommand::ValidRefPtr > ButtonCommandMap;

  // Grouping of command mappings.
  struct Commands
  {
    typedef std::map < unsigned long, ICommand::ValidRefPtr > Map;
    Map pressed, down, released;
  };

  // No default construction or copying.
  Application();
  Application ( const Application &app );
  Application& operator = ( const Application &app );

  // Destructor.
  virtual ~Application();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  JSAL::Interfaces::IInit
  //
  /////////////////////////////////////////////////////////////////////////////

  // Called when the program is initializing.
  virtual void              init();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  JSAL::Interfaces::IContextInit
  //
  /////////////////////////////////////////////////////////////////////////////

  // Called when the context is initializing.
  virtual void              contextInit();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  JSAL::Interfaces::IContextPreDraw
  //
  /////////////////////////////////////////////////////////////////////////////

  // Called once for each display every frame.
  virtual void              contextPreDraw();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  JSAL::Interfaces::IPreFrame
  //
  /////////////////////////////////////////////////////////////////////////////

  // Called before the frame is drawn.
  virtual void              preFrame();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  JSAL::Interfaces::IPostFrame
  //
  /////////////////////////////////////////////////////////////////////////////

  // Called after the frame is drawn.
  virtual void              postFrame();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  VRV::Interfaces::IModelAdd
  //
  /////////////////////////////////////////////////////////////////////////////

  // Add the new model.
  virtual void              addModel ( Unknown *model, const Matrix &m, const std::string &filename );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  VRV::Interfaces::IRequestRead
  //
  /////////////////////////////////////////////////////////////////////////////

  // Request the component to read the model from the named source and 
  // position it using the matrix.
  virtual void              requestRead ( const std::string &source, const Matrix &m );

  /////////////////////////////////////////////////////////////////////////////

  // Add a job to the queue.
  void                      _addJob ( Unknown *model );

  // Add the new model.
  void                      _addModel ( Unknown *model );

  // Check the job queue.
  void                      _checkJobs();

  //  Check the group of recently loaded models.
  void                      _checkRecentModelReads();

  // Called when the context is initializing.
  void                      _contextInit();

  // Called once for each display every frame.
  void                      _contextPreDraw();

  // Called when program is initializing.
  void                      _init();

  // Initialize.
  void                      _initGrid ( INode *node );
  void                      _initLight();

  // Load all button commands.
  void                      _loadButtonCommands();
  void                      _loadButtonCommands ( VRV::Interfaces::ICommandList *cl );

  // Load the files.
  void                      _loadConfigFiles ( const Args &configs );
  void                      _loadModelFile   ( const std::string &filename );
  void                      _loadRestartFile ( const std::string &filename );

  // Make a sphere.
  INode *                   _makeSphere ( const Vec3f &center, float radius ) const;

  // Parse the command-line arguments.
  void                      _parseCommandLine();

  // Called before/after the frame is drawn.
  void                      _preFrame();
  void                      _postFrame();

  // Process the button state.
  void                      _processButtons();


  // Read the user's preferences.
  void                      _readUserPreferences();

  // Quit the program.
  virtual void              _quit();

  // Set the near and far clipping planes based on the scene.
  void                      _setNearAndFarClippingPlanes();

  // Get the bounding sphere, or a default one.
  Spheref                   _validBoundingSphere ( INode *node ) const;

  // Move the transform node so that we can see it.
  void                      _viewAll ( ITransform *moveMe );

  // Write the node to a scene-graph file.
  void                      _writeNodeFile ( const std::string &filename, INode *node );

private:

  static unsigned long _appThread;
  static unsigned long _mainThread;
  Usul::CommandLine::Parser _parser;
  JSAL::Interfaces::IApplication::ValidQueryPtr _app;
  ValidAccessGroup _root;
  ValidAccessTransform _navBranch;
  ValidAccessTransform _models;
  ValidAccessTransform _grid;
  ValidAccessTransform _cursor;
  ValidAccessGroup _auxiliary;
  Usul::Threads::Variable<ValidAccessGroup> _loaded;
  USUL_VALID_REF_POINTER(vrjGA::ButtonGroup) _buttons;
  USUL_VALID_REF_POINTER(vrjGA::TrackerDevice) _tracker;
  USUL_VALID_REF_POINTER(vrjGA::JoystickDevice) _joystick;
  Vec2f _analogTrim;
  std::string _exception;
  JobQueue _jobQueue;
  Preferences _prefs;
  Commands _commands;
};


}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_APPLICATION_CLASS_H_
