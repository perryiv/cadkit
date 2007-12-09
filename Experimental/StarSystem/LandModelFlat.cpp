
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/LandModelFlat.h"

#include "Usul/Trace/Trace.h"

#include "ossim/base/ossimGpt.h"
#include "ossim/base/ossimDpt.h"
#include "ossim/base/ossimKeywordNames.h"
#include "ossim/base/ossimKeywordlist.h"
#include "ossim/projection/ossimProjectionFactoryRegistry.h"
#include "ossim/projection/ossimProjection.h"

using namespace StarSystem;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LandModelFlat::LandModelFlat ( unsigned int pcscode ) : 
  BaseClass(),
  _projection ( 0x0 )
{
  USUL_TRACE_SCOPE;

  ossimKeywordlist kwl;

  std::ostringstream os;
  os << pcscode;

  kwl.add( ossimKeywordNames::PCS_CODE_KW, os.str().c_str() );

  _projection = ossimProjectionFactoryRegistry::instance()->createProjection( kwl );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LandModelFlat::LandModelFlat ( const std::string& name ) : 
  BaseClass(),
  _projection ( 0x0 )
{
  USUL_TRACE_SCOPE;
  _projection = ossimProjectionFactoryRegistry::instance()->createProjection( ossimString ( name.c_str() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LandModelFlat::~LandModelFlat ()
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != _projection )
    delete _projection;
  
  _projection = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

double LandModelFlat::size () const
{
  USUL_TRACE_SCOPE;

  osg::Vec3f v0, v1;
  this->latLonHeightToXYZ ( 0, 0, 0, v0 );
  this->latLonHeightToXYZ ( -90, -180, 0, v1 );

  return (v1 - v0).length() / 10;
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation at given lat, lon.
//
///////////////////////////////////////////////////////////////////////////////

double LandModelFlat::elevation ( double lat, double lon ) const
{
  USUL_TRACE_SCOPE;
  return 0.0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert lat, lon, height to x,y,z.
//
///////////////////////////////////////////////////////////////////////////////

void LandModelFlat::latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3f& point ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  typedef osg::Vec3f::value_type ValueType;

  if ( 0x0 != _projection )
  {
    ossimDpt p;
    ossimGpt latLonPoint ( lat, lon, elevation );
    _projection->worldToLineSample ( latLonPoint, p );
    point.set ( static_cast<ValueType> ( p.x ), static_cast<ValueType> ( p.y ), 0.0 );
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mesh size for the extents.
//
///////////////////////////////////////////////////////////////////////////////

LandModel::MeshSize LandModelFlat::meshSize ( const LandModel::Extents &extents, const LandModel::MeshSize &ms )
{
  USUL_TRACE_SCOPE;
  return ms;
}
