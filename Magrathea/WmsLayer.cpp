
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Magrathea/WmsLayer.h"

using namespace Magrathea;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( WmsLayer, WmsLayer::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WmsLayer::WmsLayer() : BaseClass(),
_wmsImageLayer ( new ossimPlanetWmsImageLayer )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WmsLayer::~WmsLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the server.
//
///////////////////////////////////////////////////////////////////////////////

void WmsLayer::server( const std::string& serverString )
{
  _wmsImageLayer->setServer ( serverString );
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

const std::string& WmsLayer::server() const
{
  return _wmsImageLayer->getServer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void WmsLayer::imageType( const std::string& imageType )
{
  _wmsImageLayer->setImageType ( imageType );
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

const std::string& WmsLayer::imageType() const
{
  return _wmsImageLayer->getImageType();
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void WmsLayer::cacheDirectory( const std::string& directory )
{
  _wmsImageLayer->setCacheDirectory( directory.c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

const std::string& WmsLayer::cacheDirectory() const
{
  return _wmsImageLayer->getCacheDirectory().c_str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void WmsLayer::transparentFlag( bool flag )
{
  _wmsImageLayer->setTransparentFlag ( flag );
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

bool WmsLayer::transparentFlag() const
{
  return _wmsImageLayer->getTransparentFlag();
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void WmsLayer::transparentColorFlag( bool flag )
{
  _wmsImageLayer->setTransparentColorFlag ( flag );
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

bool WmsLayer::transparentColorFlag() const
{
  return _wmsImageLayer->getTransparentColorFlag();
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void WmsLayer::transparentColor( unsigned int r, unsigned int g, unsigned int b )
{
  _wmsImageLayer->setTransparentColor ( r, g, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* WmsLayer::queryInterface(unsigned long iid)
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ILayer::IID:
    return static_cast < Usul::Interfaces::ILayer * > ( this );
  default:
    return 0x0;
  }
}

