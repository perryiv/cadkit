
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/TileEngine/LandModelFlat.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Trace/Trace.h"

#include "ossim/base/ossimGpt.h"
#include "ossim/base/ossimDpt.h"
#include "ossim/base/ossimKeywordNames.h"
#include "ossim/base/ossimKeywordlist.h"
#include "ossim/projection/ossimPcsCodeProjectionFactory.h"
#include "ossim/projection/ossimProjectionFactoryRegistry.h"
#include "ossim/projection/ossimProjection.h"

#include "osg/Matrixd"

using namespace Minerva::Core::TileEngine;

USUL_FACTORY_REGISTER_CREATOR ( LandModelFlat );


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

  std::ostringstream os;
  os << pcscode;

  _projection = ossimPcsCodeProjectionFactory::instance()->createProjection( os.str() );

  // If we didn't create one, fall back on the factory.
  if ( 0x0 == _projection )
  {
    ossimKeywordlist kwl;
    kwl.add( ossimKeywordNames::PCS_CODE_KW, os.str().c_str() );

    _projection = ossimProjectionFactoryRegistry::instance()->createProjection( kwl );
  }
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

  osg::Vec3d v0, v1;
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

void LandModelFlat::latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3d& point ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  typedef osg::Vec3d::value_type ValueType;

  if ( 0x0 != _projection )
  {
    ossimGpt latLonPoint ( lat, lon, elevation );
#if 0
    ossimDpt p;
    _projection->worldToLineSample ( latLonPoint, p );
#else
    ossimDpt p ( _projection->forward ( latLonPoint ) );
#endif
    point.set ( static_cast<ValueType> ( p.x ), static_cast<ValueType> ( p.y ), elevation );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert x,y,z to lat, lon, height.
//
///////////////////////////////////////////////////////////////////////////////

void LandModelFlat::xyzToLatLonHeight ( const osg::Vec3d& point, double& lat, double& lon, double& elevation ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 != _projection )
  {
#if 0
    ossimGpt latLonPoint;
    _projection->lineSampleToWorld ( ossimDpt ( point.x(), point.y() ), latLonPoint );
#else
    ossimGpt latLonPoint ( _projection->inverse ( ossimDpt ( point.x(), point.y() ) ) );
#endif
    lon = static_cast<double> ( latLonPoint.lon );
    lat = static_cast<double> ( latLonPoint.lat );
    elevation = static_cast<double> ( point.z() );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Serialize the members.
//
///////////////////////////////////////////////////////////////////////////////

void LandModelFlat::serialize ( XmlTree::Node &parent ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Copy the map.
  Serialize::XML::DataMemberMap m ( _dataMemberMap );

  // Write members to the node from local map.
  m.serialize ( parent );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize the members.
//
///////////////////////////////////////////////////////////////////////////////

void LandModelFlat::deserialize ( const XmlTree::Node &node )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Copy the map.
  Serialize::XML::DataMemberMap m ( _dataMemberMap );

  // Initialize locals and members from the the node.
  m.deserialize ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Matrix to place items on the planet (i.e. local coordinates to world coordinates).
//
///////////////////////////////////////////////////////////////////////////////

osg::Matrixd LandModelFlat::planetRotationMatrix ( double lat, double lon, double elevation, double heading ) const
{
  osg::Vec3d p;
  this->latLonHeightToXYZ ( lat, lon, elevation, p );

  return osg::Matrixd::rotate ( osg::DegreesToRadians ( heading ), osg::Vec3f ( 0.0, 0.0, 1.0 ) ) * osg::Matrixd::translate ( p );
}
