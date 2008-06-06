
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Rotate functors.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Functors/Interaction/Navigate/Rotate.h"
#include "Usul/Math/Constants.h"
#include "Usul/Trace/Trace.h"

using namespace Usul::Functors::Interaction::Navigate;

USUL_IMPLEMENT_TYPE_ID ( Rotate );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Rotate::Rotate ( 
  Unknown *caller, 
  const std::string &name,
  Direction *dir, 
  AnalogInput *ai,
  double speed ) : 
  BaseClass ( caller, name, dir, ai, speed )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Rotate::Rotate ( const Rotate &cb ) : BaseClass ( cb )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Rotate::~Rotate()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rotate.
//
///////////////////////////////////////////////////////////////////////////////

void Rotate::operator()()
{
  USUL_TRACE_SCOPE;

  // The stick value is just a unit-less scalar in the range [-1,1].
  const double stick ( this->_analog() ); // no units

  // This is the number of seconds since the last time we were here.
  const double interval ( this->_frameTime() ); // seconds

  // Get the speed.
  const double speed ( this->maxAngularSpeed() );

  // The angle to rotate should be in units of radians.
  //
  //           no-units  degrees  seconds  radians
  // radians = --------  -------  -------  -------
  //           no-units  seconds     1     degrees
  //
  const double radians = stick * speed * interval * Usul::Math::DEG_TO_RAD; // radians

  // Get the direction vector.
  const Direction::Vector axis ( this->_direction() );

  // Rotate about the axis. Note: post-multiplying will rotate about where 
  // the wand is at. Pre-multiplying will rotate about the global origin.
  Matrix R;
  R.makeRotation ( radians, axis );
  this->_postMult ( R );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximum degrees/second.
//
///////////////////////////////////////////////////////////////////////////////

void Rotate::maxAngularSpeed ( double s )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  this->speed ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Functors::Interaction::Common::BaseFunctor* Rotate::clone()
{
  return new Rotate ( *this );
}
