
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008/2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "LibModelManager.h"


#include "osg/Group"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

LibModelManager::LibModelManager() : 
  BaseClass ()
{
  

}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

LibModelManager::~LibModelManager()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void LibModelManager::clear ( Unknown *caller )
{
  
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void LibModelManager::read( const std::string &filename, const std::string &type, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{

}

///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *LibModelManager::buildScene ( Unknown *caller )
{
  
  Guard guard ( this->mutex() );

  osg::ref_ptr< osg::Group > group ( new osg::Group );

  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the mpd file.
//
///////////////////////////////////////////////////////////////////////////////

void LibModelManager::write() const
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the mpd file to filename <filename>.
//
///////////////////////////////////////////////////////////////////////////////

void LibModelManager::write( const std::string &filename ) const
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Update
//
///////////////////////////////////////////////////////////////////////////////

void LibModelManager::updateNotify ( Usul::Interfaces::IUnknown *caller )
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Set to animate or not
//
///////////////////////////////////////////////////////////////////////////////

void LibModelManager::animate( bool value )
{
  Guard guard( this );

  // Set the animation state
  _animateTimeline = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the current animation state
//
///////////////////////////////////////////////////////////////////////////////

bool LibModelManager::animate()
{
  Guard guard( this );

  // Return the animation state
  return _animateTimeline;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increment the current time in the animation sequence for the timeline
//
///////////////////////////////////////////////////////////////////////////////

void LibModelManager::next()
{
  Guard guard( this );
  
  // Increment the time step
  if( _currentTimeStep < _maxTimeSteps )
  {
    ++_currentTimeStep;
  }
  else
  {
    _currentTimeStep = 0;
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Decrement the current time in the animation sequence for the timeline
//
///////////////////////////////////////////////////////////////////////////////

void LibModelManager::prev()
{
  Guard guard( this );

  // Decrement the time step
  if( _currentTimeStep > 0 )
  {
    --_currentTimeStep;
  }
  else
  {
    _currentTimeStep = _maxTimeSteps;
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the current time in the animation seuqence for the timeline
//
///////////////////////////////////////////////////////////////////////////////

void LibModelManager::setTime( Usul::Types::Uint64 time )
{
  Guard guard( this );

  // Make sure the time passed in is within an acceptable range
  if( time <= _maxTimeSteps && time >= 0 )
  {
    _currentTimeStep = time;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Add a group to the timeline collection
//
///////////////////////////////////////////////////////////////////////////////

void LibModelManager::addToTimeline( const std::string& name, Timeline::RefPtr value )
{
  // Guard
  Guard guard( this );

  // add to the map
  _timelines[ name ].push_back( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a timeline by name
//
///////////////////////////////////////////////////////////////////////////////

LibModelManager::Timelines LibModelManager::getTimelineByName( const std::string& name )
{
  // Guard
  Guard guard( this );

  // Temp timeline
  Timelines timeline ( _timelines[ name ] );

  // Return
  return timeline;
}

