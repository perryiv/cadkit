
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Visitors/StackPoints.h"
#include "Minerva/Core/Layers/Vector.h"
#include "Minerva/Core/DataObjects/Point.h"

#include "Minerva/Interfaces/IPointData.h"
#include "Minerva/Interfaces/IOffset.h"

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

void StackPoints::visit ( Minerva::Core::DataObjects::Point &point )
{
  Minerva::Interfaces::IPointData::QueryPtr pd ( point.geometry() );
  
  if( pd.valid () )
  {
    Usul::Math::Vec3d p ( pd->pointData() );

    const unsigned int multiplier ( 100000 );
    Usul::Math::Vec3ui center ( static_cast<unsigned int> ( p[0] * multiplier ),
                                static_cast<unsigned int> ( p[1] * multiplier ),
                                /*static_cast<unsigned int> ( p[2] * multiplier )*/ 0 );

    Counts::iterator iter = _counts.find ( center );
    if ( _counts.end() == iter )
    {
      _counts[center] = 1;
    }
    else
    {
      // Make sure it's using the right altitude mode.
      point.altitudeMode ( Minerva::Core::DataObjects::DataObject::RELATIVE_TO_GROUND );
      unsigned int count ( iter->second );
      
      Minerva::Interfaces::IOffset::QueryPtr offset ( point.geometry() );
      
      if( offset.valid () )
      {
        const osg::Vec3& o ( offset->spatialOffset() );
        const double distance ( count * _multiplier * ( point.autotransform() ? 20 : point.size() ) );
        offset->spatialOffset ( osg::Vec3f ( o.x(), o.y(), /*o.z() +*/ ( distance ) ) );
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

void StackPoints::visit ( Minerva::Core::Layers::Vector &layer )
{
  if ( layer.showLayer() )
  {
    layer.traverse ( *this );
    layer.dirtyScene ( true );
  }
}
