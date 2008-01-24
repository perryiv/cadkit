
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Visitors/StackPoints.h"
#include "Minerva/Core/Layers/PointLayer.h"
#include "Minerva/Core/DataObjects/Point.h"

#include "Usul/Interfaces/IGeometryCenter.h"
#include "Usul/Interfaces/IOffset.h"

using namespace Minerva::Core::Visitors;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

StackPoints::StackPoints() : BaseClass(),
  _counts( LessVector ( EqualPredicate() ) )
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

void StackPoints::visit ( Minerva::Core::DataObjects::Point &point )
{
  unsigned int srid ( 0 );
  
  Usul::Interfaces::IGeometryCenter::QueryPtr geometryCenter ( point.geometry() );
  
  if( geometryCenter.valid () )
  {
    osg::Vec3 p ( geometryCenter->geometryCenter( srid ) );

    const unsigned int multiplier ( 100000 );
    Usul::Math::Vec3ui center ( static_cast<unsigned int> ( p[0] * multiplier ),
                                static_cast<unsigned int> ( p[1] * multiplier ),
                                static_cast<unsigned int> ( p[2] * multiplier ) );

    Counts::iterator iter = _counts.find ( center );
    if ( _counts.end() == iter )
    {
      _counts[center] = 1;
    }
    else
    {
      unsigned int count ( iter->second );
      
      Usul::Interfaces::IOffset::QueryPtr offset ( point.geometry() );
      
      if( offset.valid () )
      {
        const osg::Vec3& o ( offset->spatialOffset() );
        offset->spatialOffset ( osg::Vec3f ( o.x(), o.y(), /*o.z() +*/ ( count * 20 ) ) );
        point.dirty( true );
      }
      
      iter->second = count + 1;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit a point layer.
//
///////////////////////////////////////////////////////////////////////////////

void StackPoints::visit ( Minerva::Core::Layers::PointLayer &layer )
{
  if ( layer.stackPoints() && layer.showLayer() )
    layer.traverse ( *this );
}
