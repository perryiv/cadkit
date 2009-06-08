
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSSIM/LandModelFlat.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Math/Constants.h"
#include "Usul/Trace/Trace.h"

#include "ossim/base/ossimGpt.h"
#include "ossim/base/ossimDpt.h"
#include "ossim/base/ossimKeywordNames.h"
#include "ossim/base/ossimKeywordlist.h"
#include "ossim/projection/ossimPcsCodeProjectionFactory.h"
#include "ossim/projection/ossimProjectionFactoryRegistry.h"
#include "ossim/projection/ossimProjection.h"

#include "osg/Matrixd"

using namespace Minerva::Layers::Ossim;

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
//  Convert lat, lon, height to x,y,z.
//
///////////////////////////////////////////////////////////////////////////////

void LandModelFlat::latLonHeightToXYZ ( double lat, double lon, double elevation, double& x, double& y, double& z ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 != _projection )
  {
    ossimGpt latLonPoint ( lat, lon, elevation );
#if 0
    ossimDpt p;
    _projection->worldToLineSample ( latLonPoint, p );
#else
    ossimDpt p ( _projection->forward ( latLonPoint ) );
#endif
    x = p.x;
    y = p.y;
    z = elevation;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert x,y,z to lat, lon, height.
//
///////////////////////////////////////////////////////////////////////////////

void LandModelFlat::xyzToLatLonHeight ( double x, double y, double z, double& lat, double& lon, double& elevation ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 != _projection )
  {
#if 0
    ossimGpt latLonPoint;
    _projection->lineSampleToWorld ( ossimDpt ( point.x(), point.y() ), latLonPoint );
#else
    ossimGpt latLonPoint ( _projection->inverse ( ossimDpt ( x, y ) ) );
#endif
    lon = static_cast<double> ( latLonPoint.lon );
    lat = static_cast<double> ( latLonPoint.lat );
    elevation = static_cast<double> ( z );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mesh size for the extents.
//
///////////////////////////////////////////////////////////////////////////////

LandModelFlat::MeshSize LandModelFlat::meshSize ( const LandModel::Extents &extents, const LandModel::MeshSize &ms )
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

LandModelFlat::Matrix LandModelFlat::planetRotationMatrix ( double lat, double lon, double elevation, double heading ) const
{
  Usul::Math::Vec3d p;
  this->latLonHeightToXYZ ( lat, lon, elevation, p[0], p[1], p[2] );

  Matrix m;
  m.makeRotation ( heading * Usul::Math::DEG_TO_RAD, Usul::Math::Vec3d ( 0.0, 0.0, 1.0 ) );
  m * Matrix::translation ( p );

  return m;
}
