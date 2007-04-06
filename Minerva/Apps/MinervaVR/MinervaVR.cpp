
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

#include "VrjCore/OssimInteraction.h"

#include <fstream>

#include <osg/Group>
#include <osg/Node>

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
  _options()
{
  Minerva::Core::DB::Connection::RefPtr applicationConnection ( new Minerva::Core::DB::Connection );

  std::ifstream fin ( "Minerva.config" );

  if( fin.is_open() )
  {
    fin >> _options;

    applicationConnection->username( _options.value( USERNAME ) );
    applicationConnection->password( _options.value( PASSWORD ) );
    applicationConnection->database( _options.value( DATABASE ) );
    applicationConnection->hostname( _options.value( HOST     ) );
    applicationConnection->connect();

    _dbManager->applicationConnection( applicationConnection.get() );

    _dbManager->connectToSession( _options.value ( SESSION ) );
    _dbManager->sceneManager ( _sceneManager.get() );
  }

  std::cerr << " [MinervaVR] Constructor finished: " << std::endl;
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

  //: Set all the properties here
  setDevice(ALL, OFF);	
  setBackgroundColor((gmtl::Vec4f(0.35, 0.35, 0.35, 1.0)).getData());	

  setNearFar(0.0001, 10.0);
  setSceneInitialPosition( osg::Vec3f( 0.0, 10.0, 0.0 ) );

  getEngine()->setTransSpeed( 0.0001 );
  getEngine()->setTransDelta( 0.0001 );
  getEngine()->setRotSpeed( 0.0001 );

  _sceneManager->showLegend ( false );

  if( _options.option( LEGEND_NODE ) )
  {
    std::string legendNode ( _options.value ( LEGEND_NODE ) );
    
    if( Usul::System::Host::name() == legendNode )
    {
      _sceneManager->showLegend ( true );
      _sceneManager->legendWidth ( 0.50 );
      _sceneManager->legendPadding ( osg::Vec2 ( 20.0, 40.0 ) );
      _sceneManager->legendHeightPerItem ( 60 );
    }
  }

  // Create and set-up the interactor.
  VrjCore::OssimInteraction *interactor = new VrjCore::OssimInteraction();

  if( ossimPlanet* planet = dynamic_cast< ossimPlanet* >( _planet->root() ) )
  {
    interactor->planet( planet );
    setEngine( interactor );
  }

  std::cerr << " [MinervaVR] app init ends: " << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Drawing is about to happen. 
//
///////////////////////////////////////////////////////////////////////////////

void MinervaVR::appPreOsgDraw()
{
  //: Set background color to grey.
  glClearColor(0.7, 0.7, 0.7, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  GLint viewport[4];

  ::glGetIntegerv(GL_VIEWPORT, viewport);

  _sceneManager->resize ( viewport[2], viewport[3] );

  this->_updateScene();
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
    ::chdir ( dir.c_str() );
    _planet->readKWL( kwl );
  }
  
  std::cerr << " [MinveraVR] done reading kwl: " << std::endl;
 
  mModelGroupNode->addChild ( _planet->root() );
  
  std::cerr << " [MinervaVR] Root set: " << std::endl;

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

void MinervaVR::_updateScene()
{
  try
  {
    // If there are draw commands to process...
    if( _dbManager->hasEvents() )
    {
      std::cerr << " [MinervaVR] Updating scene..." << std::endl;

      // Update the scene.
      _dbManager->updateScene();
      _sceneManager->buildScene();

      std::cerr << " [MinervaVR] Finished updating scene." << std::endl;
    }
  }
  catch ( ... )
  {
    std::cerr << "[MinervaVR] exception caught while trying to update scene." << std::endl;
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
  //  light->setEmission( osg::Vec4 ( 0.0f, 0.0f, 0.0f, 1.0f ) );
  //light->setDirection ( osg::Vec3 ( 0.0, 0.0, -1.0f ) );
  light->setPosition( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
  light->setLightNum ( 0 );

  osg::ref_ptr < osg::LightSource > lightSource ( new osg::LightSource );

  lightSource->setLight ( light.get() );
  lightSource->setReferenceFrame ( osg::LightSource::ABSOLUTE_RF );

  mLightGroup->addChild ( lightSource.get() );

  osg::ref_ptr < osg::StateSet > ss ( mSceneRoot->getOrCreateStateSet() );
  lightSource->setStateSetModes( *ss.get(), osg::StateAttribute::ON );
    //ss->setAttributeAndModes( light.get(), osg::StateAttribute::ON );
}
