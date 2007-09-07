
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Visitors/FindMinMaxDates.h"
#include "Minerva/Core/DataObjects/PointTime.h"

using namespace Minerva::Core::Visitors;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FindMinMaxDates::FindMinMaxDates () :
  BaseClass (),
  _first ( boost::date_time::min_date_time ),
  _last ( boost::date_time::max_date_time )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FindMinMaxDates::~FindMinMaxDates()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update min/max dates.
//
///////////////////////////////////////////////////////////////////////////////

void FindMinMaxDates::visit ( Minerva::Core::DataObjects::PointTime &pointTime )
{
  Date first ( pointTime.firstDate () );
  Date last ( pointTime.lastDate () );

  const Date min ( boost::date_time::min_date_time );
  const Date max ( boost::date_time::max_date_time );

  if ( min == _first || first < _first )
    _first = first;

  if ( max == _last || last > _last )
    _last = last;
}
