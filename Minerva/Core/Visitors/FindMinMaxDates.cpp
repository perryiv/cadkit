
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include "Minerva/Core/Visitors/FindMinMaxDates.h"
#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/TimeSpan.h"
#include "Minerva/Core/Data/TimeStamp.h"

using namespace Minerva::Core::Visitors;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FindMinMaxDates::FindMinMaxDates () :
  BaseClass (),
  _first ( boost::date_time::not_a_date_time ),
  _last ( boost::date_time::not_a_date_time )
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

void FindMinMaxDates::visit ( Minerva::Core::Data::Feature &object )
{
  if ( Minerva::Core::Data::TimeSpan *span = dynamic_cast<Minerva::Core::Data::TimeSpan*> ( object.timePrimitive() ) )
  {
    const Date first ( span->begin() );
    const Date last ( span->end() );

#ifndef _DEBUG
    const std::string s0 ( first.toString() );
    const std::string s1 ( last.toString() );
#endif
    
    this->_updateMin ( first );
    this->_updateMax ( last );
  }
  else if ( Minerva::Core::Data::TimeStamp *stamp = dynamic_cast<Minerva::Core::Data::TimeStamp*> ( object.timePrimitive() ) )
  {
    const Date date ( stamp->when() );
    
    this->_updateMin ( date );
    this->_updateMax ( date );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update min date.
//
///////////////////////////////////////////////////////////////////////////////

void FindMinMaxDates::_updateMin ( const Date& date )
{
  if ( Date ( boost::date_time::not_a_date_time ) == _first || date < _first )
    _first = date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update max date.
//
///////////////////////////////////////////////////////////////////////////////

void FindMinMaxDates::_updateMax ( const Date& date )
{
  if ( Date ( boost::date_time::not_a_date_time ) == _last || date > _last )
    _last = date;
}
