
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "MinervaVR.h"


//#include "ossim/base/ossimNotify.h"
//#include "ossim/base/ossimTraceManager.h"

class ComputeBounds : public osg::Node::ComputeBoundingSphereCallback
{
public:
  virtual osg::BoundingSphere computeBound ( const osg::Node& )
  {
     return osg::BoundingSphere ( osg::Vec3 ( 0.0, 0.0, 0.0 ), 1 );
  }
};

#include "Magrathea/Planet.h"

#include "Usul/System/Host.h"

#include <fstream>

#include <osg/Group>
#include <osg/Node>

#if _MSC_VER
#include <direct.h>
#endif



MinervaVR::MinervaVR( vrj::Kernel* kern, int& argc, char** argv ) : 
  OsgVJApp( kern, argc, argv ),
  _dbManager ( new Minerva::Core::Viz::Controller (  ) ),
  _sceneManager ( new Minerva::Scene::SceneManager ),
  _planet ( new Magrathea::Planet )
{
  Minerva::DB::Connection::RefPtr applicationConnection ( new Minerva::DB::Connection );

  // TODO: Put this in a config file.
  applicationConnection->username( "wnv_app" );
  applicationConnection->password( "wnv" );
  applicationConnection->database( "wnv_application" );
  applicationConnection->hostname ( "cinema" );
  applicationConnection->connect();

  _dbManager->applicationConnection( applicationConnection.get() );

  _dbManager->connectToSession( "drum_session" );
  _dbManager->sceneManager ( _sceneManager.get() );
}


MinervaVR::~MinervaVR()
{
}


//:---------------------------------------------------------------------
//: One time initialization. 
//:---------------------------------------------------------------------
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

  // TODO: put this in a config file.
  if( Usul::System::Host::name() != "viz1" )
    _sceneManager->showLegend ( false );
  else
  {
    _sceneManager->legendWidth ( 0.50 );
    _sceneManager->legendPadding ( osg::Vec2 ( 20.0, 40.0 ) );
    _sceneManager->legendHeightPerItem ( 60 );
  }

  std::cerr << " [MinervaVR] app init ends: " << std::endl;
}


void MinervaVR::appPreOsgDraw()
{
  //: Set background color to grey.
  glClearColor(0.7, 0.7, 0.7, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  GLint viewport[4];

  ::glGetIntegerv(GL_VIEWPORT, viewport);

  _sceneManager->resize ( viewport[0], viewport[1], viewport[2], viewport[3] );

  this->_updateScene();
}

//:---------------------------------------------------------------------
//: Scenegraph will be build here. 
//:---------------------------------------------------------------------
void MinervaVR::appSceneInit()
{	
  std::cerr << " [MinervaVR] Scene init starts: " << std::endl;

  //ossimTraceManager::instance()->setTracePattern("ossim*");
  //ossimNotify(ossimNotifyLevel_DEBUG);
  //ossimEnableNotify( ossimNotifyFlags_ALL );
  //ossimSetLogFilename("/array/cluster/home/adam/ossim.log");
  
  _planet->init();

  std::cerr << " [MinervaVR] planet initialized: " << std::endl;

  mOsgDatabasePager = _planet->databasePager();

  std::cerr << " [MinervaVR] DB pager set: " << std::endl;

  _planet->root()->addChild( _sceneManager->root() );

  std::cerr << " [MinervaVR] begin reading kwl: " << std::endl;

#if _MSC_VER
  ::chdir ( "F:\\data\\images\\earth" );
  _planet->readKWL( "F:\\data\\images\\earth\\blue_marble.kwl" );
#else
  _planet->readKWL("/array/cluster/demos/ossimTest/MCFCD.kwl");
  //_planet->readKWL("/array/cluster/demos/ossimTest/earth.kwl");
  //_planet->readKWL("/array/cluster/demos/ossimTest/afghanistan.kwl" );
  //_planet->readKWL("/array/cluster/home/adam/sdk/ossimPlanet/examples/ossimplanetviewer/test.kwl");

#endif

  
  std::cerr << " [MinveraVR] done reading kwl: " << std::endl;
 
  _planet->root()->setComputeBoundingSphereCallback ( new ComputeBounds() );
  mModelGroupNode->addChild ( _planet->root() );
  
  std::cerr << " [MinervaVR] Root set: " << std::endl;

  //this->_updateScene();

  // TODO: This should be done in a environment variable.
  // get the existing high level path list
  osgDB::FilePathList wFilePathList = osgDB::Registry::instance()->getDataFilePathList();
    
  // add additional paths of interest to the path list
  wFilePathList.push_back( "/array/cluster/home/adam/src-osg-1.0/bin/fonts" );
    
  // re-assign the expanded path list
  osgDB::Registry::instance()->setDataFilePathList( wFilePathList );
  
  std::cerr << " [MinervaVR] Scene init ends: " << std::endl;
}


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

      std::cerr << " [MinervaVR] Finished updating scene." << std::endl;
    }
  }
  catch ( ... )
  {
    std::cerr << "[MinervaVR] exception caught while trying to update scene." << std::endl;
  }
}


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
