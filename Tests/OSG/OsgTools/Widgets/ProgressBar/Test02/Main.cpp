
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Perry Miller, Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "TestJob.h"

#include "Usul/Adaptors/Random.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Trace/Trace.h"

#include "Threads/OpenThreads/Thread.h"

#include "OsgTools/Widgets/ProgressBarGroup.h"
#include "OsgTools/Widgets/ProgressBar.h"

#include "osg/Camera"
#include "osg/Group"
#include "osgViewer/Viewer"

#include <fstream>

namespace Detail
{
  typedef std::vector<unsigned long> RandomNumbers;
  RandomNumbers randomNumbers;
}
class ProgressBarGroupCallback : public osg::NodeCallback
{
  public:
  //typedef osg::Drawable::UpdateCallback   BaseClass;

  ProgressBarGroupCallback ( OsgTools::Widgets::ProgressBarGroup * pbarGroup ) :
  _pbarGroup ( pbarGroup )
  {
  }
  
  virtual void operator() (osg::Node* node, osg::NodeVisitor* nv)
  {
    if( 0x0 != _pbarGroup )
    {
      _pbarGroup->buildScene(); 
    }
    traverse(node,nv);
  }

protected:
  virtual ~ProgressBarGroupCallback()
  {
    _pbarGroup = 0x0;
  }
private:
 
  OsgTools::Widgets::ProgressBarGroup * _pbarGroup;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _test()
{
  USUL_TRACE_SCOPE_STATIC;

  Usul::CommandLine::Arguments::Args args ( Usul::CommandLine::Arguments::instance().args() );

  // The number of jobs to start.
  const unsigned int num ( ( args.size() > 1 && false == args[1].empty() ) ? 
                           ( static_cast < unsigned int > ( Usul::Math::absolute ( ::atoi ( args[1].c_str() ) ) ) ) : 
                           ( 10 ) );
                           std::cout << "Number of jobs: " << num << std::endl;

  // The pool size.
  const unsigned int size ( ( args.size() > 2 && false == args[2].empty() ) ? 
                            ( static_cast < unsigned int > ( Usul::Math::absolute ( ::atoi ( args[2].c_str() ) ) ) ) : 
                            ( 5 ) );
                            std::cout << "Pool size: " << size << std::endl;

  // Fill random number vector now. Calling rand() in the child threads is unreliable.
  Detail::randomNumbers.resize ( num * 2 );
  Usul::Adaptors::Random <double> rand ( 1.0, 25000.0 );
  std::generate ( Detail::randomNumbers.begin(), Detail::randomNumbers.end(), rand );

  // Set the job manager's thread-pool size.
  Usul::Jobs::Manager::init ( size, true );

  // Create the scene.
  osg::ref_ptr< osg::Group > root ( new osg::Group() );

  osg::ref_ptr< OsgTools::Widgets::ProgressBarGroup > progressBar ( new OsgTools::Widgets::ProgressBarGroup() );
  progressBar->defaultProgressBarSize ( Usul::Math::Vec2f ( 200, 25 ) );
  progressBar->position ( Usul::Math::Vec3f ( 10, 10, 0 ) );
  root->addChild( progressBar->getScene() );
  root->setUpdateCallback ( new ProgressBarGroupCallback ( progressBar.get() ) );

  // Construct the viewer.
  osgViewer::Viewer viewer;
  osg::ref_ptr<osg::Camera> camera ( new osg::Camera );
  camera->setRenderOrder ( osg::Camera::POST_RENDER );
  camera->setReferenceFrame ( osg::Camera::ABSOLUTE_RF );
  camera->setClearMask( GL_DEPTH_BUFFER_BIT );
  camera->setViewMatrix( osg::Matrix::identity() );
  camera->setComputeNearFarMode ( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
  camera->setCullingMode ( osg::CullSettings::NO_CULLING );
  camera->setViewport ( 0, 0, 1024, 1024 );
  camera->setProjectionMatrixAsOrtho2D ( 0, 1024, 0, 1024 );
  camera->addChild ( root.get() );
  
  viewer.setSceneData ( camera.get() );

  // Start the jobs.
  for ( unsigned int i = 0; i < num; ++i )
  {
    Usul::Jobs::Job::RefPtr job ( new TestJob ( Detail::randomNumbers.at(i) ) );

    // Set the progress and status bar.
    Usul::Interfaces::IUnknown::QueryPtr unknown ( progressBar->append() );
    job->progress ( unknown.get() );
    job->label ( unknown.get() );

    // Add the Job.
    Usul::Jobs::Manager::instance().addJob ( job.get() );
  }

  // Run the viewer.
  viewer.run();

  // Wait until all jobs finish.
  TRACE_AND_PRINT ( "Waiting for jobs to finish...\n" );
  Usul::Jobs::Manager::instance().wait();
  TRACE_AND_PRINT ( "All jobs have finished.\n" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.
//
///////////////////////////////////////////////////////////////////////////////

void _clean()
{
  USUL_TRACE_SCOPE_STATIC;

  // Clear global list of random numbers.
  Detail::randomNumbers.clear();

  // The job manager has a thread-pool.
  Usul::Jobs::Manager::destroy();

  // There should not be any threads running.
  TRACE_AND_PRINT ( "Waiting for all threads to finish...\n" );
  Usul::Threads::Manager::instance().wait();
  TRACE_AND_PRINT ( "All threads have finished.\n" );

  // Clean up the thread manager.
  Usul::Threads::Manager::destroy();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  ::srand ( 10 );
  Detail::randomNumbers.clear();

  Usul::Threads::Manager::instance().factory ( &Threads::OT::newOpenThreadsThread );

  std::ofstream trace ( "trace.csv" );
  Usul::Trace::Print::stream ( &trace );
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  Usul::Functions::safeCall ( _test,  "3274711482" );
  Usul::Functions::safeCall ( _clean, "1164335400" );

  return 0;
}
