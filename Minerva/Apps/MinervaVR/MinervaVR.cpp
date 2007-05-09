
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
#include "Usul/File/Path.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Adaptors/MemberFunction.h"

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
  _dbManager ( new Minerva::Core::Viz::Controller (  ) ),
  _sceneManager ( new Minerva::Core::Scene::SceneManager ),
  _planet ( new Magrathea::Planet ),
  _options(),
  _background( 0.0, 0.0, 0.0, 1.0 ),
  _update( ),
  _numFramesBuild ( 0 ),
  _frameBuild ( 0 ),
  _updateThread ( 0x0 )
{
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

  std::cerr << " [MinervaVR] Constructor finished: " << std::endl;

  std::string host ( Usul::System::Host::name() );
  char num ( host[ host.size() - 1 ] );
  _frameBuild = 10 + ( num - 48 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaVR::~MinervaVR()
{
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

  setNearFar(0.0001, 10.0);
  setSceneInitialPosition( osg::Vec3f( 0.0, 10.0, 0.0 ) );

  this->_initLegend();

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
  static bool sizeSet ( false );

  if( false == sizeSet )
  {
    GLint viewport[4];

    ::glGetIntegerv( GL_VIEWPORT, viewport );

    _sceneManager->resize ( viewport[2], viewport[3] );
    
    sizeSet = true;
  }

  if( _updateThread->isIdle() )
  {
    Usul::Threads::Manager::instance().purge();
    _updateThread = 0x0;
  }

  // If there are draw commands to process...
  if( _dbManager->hasEvents() && 0x0 == _updateThread.get() )
  {
    _updateThread = Usul::Threads::Manager::instance().create();

    typedef void (MinervaVR::*Function) ( Usul::Threads::Thread *s );
    typedef Usul::Adaptors::MemberFunction < MinervaVR*, Function > MemFun;

    _updateThread->started ( Usul::Threads::newFunctionCallback( MemFun ( this, &MinervaVR::_updateScene ) ) );
    _updateThread->start();
  }
    
  //this->_updateScene();
  this->_buildScene();
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
//  Update the scene.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::_updateScene( Usul::Threads::Thread *thread )
{
  try
  {
    // If there are draw commands to process...
    //if( _dbManager->hasEvents() )
    //if ( _update->dataAvailable() )
    {
      std::cerr << " [MinervaVR] Updating scene..." << std::endl;

      // Update the scene.
      _dbManager->updateScene();
      _sceneManager->dirty ( true );

      std::cerr << " [MinervaVR] Finished updating scene." << std::endl;

      _numFramesBuild = 0;
    }
  }
  catch ( ... )
  {
    std::cerr << "[MinervaVR] exception caught while trying to update scene." << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene..
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::_buildScene()
{
  try
  {
    if( _sceneManager->dirty() && _numFramesBuild > _frameBuild )
    {
      std::cerr << " [MinervaVR] Starting building scene." << std::endl;

      _sceneManager->buildScene();
      _numFramesBuild = 0;

      std::cerr << " [MinervaVR] Finished building scene." << std::endl;

      _sceneManager->dirty ( false );
    }
    else
      ++_numFramesBuild;
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
