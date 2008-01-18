
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/ProjectionManager/ProjectionManagerComponent.h"

#include "ossim/base/ossimKeywordNames.h"
#include "ossim/base/ossimKeywordlist.h"
#include "ossim/projection/ossimPcsCodeProjectionFactory.h"
#include "ossim/projection/ossimProjectionFactoryRegistry.h"
#include "ossim/projection/ossimMapProjection.h"
#include "ossim/base/ossimGpt.h"
#include "ossim/base/ossimEcefPoint.h"
#include "ossim/base/ossimGeoidManager.h"

#include "ossim/base/ossimEllipsoid.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ProjectionManagerComponent, ProjectionManagerComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

ProjectionManagerComponent::ProjectionManagerComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

ProjectionManagerComponent::~ProjectionManagerComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ProjectionManagerComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::IProjectCoordinates::IID:
    return static_cast < Usul::Interfaces::IProjectCoordinates* > ( this );
  case Usul::Interfaces::IPlanetCoordinates::IID:
    return static_cast < Usul::Interfaces::IPlanetCoordinates* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IPlugin implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string ProjectionManagerComponent::getPluginName() const 
{
  return "Projection Manager";
}

namespace Detail
{
  bool isSridSpherical ( unsigned int srid )
  {
    return srid == 4326 || srid == 4269;
  }

  void projectFromGpt( const ossimGpt& gpt, Usul::Math::Vec3d& latLonPoint )
  {
    latLonPoint[0] = gpt.lon;
    latLonPoint[1] = gpt.lat;
    latLonPoint[2] = 0.0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Project to lat/lon with elevation using given spatial reference id.
//  Note: Longitude is x and latidute is y.
//
///////////////////////////////////////////////////////////////////////////////

void ProjectionManagerComponent::projectToSpherical ( const Usul::Math::Vec3d& orginal, unsigned int srid, Usul::Math::Vec3d& latLonPoint ) const
{
  std::ostringstream os;
  os << srid;

  ossimRefPtr < ossimProjection > projection ( ossimPcsCodeProjectionFactory::instance()->createProjection( os.str() ) );

  if ( false == projection.valid() )
  {
    ossimKeywordlist kwl;
    kwl.add( ossimKeywordNames::PCS_CODE_KW, os.str().c_str() );

    projection = ossimProjectionFactoryRegistry::instance()->createProjection( kwl );
  }

  ossimMapProjection *mapProj = dynamic_cast < ossimMapProjection * > ( projection.get() );

  if( Detail::isSridSpherical( srid ) )
  {
    // Index 0 contains longitude, index 1 contains latitude.
    ossimGpt gpt ( orginal[1], orginal[0] ); // Lat is first agrument, long is second.
    Detail::projectFromGpt( gpt, latLonPoint );
  }
  else if( projection.valid() && 0x0 != mapProj )
  {
    ossimGpt gpt;
    mapProj->eastingNorthingToWorld( ossimDpt ( orginal[0], orginal[1] ), gpt );

    Detail::projectFromGpt( gpt, latLonPoint );
  }

  /// Add any height offset.
  latLonPoint[2] += orginal[2];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Project to planet coordinates.  Values are normalized.
//  Note: Longitude is x and latidute is y.
//
///////////////////////////////////////////////////////////////////////////////

void ProjectionManagerComponent::convertToPlanet ( const Usul::Math::Vec3d& orginal, Usul::Math::Vec3d& planetPoint ) const
{
  ossimEcefPoint ecef;
  ossimGpt dummy;
  ecef = dummy;

  ossimGpt gpt ( orginal[1], orginal[0], orginal[2] );

  // Transform to ossimPlanet coordinates
  ecef = gpt;
  planetPoint[0] = ecef.x();
  planetPoint[1] = ecef.y();
  planetPoint[2] = ecef.z();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert from planet coordinats to lat/long.
//
///////////////////////////////////////////////////////////////////////////////

void ProjectionManagerComponent::convertFromPlanet ( const Usul::Math::Vec3d& planetPoint, Usul::Math::Vec3d& latLonPoint ) const
{
  ossimEcefPoint ecef;
  ossimGpt dummy;
  ecef = dummy;

  ecef = ossimEcefPoint( planetPoint [ 0 ], planetPoint [ 1 ], planetPoint [ 2 ] );

  ossimGpt gpt ( ecef );

  latLonPoint [ 0 ] = gpt.lon;
  latLonPoint [ 1 ] = gpt.lat;
  latLonPoint [ 2 ] = gpt.hgt;
}
