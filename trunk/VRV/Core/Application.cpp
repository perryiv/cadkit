
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

#include "Precompiled.h"
#include "Application.h"
#include "ErrorChecker.h"
#include "Exceptions.h"
#include "Components.h"
#include "ReadModel.h"

#include "VRV/Common/Buttons.h"
#include "VRV/Interfaces/IParseRestart.h"
#include "VRV/Interfaces/IRequestRead.h"
#include "VRV/Interfaces/IButtonDown.h"
#include "VRV/Interfaces/IButtonPress.h"
#include "VRV/Interfaces/IButtonRelease.h"

#include "JSAL/Interfaces/IChild.h"
#include "JSAL/Interfaces/IQuit.h"
#include "JSAL/Interfaces/IViewer.h"
#include "JSAL/Interfaces/ILight.h"

#include "SAL/Interfaces/IOptimize.h"
#include "SAL/Interfaces/IGrid.h"
#include "SAL/Interfaces/ISphere.h"
#include "SAL/Interfaces/IBoundingSphere.h"
#include "SAL/Interfaces/IColorRGBA.h"
#include "SAL/Interfaces/IBuilder.h"

#include "vrjGA/ButtonGroup.h"
#include "vrjGA/TrackerDevice.h"
#include "vrjGA/JoystickDevice.h"

#include "Usul/Components/Object.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Interfaces/IWrite.h"
#include "Usul/Threads/ThreadId.h"

#include "vrj/Kernel/Kernel.h"
#include "vrj/Display/Projection.h"

using namespace VRV;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Application , Application::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Static variables.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Application::_appThread  = 0;
unsigned long Application::_mainThread = 0;


///////////////////////////////////////////////////////////////////////////////
//
//  Macros to shorten up the lines in the constructor initializer list.
//
///////////////////////////////////////////////////////////////////////////////

