
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Animate/Settings.h"

using namespace OsgTools::Animate;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Settings() : 
BaseClass(),
_firstDate( boost::date_time::min_date_time ),
_lastDate( boost::date_time::max_date_time ),
_animate( false ),
_showPastDays( true ),
_timeWindow ( false ),
_windowLength ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Settings::~Settings()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the min date.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::firstDate( const OsgTools::Animate::Date& date )
{
  _firstDate = date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the min date.
//
///////////////////////////////////////////////////////////////////////////////

const OsgTools::Animate::Date& Settings::firstDate() const
{
  return _firstDate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the max date.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::lastDate( const OsgTools::Animate::Date& date )
{
  _lastDate = date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the max date.
//
///////////////////////////////////////////////////////////////////////////////

const OsgTools::Animate::Date& Settings::lastDate() const
{
  return _lastDate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set show past days flag.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::showPastDays( bool b )
{
  _showPastDays = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should past days be shown?
//
///////////////////////////////////////////////////////////////////////////////

bool Settings::showPastDays() const
{
  return _showPastDays;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the time window flag.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::timeWindow( bool b )
{
  _timeWindow = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should there be a time window?
//
///////////////////////////////////////////////////////////////////////////////

bool Settings::timeWindow() const
{
  return _timeWindow;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the time window length.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::windowLength ( unsigned int length )
{
  _windowLength = length;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the time window length.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Settings::windowLength () const
{
  return _windowLength;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the animate flag.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::animate( bool b )
{
  _animate = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the animate flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Settings::animate() const
{
  return _animate;
}
