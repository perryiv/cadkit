
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Database pager class.
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/Pager.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/ICullSceneVisitor.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Interfaces/IUpdateSceneVisitor.h"
#include "Usul/System/Sleep.h"
#include "Usul/Trace/Trace.h"

#include "osgDB/DatabasePager"

#include "osgUtil/CullVisitor"
#include "osgUtil/UpdateVisitor"

using namespace StarSystem;


///////////////////////////////////////////////////////////////////////////////
//
//  Automatically destroy pager.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  struct ScopedPager
  {
    ~ScopedPager()
    {
      StarSystem::Pager::destroy();
    }
  } _scopedPager;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Static data members.
//
///////////////////////////////////////////////////////////////////////////////

Pager *Pager::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Pager::Pager() : 
  _mutex ( new Pager::Mutex() ),
  _pager ( new DataPager() )
{
  USUL_TRACE_SCOPE;

  // Reference the database pager.
  _pager->ref();

  // Set the amount of time that a subgraph will be kept without being 
  // visited in the cull traversal before being removed.
  _pager->setExpiryDelay ( 0 );

  // Set the use of the frame block which, if enabled, blocks the 
  // DatabasePager from executing when the current frame is being drawn.
  // Note: Setting to "true" breaks it.
  _pager->setUseFrameBlock ( false );

  // Set whether new database request calls are accepted or ignored.
  _pager->setAcceptNewDatabaseRequests ( true );

  // Set whether the database pager thread should be paused or not.
  _pager->setDatabasePagerThreadPause ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Pager::~Pager()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Pager::_destroy ), "2555885310" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Pager::_destroy()
{
  USUL_TRACE_SCOPE;

  // Required in order to shut down.
  _pager->setUseFrameBlock ( false );

  // Tell the pager to not accept any more requests.
  _pager->setAcceptNewDatabaseRequests ( false );

  // Cancel the thread.
  _pager->cancel();

  // Wait for the thread to exit.
  while ( _pager->isRunning() )
    Usul::System::Sleep::seconds ( 1 );

  // Done with this pointer.
  _pager->unref();
  _pager = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the single instance.
//
///////////////////////////////////////////////////////////////////////////////

Pager &Pager::instance()
{
  USUL_TRACE_SCOPE_STATIC;
  if ( 0x0 == _instance )
  {
    _instance = new Pager();
  }
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the single instance.
//
///////////////////////////////////////////////////////////////////////////////

void Pager::destroy()
{
  USUL_TRACE_SCOPE_STATIC;
  if ( 0x0 != _instance )
  {
    delete _instance;
    _instance = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the mutex.
//
///////////////////////////////////////////////////////////////////////////////

Pager::Mutex &Pager::mutex() const
{
  USUL_TRACE_SCOPE;
  return *_mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the database pager.
//
///////////////////////////////////////////////////////////////////////////////

Pager::DataPager *Pager::pager()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _pager;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-render notification.
//
///////////////////////////////////////////////////////////////////////////////

void Pager::preRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  Usul::Interfaces::IFrameStamp::QueryPtr fs ( caller );
  if ( ( true == fs.valid() ) && ( 0x0 != fs->frameStamp() ) )
  {
    Guard guard ( this->mutex() );
    if ( 0x0 != _pager )
    {
      _pager->signalBeginFrame ( fs->frameStamp() );
      _pager->updateSceneGraph ( fs->frameStamp()->getReferenceTime() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-render notification.
//
///////////////////////////////////////////////////////////////////////////////

void Pager::postRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  if ( 0x0 != _pager )
  {
    _pager->signalEndFrame();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the cull and update visitors of the caller.
//
///////////////////////////////////////////////////////////////////////////////

void Pager::initVisitors ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // Set cull visitor's database pager.
  {
    Usul::Interfaces::ICullSceneVisitor::QueryPtr getVisitor ( caller );
    if ( true == getVisitor.valid() )
    {
      osg::ref_ptr<osgUtil::CullVisitor> visitor ( getVisitor->getCullSceneVisitor ( 0x0 ) );
      if ( true == visitor.valid() )
      {
        Guard guard ( this->mutex() );
        visitor->setDatabaseRequestHandler ( _pager );
      }
    }
  }

  // Set update visitor's database pager.
  {
    Usul::Interfaces::IUpdateSceneVisitor::QueryPtr getVisitor ( caller );
    if ( true == getVisitor.valid() )
    {
      osg::ref_ptr<osg::NodeVisitor> visitor ( getVisitor->getUpdateSceneVisitor ( 0x0 ) );
      if ( true == visitor.valid() )
      {
        Guard guard ( this->mutex() );
        visitor->setDatabaseRequestHandler ( _pager );
      }
    }
  }
}
