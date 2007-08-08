
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
#include "Usul/Documents/Manager.h"

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
const std::string NEAR_FAR      = "near_far";

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaVR::MinervaVR( vrj::Kernel* kern, int& argc, char** argv ) : 
  BaseClass ( kern, argc, argv ),
  _document ( new Minerva::Document::MinervaDocument ),
  _options(),
  _background( 0.0, 0.0, 0.0, 1.0 ),
  _mutex(),
  _commandQueue ()
{
  // Set our document as the active one.
  Usul::Documents::Manager::instance().active ( _document );

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

  // Create the connection.
  Minerva::Core::DB::Connection::RefPtr connection ( new Minerva::Core::DB::Connection );
  connection->username( _options.value( USERNAME ) );
  connection->password( _options.value( PASSWORD ) );
  connection->database( _options.value( DATABASE ) );
  connection->hostname( _options.value( HOST     ) );
  connection->connect();
  connection->activate();

  // Set the connection.
  _document->connection ( connection.get() );

  // Set the session.
  std::string session ( _options.value ( SESSION ) );
  _document->session( session );

  // We want to receive commands.
  _document->commandsReceive ( true );

  // Connect to the session.
  _document->connectToSession ();

  std::cerr << " [MinervaVR] Connected to session: " << session << std::endl;

  if( _options.option ( BACKGROUND ) )
  {
    std::string background ( _options.value ( BACKGROUND ) );
    std::istringstream in ( background );
    in >> _background[0] >> _background[1] >> _background[2] >> _background[3];
  }

  // Set the background color.
  setBackgroundColor( _background.ptr() );

  float zNear ( 0.000001 ), zFar ( 15.0 );

  if ( _options.option ( NEAR_FAR ) )
  {
    std::istringstream in ( _options.value ( NEAR_FAR ) );
    in >> zNear >> zFar;
  }

  // Set the near and far planes.
  vrj::Projection::setNearFar( zNear, zFar );

  // Initialize the legend.
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
  _document->showLegend ( false );

  if( _options.option( LEGEND_NODE ) )
  {
    std::string legendNode ( _options.value ( LEGEND_NODE ) );
    
    if( Usul::System::Host::name() == legendNode )
    {
      _document->showLegend ( true );
      _document->sceneManager()->legendWidth ( 0.75 );
      _document->sceneManager()->legendPadding ( osg::Vec2 ( 20.0, 40.0 ) );
      _document->sceneManager()->legendHeightPerItem ( 60 );
      _document->sceneManager()->legendPosition( Minerva::Core::Scene::SceneManager::LEGEND_TOP_LEFT );
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
  BaseClass::preFrame();

  _document->updateNotify ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );

  // Purge any threads that may be finished.
  Usul::Threads::Manager::instance().purge();
  Usul::Jobs::Manager::instance().purge(); 
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

    _document->sceneManager()->resize ( viewport[2], viewport[3] );
    
    sizeSet = true;
  }
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

  // Set the database pager.
  mOsgDatabasePager = _document->getDatabasePager ();

  std::cerr << " [MinervaVR] begin reading kwl: " << std::endl;

  if( _options.option ( KWL ) )
  {
    std::string kwl ( _options.value ( KWL ) );
    std::string dir ( Usul::File::directory ( kwl, false ) );

    //::chdir ( dir.c_str() );
    _document->read( kwl.c_str() );
  }
  
  std::cerr << " [MinveraVR] done reading kwl: " << std::endl;
 
  if( _options.option ( EPHEMERIS ) )
  {
    std::string e ( _options.value ( EPHEMERIS ) );
    std::istringstream in ( e );
    bool useEphemeris ( false );
    in >> useEphemeris;
    _document->planet()->ephemerisFlag( useEphemeris );
  }

  // Add the planet to the scene.
  mModelGroupNode->addChild ( _document->buildScene( Usul::Documents::Document::Options () ) );

  // Create and set-up the interactor.
  VrjCore::OssimInteraction *interactor = new VrjCore::OssimInteraction();

  // Add the compass.
  //if( "viz0" == Usul::System::Host::name() )
  //  mModelGroupNode->addChild ( interactor->compass() );

  if( ossimPlanet* planet = dynamic_cast< ossimPlanet* >( _document->planet()->root() ) )
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
    _commandQueue.pop_front();

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
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::startAnimation ()
{
  _document->sceneManager()->startAnimation ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::stopAnimation ()
{
  _document->sceneManager()->stopAnimation ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pause the animation.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::pauseAnimation ()
{
  _document->sceneManager()->pauseAnimation ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the animate speed.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::animateSpeed ( double speed )
{
  _document->sceneManager()->animationSpeed ( speed );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the animate speed.
//
///////////////////////////////////////////////////////////////////////////////

double MinervaVR::animateSpeed () const
{
  return _document->sceneManager()->animationSpeed ();
}
