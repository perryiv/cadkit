
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "MinervaVR.h"

#include "Usul/System/Host.h"
#include "Usul/System/Sleep.h"
#include "Usul/File/Path.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Jobs/Manager.h"

#include "OsgTools/Builders/Compass.h"

#include "VrjCore/OssimInteraction.h"

#include <fstream>

#include <osg/Group>
#include <osg/Node>

#include "osgDB/WriteFile"

#if _MSC_VER
#include <direct.h>
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Constant keys for options..
//
///////////////////////////////////////////////////////////////////////////////

const std::string USERNAME      = "username";
const std::string PASSWORD      = "password";
const std::string HOST          = "host";
const std::string DATABASE      = "database";
const std::string SESSION       = "session";
const std::string KWL           = "kwl";
const std::string LEGEND_NODE   = "legend_node";
const std::string HOME          = "home";
const std::string HOME_LOOK     = "home_look";
const std::string BACKGROUND    = "background";
const std::string EPHEMERIS     = "ephemeris";

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaVR::MinervaVR( vrj::Kernel* kern, int& argc, char** argv ) : 
  OsgVJApp( kern, argc, argv ),
  _dbManager ( 0x0 ),
  _sceneManager ( new Minerva::Core::Scene::SceneManager ),
  _planet ( new Magrathea::Planet ),
  _options(),
  _background( 0.0, 0.0, 0.0, 1.0 ),
  _update( ),
  _updateThread ( 0x0 ),
  _mutex(),
  _rand ( 0, 5 ),
  _commandQueue ()
{
  _dbManager = new Minerva::Core::Viz::Controller ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );

  // Initialize random numbers
  ::srand ( ::time ( 0x0 ) );

  // Get the config file.
  std::ostringstream file;
  file << Usul::CommandLine::Arguments::instance().directory() << "/Minerva.config";
  std::ifstream fin ( file.str().c_str() );

  if( fin.is_open() )
  {
    fin >> _options;
  }
  else
  {
    std::cerr << " [MinervaVR] Warning: No Minerva config file found. " << std::endl;
  }

  // Make room for 2 Jobs.
  Usul::Jobs::Manager::instance().poolResize ( 2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaVR::~MinervaVR()
{
  // Kill the update thread.
  if( _updateThread.valid() )
    _updateThread->kill();

  // Clear the thread pool
  Usul::Jobs::Manager::destroy();

  // Wait until all threads are done.
  Usul::Threads::Manager::instance().wait();
}


///////////////////////////////////////////////////////////////////////////////
//
//  One time initialization. 
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::appInit()
{
  std::cerr << " [MinervaVR] app init starts: " << std::endl;

  // Initalize clock.
  ::clock();

  // Set all the properties here
  setDevice(ALL, OFF);	

  //vpr::GUID guid ( "6D4B401D-C3FD-429e-84DB-1F9DCC9A2A5C" );
  //_update.init( guid, "viz1" );

  Minerva::Core::DB::Connection::RefPtr applicationConnection ( new Minerva::Core::DB::Connection );
  applicationConnection->username( _options.value( USERNAME ) );
  applicationConnection->password( _options.value( PASSWORD ) );
  applicationConnection->database( _options.value( DATABASE ) );
  applicationConnection->hostname( _options.value( HOST     ) );
  applicationConnection->connect();
  applicationConnection->activate();

  _dbManager->applicationConnection( applicationConnection.get() );

  std::string session ( _options.value ( SESSION ) );
  _dbManager->connectToSession( session );
  _dbManager->sceneManager ( _sceneManager.get() );

  std::cerr << " [MinervaVR] Connected to session: " << session << std::endl;

  if( _options.option ( BACKGROUND ) )
  {
    std::string background ( _options.value ( BACKGROUND ) );
    std::istringstream in ( background );
    in >> _background[0] >> _background[1] >> _background[2] >> _background[3];
  }

  setBackgroundColor( gmtl::Vec4f( _background[0], _background[1], _background[2], _background[3] ).getData());

  vrj::Projection::setNearFar(0.000001, 15.0);
  setSceneInitialPosition( osg::Vec3f( 0.0, 10.0, 0.0 ) );

  this->_initLegend();

  // Launch the update thread.
  this->_launchUpdateThread();
  
  std::cerr << " [MinervaVR] app init ends: " << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the legend.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::_initLegend()
{
  _sceneManager->showLegend ( false );

  if( _options.option( LEGEND_NODE ) )
  {
    std::string legendNode ( _options.value ( LEGEND_NODE ) );
    
    if( Usul::System::Host::name() == legendNode )
    {
      _sceneManager->showLegend ( true );
      _sceneManager->legendWidth ( 0.75 );
      _sceneManager->legendPadding ( osg::Vec2 ( 20.0, 40.0 ) );
      _sceneManager->legendHeightPerItem ( 60 );
      _sceneManager->legendPosition( Minerva::Core::Scene::SceneManager::LEGEND_TOP_LEFT );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a thread to poll database for updating.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::_launchUpdateThread()
{
  // Create a new thread to update in.
  _updateThread = Usul::Threads::Manager::instance().create();
  
  typedef void (MinervaVR::*Function) ( Usul::Threads::Thread *s );
  typedef Usul::Adaptors::MemberFunction < MinervaVR*, Function > MemFun;

  _updateThread->started ( Usul::Threads::newFunctionCallback( MemFun ( this, &MinervaVR::_updateScene ) ) );
  _updateThread->start();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the frame is started. 
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::preFrame()
{
  /// Check to see if we have data.
  //if( _update.isLocal() )
  //  _update->dataAvailable ( _dbManager->hasEvents() );

  BaseClass::preFrame();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Drawing is about to happen. 
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::appPreOsgDraw()
{
  Guard guard ( this->mutex() );
  static bool sizeSet ( false );

  if( false == sizeSet )
  {
    GLint viewport[4];

    ::glGetIntegerv( GL_VIEWPORT, viewport );

    _sceneManager->resize ( viewport[2], viewport[3] );
    
    sizeSet = true;
  }

  // Purge any threads that may be finished.
  Usul::Threads::Manager::instance().purge();

  // Launch the update thread if it stopped.
  if( _updateThread->isIdle() )
  {
    this->_launchUpdateThread();
  }

  this->_buildScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw.  Guard with a mutex.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::draw()
{
  Guard guard ( this->mutex() );
  BaseClass::draw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scenegraph will be build here.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::appSceneInit()
{	
  std::cerr << " [MinervaVR] Scene init starts: " << std::endl;

  _planet->init();

  std::cerr << " [MinervaVR] planet initialized: " << std::endl;

  mOsgDatabasePager = _planet->databasePager();
  _planet->root()->addChild( _sceneManager->root() );

  std::cerr << " [MinervaVR] begin reading kwl: " << std::endl;

  
  if( _options.option ( KWL ) )
  {
    std::string kwl ( _options.value ( KWL ) );
    std::string dir ( Usul::File::directory ( kwl, false ) );

    //::chdir ( dir.c_str() );
    _planet->readKWL( kwl.c_str() );
  }
  
  std::cerr << " [MinveraVR] done reading kwl: " << std::endl;
 
  if( _options.option ( EPHEMERIS ) )
  {
    std::string e ( _options.value ( EPHEMERIS ) );
    std::istringstream in ( e );
    bool useEphemeris ( false );
    in >> useEphemeris;
    _planet->ephemerisFlag( useEphemeris );
  }

  mModelGroupNode->addChild ( _planet->root() );

  
  
  std::cerr << " [MinervaVR] Root set: " << std::endl;

  // Create and set-up the interactor.
  VrjCore::OssimInteraction *interactor = new VrjCore::OssimInteraction();

  
  if( "viz0" == Usul::System::Host::name() )
    mModelGroupNode->addChild ( interactor->compass() );

  if( ossimPlanet* planet = dynamic_cast< ossimPlanet* >( _planet->root() ) )
  {
    interactor->planet( planet );

    if( _options.option ( HOME ) )
    {
      std::string home ( _options.value ( HOME ) );
      std::istringstream in ( home );
      float lat ( 0 ), lon( 0 ), height ( 0 );
      in >> lat >> lon >> height;
      interactor->setHomePosition ( lat, lon, height );
    }
    
    if( _options.option ( HOME_LOOK ) )
    {
      std::string home_look ( _options.value ( HOME_LOOK ) );
      std::istringstream in ( home_look );
      float yaw ( 0 ), pitch( 0 ), roll ( 0 );
      in >> yaw >> pitch >> roll;
      interactor->setHomeLook( yaw, pitch, roll );
    }

    setEngine( interactor );
#ifndef _MSC_VER
    interactor->dumpFilename ( Usul::System::Host::name() );
#endif

    interactor->goHome();
  }

  // TODO: This should be done in a environment variable.
  // get the existing high level path list
  osgDB::FilePathList wFilePathList = osgDB::Registry::instance()->getDataFilePathList();

  // add additional paths of interest to the path list
  wFilePathList.push_back( "/array/cluster/home/adam/src/bin/fonts" );
    
  // re-assign the expanded path list
  osgDB::Registry::instance()->setDataFilePathList( wFilePathList );
  
  std::cerr << " [MinervaVR] Scene init ends: " << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene.  Do not lock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::_updateScene( Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE;

  // 60 seconds.
  Usul::Policies::TimeBased update ( 6000000 );

  while ( 1 )
  {
    try
    {
      Usul::System::Sleep::seconds ( static_cast < unsigned long > ( _rand () ) );

      // Update the scene.
      _dbManager->updateScene();

      if( update () )
	      std::cerr << " [MinervaVR] Update thread still alive." << std::endl;
    }
    catch ( ... )
    {
      std::cerr << "[MinervaVR] exception caught while trying to update scene." << std::endl;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene..
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::_buildScene()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  try
  {
    if( _sceneManager->dirty() )
    {
      std::cerr << " [MinervaVR] Starting building scene." << std::endl;

      _sceneManager->buildScene();

      std::cerr << " [MinervaVR] Finished building scene." << std::endl;

      _sceneManager->dirty ( false );
    }
  }
  catch ( ... )
  {
    std::cerr << "[MinervaVR] exception caught while trying to build scene." << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a light.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::addSceneLight()
{
  osg::ref_ptr < osg::Light > light ( new osg::Light );
  light->setDiffuse( osg::Vec4 ( 0.8f, 0.8f, 0.8f, 1.0f ) );
  light->setPosition( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
  light->setLightNum ( 0 );

  osg::ref_ptr < osg::LightSource > lightSource ( new osg::LightSource );

  lightSource->setLight ( light.get() );
  lightSource->setReferenceFrame ( osg::LightSource::ABSOLUTE_RF );

  mLightGroup->addChild ( lightSource.get() );

  osg::ref_ptr < osg::StateSet > ss ( mSceneRoot->getOrCreateStateSet() );
  lightSource->setStateSetModes( *ss.get(), osg::StateAttribute::ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Frame has finished.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::postFrame()
{
  USUL_TRACE_SCOPE;

  try
  {
    BaseClass::postFrame();

    this->_processCommands();
  }
  catch ( const std::exception& e )
  {
    std::cerr << "Error 1632458424: Standard exception caught in post frame." << std::endl;
    std::cerr << "Message: " << e.what() << std::endl;
  }
  catch ( ... )
  {
    std::cerr << "Error 2681275507: Unknown exception caught in post frame." << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process commands in the queue.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::_processCommands ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  while ( false == _commandQueue.empty() )
  {
    // Get the first command.
    CommandPtr command ( _commandQueue.front() );

    // Remove it from the list.
    _commandQueue.pop_front():

    // Execute the command.
    if( command.valid() )
      command->execute ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a command.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::addCommand ( Usul::Interfaces::ICommand* command )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _commandQueue.push_back ( command );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for an interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* MinervaVR::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Minerva::Interfaces::IAnimationControl::IID:
    return static_cast < Minerva::Interfaces::IAnimationControl * > ( this );
  case Usul::Interfaces::ICommandQueueAdd::IID:
    return static_cast < Usul::Interfaces::ICommandQueueAdd * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Referenece.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::ref()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreferenece.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::unref( bool )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::stopAnimation ()
{
  _sceneManager->stopAnimation ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the animate speed.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::animateSpeed ( double speed )
{
  _sceneManager->animationSpeed ( speed );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the animate speed.
//
///////////////////////////////////////////////////////////////////////////////

double MinervaVR::animateSpeed () const
{
  return _sceneManager->animationSpeed ();
}
