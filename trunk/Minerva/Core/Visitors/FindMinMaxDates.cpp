
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Visitors/FindMinMaxDates.h"
#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/TimeSpan.h"

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

void FindMinMaxDates::visit ( Minerva::Core::Data::DataObject &object )
{
  if ( Minerva::Core::Data::TimeSpan *span = dynamic_cast<Minerva::Core::Data::TimeSpan*> ( object.timePrimitive() ) )
  {
    const Date first ( span->begin() );
    const Date last ( span->end() );

    const Date min ( boost::date_time::min_date_time );
    const Date max ( boost::date_time::max_date_time );
    
    // These are human readable in the debugger...
  #if _DEBUG
    const std::string sFirst ( first.toString() );
    const std::string sLast ( last.toString() );
    
    const std::string sMin ( min.toString() );
    const std::string sMax ( max.toString() );
  #endif
    
    const bool valid ( min != first && max != last );
    
    // If the dates are valid.
    if ( true == valid )
    {
      if ( min == _first || first < _first )
        _first = first;

      if ( max == _last || last > _last )
        _last = last;
    }
  }
}
