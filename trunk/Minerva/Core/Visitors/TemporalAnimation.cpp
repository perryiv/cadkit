
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Visitors/TemporalAnimation.h"

#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/TimeSpan.h"

using namespace Minerva::Core::Visitors;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TemporalAnimation::TemporalAnimation ( const Date& first, const Date& last ) : BaseClass (),
  _period( first.date(), last.date() )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TemporalAnimation::~TemporalAnimation ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit.
//
///////////////////////////////////////////////////////////////////////////////

void TemporalAnimation::visit ( Minerva::Core::Data::DataObject &object )
{
  if ( Minerva::Core::Data::TimeSpan *span = dynamic_cast<Minerva::Core::Data::TimeSpan*> ( object.timePrimitive() ) )
  {
    // This is [first,last), so for proper animation, make the object's last date one day past the actual last date.
    boost::gregorian::date_period period ( span->begin().date(), span->end().date() );
    
    const bool visible ( _period.intersects ( period ) ? true : false );
    object.visibility ( visible );
  }
}
