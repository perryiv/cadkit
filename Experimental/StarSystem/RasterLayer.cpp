
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/RasterLayer.h"

#include "osg/Image"

using namespace StarSystem;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::RasterLayer() : 
  BaseClass(),
  _extents(),
  _alphas()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::~RasterLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the extents.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::extents ( const Extents& extents )
{
  Guard guard ( this );
  _extents = extents;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::Extents RasterLayer::extents() const
{
  Guard guard ( this );
  return _extents;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add an alpha value.
//
///////////////////////////////////////////////////////////////////////////////

void RasterLayer::alpha ( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha )
{
  Guard guard ( this );
  Color color ( RedGreen ( red, green ), blue );
  _alphas[color] = alpha;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the alpha values.
//
///////////////////////////////////////////////////////////////////////////////

RasterLayer::Alphas RasterLayer::alphas() const
{
  Guard guard ( this );
  return _alphas;
}
