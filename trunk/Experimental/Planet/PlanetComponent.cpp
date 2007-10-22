
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
    planet->getLand()->setLandType ( ossimPlanetLandType_ELLIPSOID );
    planet->setEnableHudFlag ( false );

    {
      osg::ref_ptr < ossimPlanetOssimImageLayer > layer ( new ossimPlanetOssimImageLayer );
      layer->openImage ( "C:\\adam\\data\\earth\\blue_marble\\land_shallow_topo_west_tiled.tif" );
      planet->getLand()->setTextureLayer ( layer.get(), 0 );
      planet->getLand()->resetGraph( layer->getExtents().get(), ossimPlanetPagedLandLodRefreshType_TEXTURE );
    }

    {
      osg::ref_ptr < ossimPlanetOssimImageLayer > layer ( new ossimPlanetOssimImageLayer );
      layer->openImage ( "C:\\adam\\data\\earth\\blue_marble\\land_shallow_topo_east_tiled.tif" );
      planet->getLand()->setTextureLayer ( layer.get(), 1 );
      planet->getLand()->resetGraph( layer->getExtents().get(), ossimPlanetPagedLandLodRefreshType_TEXTURE );
    }

    

  }

  return planet.get();
}
