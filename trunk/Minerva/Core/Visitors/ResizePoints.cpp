
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Visitors/ResizePoints.h"
#include "Minerva/Core/DataObjects/Point.h"

using namespace Minerva::Core::Visitors;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ResizePoints::ResizePoints( double factor ) : 
  BaseClass(),
  _factor ( factor )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ResizePoints::~ResizePoints()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit a point.
//
///////////////////////////////////////////////////////////////////////////////

void ResizePoints::visit ( Minerva::Core::DataObjects::Point &point )
{
  point.size( point.size() * _factor );
}
