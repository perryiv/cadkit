
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

#include "Usul/Components/Factory.h"
#include "Usul/Strings/Format.h"

#include "ossim/base/ossimKeywordNames.h"
#include "ossim/base/ossimKeywordlist.h"
#include "ossim/projection/ossimPcsCodeProjectionFactory.h"
#include "ossim/projection/ossimProjectionFactoryRegistry.h"
#include "ossim/projection/ossimMapProjection.h"
#include "ossim/base/ossimGpt.h"

USUL_DECLARE_COMPONENT_FACTORY ( ProjectionManagerComponent );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Is the srid a spherical system (lat/lon)?
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  bool isSridSpherical ( unsigned int srid )
  {
    return srid == 4326 || srid == 4269;
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

  // Try the pcs code factory first.
  ossimRefPtr < ossimProjection > projection ( ossimPcsCodeProjectionFactory::instance()->createProjection( os.str() ) );

  if ( false == projection.valid() )
  {
    ossimKeywordlist kwl;
    kwl.add( ossimKeywordNames::PCS_CODE_KW, os.str().c_str() );

    projection = ossimProjectionFactoryRegistry::instance()->createProjection( kwl );
  }

  ossimMapProjection *mapProj = dynamic_cast < ossimMapProjection * > ( projection.get() );

  ossimGpt point;

  if( Detail::isSridSpherical( srid ) )
  {
    // Index 0 contains longitude, index 1 contains latitude.
    point.lon = orginal[0];
    point.lat = orginal[1];
  }
  else if( projection.valid() && 0x0 != mapProj )
  {
    mapProj->eastingNorthingToWorld( ossimDpt ( orginal[0], orginal[1] ), point );
  }

  // Set the values.
  latLonPoint[0] = point.lon;
  latLonPoint[1] = point.lat;

  /// Set the orginal height.
  latLonPoint[2] = orginal[2];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add UTM zones.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class Container >
  void addUTMZones ( Container& c, unsigned int start, const std::string& datum, const std::string hemisphere )
  {
    for ( unsigned int i = 1; i <= 60; ++i )
    {
      std::string name ( Usul::Strings::format ( "UTM zone ", i, " ", hemisphere, " ", datum ) );
      c.push_back ( typename Container::value_type ( name, start + i ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a list of projection names.
//  http://www.remotesensing.org/geotiff/spec/geotiff6.html
//
///////////////////////////////////////////////////////////////////////////////

ProjectionManagerComponent::Projections ProjectionManagerComponent::projectionList() const
{
  Projections projections;

  // Add geographic projections.
  ProjectionInfos geographic;
  geographic.push_back ( ProjectionInfo ( "WGS 84", 4326 ) );
  projections.insert ( Projections::value_type ( "Geographic", geographic ) );

  // Add utm projections.
  ProjectionInfos utm;

  //WGS72 / UTM northern hemisphere:	322zz where zz is UTM zone number
  Detail::addUTMZones ( utm, 32200, "WGS72", "N" );
  
  //WGS72 / UTM southern hemisphere:	323zz where zz is UTM zone number
  Detail::addUTMZones ( utm, 32300, "WGS72", "S" );

  //WGS72BE / UTM northern hemisphere: 324zz where zz is UTM zone number
  Detail::addUTMZones ( utm, 32400, "WGS72BE", "N" );
  
  //WGS72BE / UTM southern hemisphere: 325zz where zz is UTM zone number
  Detail::addUTMZones ( utm, 32500, "WGS72BE", "S" );

  //WGS84 / UTM northern hemisphere:	326zz where zz is UTM zone number
  Detail::addUTMZones ( utm, 32600, "WGS84", "N" );

  //WGS84 / UTM southern hemisphere:	327zz where zz is UTM zone number
  Detail::addUTMZones ( utm, 32700, "WGS84", "S" );

  projections.insert ( Projections::value_type ( "UTM", utm ) );

  return projections;
}
