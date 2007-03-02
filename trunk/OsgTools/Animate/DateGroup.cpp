
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Animate/DateGroup.h"

#include <algorithm>

using namespace OsgTools::Animate;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DateGroup::DateGroup() : BaseClass(),
_animationSpeed ( 0.0 ),
_lastTime ( -1.0 ),
_lastDate (),
_minDate(boost::date_time::max_date_time),
_maxDate(boost::date_time::min_date_time),
_settings( new OsgTools::Animate::Settings )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DateGroup::~DateGroup()
{
  if ( _text.valid() )
  {
    _text->setText( "" );
    _text->update();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update min max dates..
//
///////////////////////////////////////////////////////////////////////////////

void DateGroup::updateMinMax ( const OsgTools::Animate::Date& first, const OsgTools::Animate::Date& last )
{
  if( first < _minDate )
    _minDate = first;
  if( last > _maxDate )
    _maxDate = last;

  _lastDate = _minDate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse the node.
//
///////////////////////////////////////////////////////////////////////////////

void DateGroup::traverse( osg::NodeVisitor& nv )
{
  if ( nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR )
  {
    if( this->settings()->animate() )
    {
      const osg::FrameStamp* framestamp = nv.getFrameStamp();
      if ( framestamp )
      {
        double t = framestamp->getReferenceTime();

        if( _lastTime == -1 )
          _lastTime = t;

        double duration ( t - _lastTime );

        if ( duration > _animationSpeed )
        {
          _lastDate.increment();
          _lastTime = t;
        }

        if( _lastDate > _maxDate )
        {
          _lastDate = _minDate;
        }

        OsgTools::Animate::Date firstDate = _lastDate;

        if ( this->settings()->showPastDays() )
        {
          firstDate = _minDate;
        }

        if( this->settings()->timeWindow() )
        {
          firstDate = _lastDate;
          firstDate.moveBackNumDays( this->settings()->windowLength() );
        }

        this->settings()->firstDate( firstDate );
        this->settings()->lastDate( _lastDate );

        if( _text.valid() )
        {
          _text->setText( _lastDate.toString() );
          _text->update();
        }
      }
    }
    else
    {
      this->settings()->firstDate( _minDate );
      this->settings()->lastDate( _maxDate );
    }
    
  }
  Group::traverse(nv);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the timestep duration.
//
///////////////////////////////////////////////////////////////////////////////

void DateGroup::animationSpeed( float speed )
{
  _animationSpeed = speed;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the timestep duration.
//
///////////////////////////////////////////////////////////////////////////////

float DateGroup::animationSpeed( ) const
{
  return _animationSpeed;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text.
//
///////////////////////////////////////////////////////////////////////////////

void DateGroup::setText ( osgText::Text *text )
{
  _text = text;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the settings.
//
///////////////////////////////////////////////////////////////////////////////

void DateGroup::settings( OsgTools::Animate::Settings* settings )
{
  _settings = settings;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the settings.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Animate::Settings* DateGroup::settings()
{
  return _settings.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the settings.
//
///////////////////////////////////////////////////////////////////////////////

const OsgTools::Animate::Settings* DateGroup::settings() const
{
  return _settings.get();
}

