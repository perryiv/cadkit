
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
_nodes(),
_speed ( 0.0 ),
_lastTime ( -1.0 ),
_lastDate (),
_minDate("2100-1-1"),
_maxDate("1900-1-1"),
_accumulate( true ),
_timeWindow ( false ),
_needToSort ( false ),
_numDays ( 0 )
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
//  Add a date.
//
///////////////////////////////////////////////////////////////////////////////

void DateGroup::addDate ( const OsgTools::Animate::Date& date, osg::Node* node )
{
  if( date < _minDate )
    _minDate = date;
  if( date > _maxDate )
    _maxDate = date;

  _lastDate = _minDate;

  _nodes.push_back ( DateNodes::value_type( date, node ) );
  _needToSort = true;

  //node->setCullingActive ( false );
  node->dirtyBound();

  this->addChild( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functor for sorting.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > struct SortDateNodes
  {
    bool operator () ( const T &a, const T &b )
    {
      return ( a.first < b.first );
    }
  };
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
    if ( _needToSort )
    {
      std::sort ( _nodes.begin(), _nodes.end(), Helper::SortDateNodes<DateNodes::value_type>() );
      _needToSort = false;
    }

    const osg::FrameStamp* framestamp = nv.getFrameStamp();
    if (framestamp)
    {
      double t = framestamp->getReferenceTime();

      if( _lastTime == -1 )
        _lastTime = t;

      double duration ( t - _lastTime );

      if ( duration > _speed )
      {
        _lastDate.increment();
        _lastTime = t;
      }

      if( _lastDate > _maxDate )
      {
        _lastDate = _minDate;
      }

      OsgTools::Animate::Date firstDate = _lastDate;

      if ( _accumulate )
      {
        firstDate = _minDate;
      }

      if( _timeWindow )
      {
        firstDate = _lastDate;
        firstDate.moveBackNumDays( _numDays );
      }

      const DateNodes::iterator firstIter ( std::lower_bound ( _nodes.begin(), _nodes.end(), DatePair ( firstDate, 0x0 ), Helper::SortDateNodes<DateNodes::value_type>() ) );
      const DateNodes::iterator lastIter  ( std::lower_bound ( firstIter,      _nodes.end(), DatePair ( _lastDate, 0x0 ), Helper::SortDateNodes<DateNodes::value_type>() ) );

      for ( DateNodes::iterator i = firstIter; i < lastIter; ++i )
      {
        i->second->accept(nv);
      }

      if( _text.valid() )
      {
        _text->setText( _lastDate.toString() );
        _text->update();
      }

      //Group::traverse(nv);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the timestep duration.
//
///////////////////////////////////////////////////////////////////////////////

void DateGroup::setDuration( float speed )
{
  _speed = speed;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Should we show past cases?
//
///////////////////////////////////////////////////////////////////////////////

void DateGroup::accumulate ( bool b ) 
{ 
  _accumulate = b; 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should we use a time window?
//
///////////////////////////////////////////////////////////////////////////////

void DateGroup::timeWindow ( bool b ) 
{ 
  _timeWindow = b; 
}


///////////////////////////////////////////////////////////////////////////////
//
//  How big should the time window be?
//
///////////////////////////////////////////////////////////////////////////////

void DateGroup::numDays ( unsigned int num )
{
  _numDays = num;
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
