
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Visitors/ResizePoints.h"
#include "Minerva/Core/DataObjects/DataObject.h"
#include "Minerva/Core/Geometry/Point.h"

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

void ResizePoints::visit ( Minerva::Core::DataObjects::DataObject &dataObject )
{
  typedef Minerva::Core::Geometry::Point Point;
  typedef Minerva::Core::DataObjects::DataObject::Geometries Geometries;
  Geometries geometries ( dataObject.geometries() );
  
  for ( Geometries::iterator iter = geometries.begin(); iter != geometries.end(); ++iter )
  {
    if ( Point* point = dynamic_cast<Point*> ( (*iter).get() ) )
    {
      point->size( point->size() * _factor );
      dataObject.dirty ( true );
    }
  }
}