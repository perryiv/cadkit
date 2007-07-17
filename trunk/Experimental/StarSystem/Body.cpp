
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
//  Body class.
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/Body.h"
#include "StarSystem/Visitor.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

#include "ossimPlanet/ossimPlanet.h"
#include "ossimPlanet/ossimPlanetLand.h"

#include "osgDB/DatabasePager"

#include "osg/Group"

using namespace StarSystem;

STAR_SYSTEM_IMPLEMENT_NODE_CLASS ( Body );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Body::Body() : BaseClass(),
  _planet ( new ossimPlanet() ),
  _pager  ( new osgDB::DatabasePager() )
{
  USUL_TRACE_SCOPE;

  // Not using smart pointers.
  _planet->ref();

  // Set the default attributes.
  _planet->getLand()->setLandType ( ossimPlanetLandType_NORMALIZED_ELLIPSOID );
  _planet->getLand()->setElevationEnabledFlag ( true );
  _planet->getLand()->setHeightExag ( 1.0 );
  _planet->getLand()->setElevationPatchSize ( 16 );
  _planet->getLand()->setMaxLevelDetail ( 32 );
  _planet->setEnableEphemerisFlag ( false );
  _planet->setEnableHudFlag ( true );
  _planet->getLand()->resetGraph();

  // Initialize the database pager.
  _pager->ref();
  _pager->setExpiryDelay ( 0 );
  _pager->setUseFrameBlock ( true );
  _pager->setAcceptNewDatabaseRequests ( true );
  _pager->setDatabasePagerThreadPause ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Body::~Body()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Body::_destroy ), "3973302267" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Body::_destroy()
{
  USUL_TRACE_SCOPE;
  _planet->unref(); _planet = 0x0;
  _pager->unref(); _pager = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *Body::buildScene ( const Body::BuildOptions &options, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( 0x0 != _planet ) ? _planet : new osg::Group() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the database pager.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::DatabasePager *Body::databasePager()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _pager;
}
