
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Visitors/TemporalAnimation.h"

#include "Minerva/Core/DataObjects/DataObject.h"

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

void TemporalAnimation::visit ( Minerva::Core::DataObjects::DataObject &object )
{
  // Have to add one more the end of the date duration.
  // This is because if the duration is 1/1/2006 to 1/31/2006 and the date is 1/31/2006, the contains function will return false.
  Minerva::Core::Animate::Date temp ( object.lastDate () );
  temp.increment();
  boost::gregorian::date_period period ( object.firstDate ().date(), temp.date() );

  bool visible ( _period.intersects ( period ) ? true : false );
  object.visibility ( visible );
}