#define NAME_SAL  VRV::SCENE_ABSTRACTION_LAYER
#define NAME_JSAL VRV::JUGGLER_SCENE_ABSTRACTION_LAYER
#define NEW_COMPONENT(parent_namespace,interface_type,component_name) \
  Usul::Components::Object::create \
    ( parent_namespace::Interfaces::interface_type::IID, component_name )


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application ( Args &args ) : BaseClass(),
  _parser     ( args.begin(), args.end() ),
  _app        ( NEW_COMPONENT ( JSAL, IApplication, NAME_JSAL ) ),
  _root       (),
  _navBranch  (),
  _models     (),
  _grid       (),
  _cursor     (),
  _auxiliary  (),
  _loaded     (),
  _buttons    ( new vrjGA::ButtonGroup ),
  _tracker    ( new vrjGA::TrackerDevice  ( "VJWand" ) ),
  _joystick   ( new vrjGA::JoystickDevice ( "VJAnalog0", "VJAnalog1" ) ),
  _analogTrim ( 0, 0 ),
  _exception  (),
  _jobQueue   (),
  _prefs      (),
  _commands   ()
{
  ErrorChecker ( 2108398036u, 0 == _mainThread );
  ErrorChecker ( 3254298964u, 0 == _appThread );

  // Initialize the main thread.
  _mainThread = Usul::Threads::currentThreadId();

  // Tell the contained app that we are the parent.
  // Note: do not use "ValidUnknown" because we cannot ref/unref this 
  // instance yet, we are still in the constructor!
  JSAL::Interfaces::IChild::ValidQueryPtr child ( _app );
  child->parent ( this->queryInterface ( Unknown::IID ) );

  // Have to load the config files now. Remove them from the arguments.
  Args configs = _parser.files ( ".config", true );
  this->_loadConfigFiles ( configs );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
  ErrorChecker ( 2962435017u, isMainThread() );

#ifdef _DEBUG
#if 0
  this->_writeNodeFile ( "pgv_root.osg", ValidNode ( _root ) );
#endif
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to help wrapping code that may throw. Use this in threads other than 
//  the main thread because exceptions cannot throw across thread barriers.
//  We print because sometimes the error doesn't get re-thrown later.
//
///////////////////////////////////////////////////////////////////////////////

#define HANDLE_EXCEPTIONS \
  catch ( const std::exception &e ) \
  { \
    _exception = e.what(); \
    std::cout << "Error: 1466870775, Exception caught.\n\t" << _exception << std::endl; \
    this->_quit(); \
  } \
  catch ( ... ) \
  { \
    _exception = "Unknown exception"; \
    std::cout << "Error: 1921480338, Exception caught.\n\t" << _exception << std::endl; \
    this->_quit(); \
  }


///////////////////////////////////////////////////////////////////////////////
//
//  Called during startup shortly after construction.
//
///////////////////////////////////////////////////////////////////////////////

void Application::init()
{
  // Call the other one.
  try { this->_init(); }
  HANDLE_EXCEPTIONS;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called during startup shortly after construction.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_init()
{
  ErrorChecker ( 3643430558u, 0 == _appThread );
  ErrorChecker ( 3621569099u, !isMainThread() );

  // Initialize the application thread.
  _appThread = Usul::Threads::currentThreadId();

  // Allocate the scene.
  _root       = NEW_COMPONENT (  SAL, IGroup,           NAME_SAL  );
  _navBranch  = NEW_COMPONENT (  SAL, ITransformFloat,  NAME_SAL  );
  _models     = NEW_COMPONENT (  SAL, ITransformFloat,  NAME_SAL  );
  _grid       = NEW_COMPONENT (  SAL, ITransformFloat,  NAME_SAL  );
  _cursor     = NEW_COMPONENT (  SAL, ITransformFloat,  NAME_SAL  );
  _auxiliary  = NEW_COMPONENT (  SAL, IGroup,           NAME_SAL  );
  _loaded     = NEW_COMPONENT (  SAL, IGroup,           NAME_SAL  );

  // Name the branches.
  ValidNode ( _root           )->setNodeName ( "_root"      );
  ValidNode ( _navBranch      )->setNodeName ( "_navBranch" );
  ValidNode ( _models         )->setNodeName ( "_models"    );
  ValidNode ( _grid           )->setNodeName ( "_grid"      );
  ValidNode ( _cursor         )->setNodeName ( "_cursor"    );
  ValidNode ( _auxiliary      )->setNodeName ( "_auxiliary" );
  ValidNode ( _loaded.value() )->setNodeName ( "_loaded"    );

  // Hook up the branches.
  ValidGroup nav  ( _navBranch );
  _root->addChild ( ValidNode ( _cursor )     );
  _root->addChild ( ValidNode ( _auxiliary )  );
  _root->addChild ( ValidNode ( _navBranch )  );
  nav->addChild   ( ValidNode ( _models )     );
  nav->addChild   ( ValidNode ( _grid )       );

  // Read the user's preference file, if any.
  this->_readUserPreferences();

  // Initialize the button group by adding the individual buttons.
  _buttons->add ( new vrjGA::ButtonDevice ( VRV::BUTTON0, "VJButton0" ) );
  _buttons->add ( new vrjGA::ButtonDevice ( VRV::BUTTON1, "VJButton1" ) );
  _buttons->add ( new vrjGA::ButtonDevice ( VRV::BUTTON2, "VJButton2" ) );
  _buttons->add ( new vrjGA::ButtonDevice ( VRV::BUTTON3, "VJButton3" ) );
  _buttons->add ( new vrjGA::ButtonDevice ( VRV::BUTTON4, "VJButton4" ) );
  _buttons->add ( new vrjGA::ButtonDevice ( VRV::BUTTON5, "VJButton5" ) );

  // Get the viewer.
  typedef JSAL::Interfaces::IViewer Viewer;
  Viewer::ValidQueryPtr viewer ( _app );

  // Turn on the lights.
  this->_initLight();

  // Add the scene to the viewer.
  viewer->scene ( ValidNode ( _root ) );

  // Parse the command-line arguments.
  this->_parseCommandLine();

  // Initialize the grid to a default size.
  this->_initGrid ( 0x0 );

  // Set the near and far clipping planes.
  this->_setNearAndFarClippingPlanes();

  // Load the button-commands.
  this->_loadButtonCommands();

  // Set a default cursor-matrix functor.
  //ValidUnknown me ( this );TODO
  //VRV::Functors::MatrixFunctor::ValieRefPtr fun ( new VRV::Functors::WandMatrix ( me ) );
  //this->_setCursorMatrixFunctor ( fun );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the grid.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initGrid ( INode *node )
{
  ErrorChecker ( 2968285735u, isAppThread() );

  // Get the bounding sphere, or a default one.
  VRV::Spheref bs = this->_validBoundingSphere ( node );

  // Create a grid component.
  SAL::Interfaces::IGridFloat::ValidQueryPtr grid 
    ( Usul::Components::Object::create 
      ( SAL::Interfaces::IGridFloat::IID, VRV::SCENE_ABSTRACTION_LAYER, true, true ) );

  // Set the properties.
  grid->setNumGridBlocks ( _prefs.numGridBlocks() );
  grid->setGridSize ( _prefs.gridScale() * bs.radius() );

  // Set the color.
  SAL::Interfaces::IColorFloatRGBA::ValidQueryPtr colorSetter ( grid );
  colorSetter->setColorRGBA ( _prefs.gridColor() );

  // Move the center so that it is below the bounding sphere of the node.
  bs.center()[1] = - bs.radius();
  grid->setGridCenter ( bs.center() );

  // Build the scene.
  SAL::Interfaces::IBuilder::ValidQueryPtr builder ( grid );
  ValidNode scene ( builder->buildScene() );

  // Change the scene.
  ValidGroup group ( _grid );
  group->removeAllChildren();
  group->addChild ( scene );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the light.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initLight()
{
  ErrorChecker ( 1079420049u, isAppThread() );

  typedef JSAL::Interfaces::ILight ILight;
  ILight::ValidQueryPtr light ( _app );
  light->setAmbientLightColor  ( _prefs.ambientLightColor() );
  light->setDiffuseLightColor  ( _prefs.diffuseLightColor() );
  light->setSpecularLightColor ( _prefs.specularLightColor() );
  light->setLightDirection     ( _prefs.lightDirection() );
  light->setLightState         ( ILight::ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the context is initializing.
//
///////////////////////////////////////////////////////////////////////////////

void Application::contextInit()
{
  // Call the other one.
  try { this->_contextInit(); }
  HANDLE_EXCEPTIONS;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the context is initializing.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_contextInit()
{
  ErrorChecker ( 4271808199u, !isAppThread() );
  ErrorChecker ( 1016076546u, !isMainThread() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called once for each display every frame.
//
///////////////////////////////////////////////////////////////////////////////

void Application::contextPreDraw()
{
  // Call the other one.
  try { this->_contextPreDraw(); }
  HANDLE_EXCEPTIONS;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called once for each display every frame.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_contextPreDraw()
{
  ErrorChecker ( 1796034783u, !isAppThread() );
  ErrorChecker ( 1703234052u, !isMainThread() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the frame is drawn.
//
///////////////////////////////////////////////////////////////////////////////

void Application::preFrame()
{
  // Call the other one.
  try { this->_preFrame(); }
  HANDLE_EXCEPTIONS;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the frame is drawn.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_preFrame()
{
  ErrorChecker ( 1778744131u, isAppThread() );

  // Update these input devices.
  _buttons->update();
  _tracker->update();
  _joystick->update();

  // Check the job queue.
  this->_checkJobs();

  // Check the recently-loaded group.
  this->_checkRecentModelReads();

  // Check the button-press commands.
  this->_processButtons();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after the frame is drawn.
//
///////////////////////////////////////////////////////////////////////////////

void Application::postFrame()
{
  // Call the other one.
  try { this->_postFrame(); }
  HANDLE_EXCEPTIONS;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after the frame is drawn.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_postFrame()
{
  ErrorChecker ( 3087873397u, isAppThread() );
  // Do nothing.
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
  case JSAL::Interfaces::IInit::IID:
    return static_cast<JSAL::Interfaces::IInit*>(this);
  case JSAL::Interfaces::IContextInit::IID:
    return static_cast<JSAL::Interfaces::IContextInit*>(this);
  case JSAL::Interfaces::IContextPreDraw::IID:
    return static_cast<JSAL::Interfaces::IContextPreDraw*>(this);
  case JSAL::Interfaces::IPreFrame::IID:
    return static_cast<JSAL::Interfaces::IPreFrame*>(this);
  case JSAL::Interfaces::IPostFrame::IID:
    return static_cast<JSAL::Interfaces::IPostFrame*>(this);
  case VRV::Interfaces::IModelAdd::IID:
    return static_cast<VRV::Interfaces::IModelAdd*>(this);
  case VRV::Interfaces::IRequestRead::IID:
    return static_cast<VRV::Interfaces::IRequestRead*>(this);
  case Usul::Interfaces::IUnknown::IID:
    return static_cast<Usul::Interfaces::IUnknown*>(static_cast<JSAL::Interfaces::IInit*>(this));
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this the application thread? The application thread is the one that 
//  calls init(), preFrame() and postFrame().
//
///////////////////////////////////////////////////////////////////////////////

bool Application::isAppThread()
{
  ErrorChecker ( 3054429253u, 0 != _appThread );
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
  ErrorChecker ( 1447897510u, 0 != _mainThread );
  return ( Usul::Threads::currentThreadId() == _mainThread );
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

void Application::_loadConfigFiles ( const Args &configs )
{
  ErrorChecker ( 3961372383u, isMainThread() );

  // Make sure there is at least one config file. Do not use the ErrorChecker.
  if ( configs.empty() )
    Usul::Exceptions::Thrower < VRV::Exceptions::UserInput > 
      ( "Must supply at least one VRJuggler config file" );

  // Load the config files. Note: it crashes with VC7 if we pass the string.
  for ( Args::const_iterator i = configs.begin(); i != configs.end(); ++i )
    vrj::Kernel::instance()->loadConfigFile ( i->c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Quit the program.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_quit()
{
  // Tell the kernel to stop the current application.
  vrj::Kernel::instance()->stop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the program.
//
///////////////////////////////////////////////////////////////////////////////

void Application::run()
{
  ErrorChecker ( 1941372826u, isMainThread() );

  // Start the kernel.
  vrj::Kernel::instance()->start();

  // Wait for events.
  vrj::Kernel::instance()->waitForKernelStop();

  // Rethrow the exception if there is one.
  if ( !_exception.empty() )
    Usul::Exceptions::Thrower < std::runtime_error > temp ( _exception );

  // Try to notify the child-app that the program is quiting.
  JSAL::Interfaces::IQuit::QueryPtr notify ( _app );
  if ( notify.valid() )
    notify->quit();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the command-line arguments.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_parseCommandLine()
{
  ErrorChecker ( 3726322352u, isAppThread() );

  // Extract the restart-files and remove them from the remaining arguments.
  Args restart = _parser.files ( ".cv", true );

  // Load the restart files.
  std::for_each ( restart.begin(), restart.end(), 
                  Usul::Adaptors::memberFunction 
                  ( this, &Application::_loadRestartFile ) );

  // Extract the model files and remove them from the remaining arguments.
  Args models = _parser.files ( true );

  // Load the model files.
  std::for_each ( models.begin(), models.end(), 
                  Usul::Adaptors::memberFunction 
                  ( this, &Application::_loadModelFile ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the restart file.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_loadRestartFile ( const std::string &filename )
{
  ErrorChecker ( 2948739140u, isAppThread() );

  // The reader type.
  typedef VRV::Interfaces::IParseRestart Reader;

  // Declare a restart-file reader.
  Reader::ValidQueryPtr reader 
    ( Usul::Components::Object::create
      ( Reader::IID, VRV::RESTART_FILE_PARSER, true, true ) );

  // Read the file.
  reader->parseRestartFile ( filename, ValidUnknown ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the model.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_loadModelFile ( const std::string &filename )
{
  ErrorChecker ( 2646715545u, isAppThread() );

  // Need an identity matrix.
  Matrix matrix;
  matrix.identity();

  // Append the request.
  this->requestRead ( filename, matrix );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append a request to read the model from the named source and position 
//  it using the matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Application::requestRead ( const std::string &source, const Matrix &matrix )
{
  ErrorChecker ( 3789570022u, isAppThread() );

  // Make a new thread for reading the model.
  VRV::Interfaces::IThread::ValidQueryPtr job 
    ( new VRV::Threads::ReadModel ( source, matrix, ValidUnknown ( this ) ) );

  // Add the job to the queue.
  this->_addJob ( job );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the new model. This function should be reentrant.
//
///////////////////////////////////////////////////////////////////////////////

void Application::addModel ( Unknown *unknown, const Matrix &matrix, const std::string &filename )
{
  // Get the interfaces we need.
  ValidNode model ( unknown );

  // Try to remove redundant top-level groups.
  typedef SAL::Interfaces::IOptimize IOptimize;
  IOptimize::QueryPtr optimizer ( Usul::Components::Object::create
    ( IOptimize::IID, VRV::SCENE_ABSTRACTION_LAYER, false, true ) );
  if ( optimizer.valid() )
    optimizer->removeRedundantGroups ( unknown, false );

  // Set its name to the filename if there is no name.
  std::string name;
  model->getNodeName ( name );
  if ( name.empty() )
    model->setNodeName ( filename );

  // Make a transform for this model.
  ValidTransform mt ( Usul::Components::Object::create
    ( SAL::Interfaces::ITransformFloat::IID, VRV::SCENE_ABSTRACTION_LAYER ) );

  // Set its matrix.
  mt->setTransformMatrix ( matrix );

  // Set its name.
  ValidNode ( mt )->setNodeName ( std::string ( "Branch for: " ) + filename );

  // Hook things up.
  ValidGroup ( mt )->addChild ( model );

  // Add the transform to the branch that holds recently loaded models.
  this->_addModel ( mt );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the new model.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_addModel ( Unknown *unknown )
{
  Guard guard ( _loaded.mutex() );
  _loaded.value()->addChild ( ValidNode ( unknown ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the job to the queue.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_addJob ( Unknown *unknown )
{
  // It is a guarded queue.
  _jobQueue.push ( unknown );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the job queue.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_checkJobs()
{
  ErrorChecker ( 4149683678u, isAppThread() );

  // If the queue is empty then just return.
  if ( _jobQueue.empty() )
    return;

  // Get the current job.
  VRV::Interfaces::IThread::ValidRefPtr job ( _jobQueue.front() );

  // If the job is running then just return.
  if ( job->running() )
    return;

  // See if the job is done.
  if ( job->done() )
  {
    // Pop the job.
    _jobQueue.pop();

    // Let the job decide what it should do.
    job->report();

    // All done.
    return;
  }

  // Start the job.
  job->start();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the group of recently loaded models.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_checkRecentModelReads()
{
  ErrorChecker ( 1921677499u, isAppThread() );

  // Lock the loaded-group.
  Guard guard ( _loaded.mutex() );

  // If the group is empty then just return.
  if ( 0 == _loaded.value()->numChildren() )
    return;

  // Needed below.
  const unsigned long numChildren ( _loaded.value()->numChildren() );
  ValidGroup models ( _models );

  // Add the children to the models-group.
  for ( unsigned long i = 0; i < numChildren; ++i )
  {
    ValidNode child ( _loaded.value()->childNode ( i ) );
    models->addChild ( child );
  }

  // Remove the children from the loaded-group.
  _loaded.value()->removeAllChildren();

  // Move the models to where we can see them all.
  this->_viewAll ( _models );

  // Reset the grid.
  this->_initGrid ( ValidNode ( models ) );

  // Set the near and far clipping planes.
  this->_setNearAndFarClippingPlanes();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the user's preference file, if any.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_readUserPreferences()
{
  ErrorChecker ( 2291479604u, isAppThread() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the near and far clipping planes based on the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_setNearAndFarClippingPlanes()
{
  ErrorChecker ( 3756350826u, isAppThread() );

  // Get the bounding sphere of the scene.
  VRV::Spheref bs = this->_validBoundingSphere ( ValidNode ( _root ) );

  // Calculate the clipping plane distances.
  float zNear ( Usul::Math::abs ( _prefs.nearClippingDistance() ) );
  float zFar  ( Usul::Math::abs ( 4 * bs.radius() ) );

  // Make sure the far-distance is grester than the near. 
  // This can happen if the radius is zero.
  if ( zNear >= zFar )
  {
    float increase ( 1 );
    std::ostringstream message;
    message << "Warning: resetting distance to far clipping plane from " 
            << zFar << " to " << zFar + increase;
    WarningChecker ( 3599916163u, false, message.str() );
    zFar = zNear + increase;
  }

  // Set the distances.
  vrj::Projection::setNearFar ( zNear, zFar );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move the transform node so that we can see it.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_viewAll ( ITransform *moveMe )
{
  ErrorChecker ( 1593413031u, isAppThread() );

  // Get the bounding-sphere from the correct node.
  VRV::Spheref bs = this->_validBoundingSphere ( ValidNode ( moveMe ) );
  const Vec3f &center = bs.center();

  // Figure out the matrix.
  float z ( _prefs.viewAllScaleZ() * bs.radius() + center[2] );
  Matrix translation;
  translation.makeTranslation ( Vec3f ( -center[0], -center[1], -z ) );

  // Post-multiply the current matrix.
  ValidTransform transform ( moveMe );
  Matrix current;
  transform->getTransformMatrix ( current );
  transform->setTransformMatrix ( current * translation );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding sphere, or a default one.
//
///////////////////////////////////////////////////////////////////////////////

VRV::Spheref Application::_validBoundingSphere ( INode *node ) const
{
  ErrorChecker ( 2925349099u, isAppThread() );

  // Initialize.
  Vec3f center ( 0, 0, 0 );
  float radius ( 1 );

  // If we have a valid node...
  if ( node )
  {
    // Get the bounding sphere of the node.
    SAL::Interfaces::IBoundingSphereFloat::ValidQueryPtr bs ( node );
    bs->boundingSphereCenter ( center );
    bs->boundingSphereRadius ( radius );

    // Handle zero-sized bounding spheres.
    if ( radius <= 1e-6 )
      radius = 1;
  }

  // Return the bounding sphere.
  return VRV::Spheref ( center, radius );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a sphere.
//
///////////////////////////////////////////////////////////////////////////////

SAL::Interfaces::INode *Application::_makeSphere ( const Vec3f &center, float radius ) const
{
  ErrorChecker ( 2847865872u, isAppThread() );

  // Create the sphere.
  typedef SAL::Interfaces::ISphereFloat Sphere;
  Sphere::ValidQueryPtr sphere ( Usul::Components::Object::create
    ( Sphere::IID, VRV::SCENE_ABSTRACTION_LAYER, true, true ) );

  // Set its properties.
  sphere->setSphereCenter ( center );
  sphere->setSphereRadius ( radius );

  // Build the sphere.
  SAL::Interfaces::IBuilder::ValidQueryPtr builder ( sphere );
  ValidNode scene ( builder->buildScene() );

  // Return the scene.
  return scene.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the node to a scene-graph file.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_writeNodeFile ( const std::string &filename, INode *node )
{
  ErrorChecker ( 2389321010u, isAppThread() );

  Usul::Interfaces::IWrite::QueryPtr writer
    ( Usul::Components::Object::create 
      ( Usul::Interfaces::IWrite::IID, VRV::SCENE_ABSTRACTION_LAYER, false, false ) );
  if ( writer.valid() )
    writer->write ( filename, ValidUnknown ( node ), 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the button state.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_processButtons()
{
  ErrorChecker ( 2385500932u, isAppThread() );

  // See if there is a command for the combination of buttons that are down.
  Commands::Map::iterator i = _commands.down.find ( _buttons->down() );
  if ( _commands.down.end() != i )
    i->second->execute ( ValidUnknown ( this ) );

  // Now check the buttons that were just pressed.
  i = _commands.pressed.find ( _buttons->pressed() );
  if ( _commands.pressed.end() != i )
    i->second->execute ( ValidUnknown ( this ) );

  // Now check the buttons that were just released.
  i = _commands.released.find ( _buttons->released() );
  if ( _commands.released.end() != i )
    i->second->execute ( ValidUnknown ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load all button commands.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_loadButtonCommands()
{
  ErrorChecker ( 1950330762u, isAppThread() );

  // To shorten the lines.
  typedef VRV::Interfaces::ICommandList ICommandList;
  typedef Usul::Components::Object::UnknownList UnknownList;

  // See if we have any appropriate plugins.
  UnknownList unknowns;
  Usul::Components::Object::create ( ICommandList::IID, unknowns );

  // Loop through the found components.
  for ( UnknownList::const_iterator i = unknowns.begin(); i != unknowns.end(); ++i )
  {
    // Get the command.
    Usul::Interfaces::IUnknown::ValidRefPtr unknown ( *i );

    // Load the command
    this->_loadButtonCommands ( ICommandList::ValidQueryPtr ( unknown ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load all button commands.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_loadButtonCommands ( VRV::Interfaces::ICommandList *cl )
{
  ErrorChecker ( 1105481961u, isAppThread() );

  // To shorten the lines.
  typedef VRV::Interfaces::ICommandList ICommandList;
  typedef VRV::Interfaces::IButtonDown IButtonDown;
  typedef VRV::Interfaces::IButtonPress IButtonPress;
  typedef VRV::Interfaces::IButtonRelease IButtonRelease;
  typedef Commands::Map::value_type MappedPair;

  // Loop through the list of commands.
  ICommandList::ValidRefPtr commands ( cl );
  unsigned long num ( commands->getNumCommands() );
  for ( unsigned long i = 0; i < num; ++i )
  {
    // Get the command.
    ICommand::ValidRefPtr command ( commands->getCommand ( i ) );

    // See if it is a button-down command.
    IButtonDown::QueryPtr down ( command );
    if ( down.valid() )
      _commands.down.insert ( MappedPair ( down->getButtonId(), command ) );

    // See if it is a button-press command.
    IButtonPress::QueryPtr pressed ( command );
    if ( pressed.valid() )
      _commands.pressed.insert ( MappedPair ( pressed->getButtonId(), command ) );

    // See if it is a button-release command.
    IButtonRelease::QueryPtr released ( command );
    if ( released.valid() )
      _commands.released.insert ( MappedPair ( released->getButtonId(), command ) );
  }
}
