
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/Planet/PlanetComponent.h"

#include "ossimPlanet/ossimPlanetLand.h"
#include "ossimPlanet/ossimPlanetOssimImageLayer.h"

#include <algorithm>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PlanetComponent, PlanetComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PlanetComponent::PlanetComponent() : BaseClass(),
_planets ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PlanetComponent::~PlanetComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *PlanetComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IPlanetNode::IID:
    return static_cast < Usul::Interfaces::IPlanetNode * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a node for a planet.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group*  PlanetComponent::planetNode ( const std::string& key )
{
  PlanetPtr &planet ( _planets [ key ] );

  if ( false == planet.valid() )
  {
    planet = new ossimPlanet;
    planet->land()->setLandType ( ossimPlanetLandType_ELLIPSOID );
    planet->setEnableHudFlag ( true );
    planet->hud()->setTextColor ( osg::Vec4 ( 1.0, 1.0, 1.0, 0.0 ) );
    planet->hud()->setCrosshairColor ( osg::Vec4 ( 1.0, 1.0, 1.0, 0.0 ) );

#ifdef _MSC_VER
    const std::string file0 ( "C:\\adam\\data\\earth\\blue_marble\\land_shallow_topo_west_tiled.tif" );
    const std::string file1 ( "C:\\adam\\data\\earth\\blue_marble\\land_shallow_topo_east_tiled.tif" );
#else
    const std::string file0 ( "/array/cluster/data/gis/earth/land_shallow_topo_west_tiled.tif" );
    const std::string file1 ( "/array/cluster/data/gis/earth/land_shallow_topo_east_tiled.tif" );
#endif

#if 1
    {
      osg::ref_ptr < ossimPlanetOssimImageLayer > layer ( new ossimPlanetOssimImageLayer );
      layer->openImage ( file0.c_str() );
      planet->land()->referenceLayer()->addTop ( layer.get() );
      planet->land()->resetGraph( layer->getExtents().get(), ossimPlanetLandRefreshType_TEXTURE );
    }

    {
      osg::ref_ptr < ossimPlanetOssimImageLayer > layer ( new ossimPlanetOssimImageLayer );
      layer->openImage ( file1.c_str() );
      planet->land()->referenceLayer()->addTop ( layer.get() );
      planet->land()->resetGraph( layer->getExtents().get(), ossimPlanetLandRefreshType_TEXTURE );
    }
#endif
    

  }

  return planet.get();
}
