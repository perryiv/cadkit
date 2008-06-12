
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Visitors/StackPoints.h"
#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Layers/Container.h"
#include "Minerva/Core/Geometry/Point.h"

using namespace Minerva::Core::Visitors;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

StackPoints::StackPoints( double multiplier ) : 
  BaseClass(),
  _counts( LessVector ( EqualPredicate() ) ),
  _multiplier ( multiplier )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

StackPoints::~StackPoints()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit a point.
//
///////////////////////////////////////////////////////////////////////////////

void StackPoints::visit ( Minerva::Core::Data::DataObject &dataObject )
{
  typedef Minerva::Core::Geometry::Point Point;
  typedef Minerva::Core::Data::DataObject::Geometries Geometries;
  Geometries geometries ( dataObject.geometries() );

  for ( Geometries::iterator iter = geometries.begin(); iter != geometries.end(); ++iter )
  {
    if ( Point* point = dynamic_cast<Point*> ( (*iter).get() ) )
    {
      Usul::Math::Vec3d p ( point->pointData() );

      const unsigned int multiplier ( 100000 );
      Usul::Math::Vec3ui center ( static_cast<unsigned int> ( p[0] * multiplier ),
                                  static_cast<unsigned int> ( p[1] * multiplier ),
                                  /*static_cast<unsigned int> ( p[2] * multiplier )*/ 0 );

      Counts::iterator countIter = _counts.find ( center );
      if ( _counts.end() == countIter )
      {
        _counts[center] = 1;
      }
      else
      {
        // Make sure it's using the right altitude mode.
        point->altitudeMode ( Minerva::Core::Geometry::Geometry::RELATIVE_TO_GROUND );

        unsigned int count ( countIter->second );

        const bool autotransform ( point->autotransform() );
        const float size ( point->size() );

        const osg::Vec3& o ( point->spatialOffset() );
        const double distance ( count * _multiplier * ( autotransform ? 20 : size ) );
        point->spatialOffset ( osg::Vec3f ( o.x(), o.y(), /*o.z() +*/ ( distance ) ) );
        dataObject.dirty( true );

        countIter->second = count + 1;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit a point layer.
//
///////////////////////////////////////////////////////////////////////////////

void StackPoints::visit ( Minerva::Core::Layers::Container &layer )
{
  if ( layer.showLayer() )
  {
    layer.traverse ( *this );
    layer.dirtyScene ( true );
  }
}
