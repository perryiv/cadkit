
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Translate functors.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Functors/Interaction/Navigate/Translate.h"
#include "Usul/Trace/Trace.h"

using namespace Usul::Functors::Interaction::Navigate;

USUL_IMPLEMENT_TYPE_ID ( Translate );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Translate::Translate ( 
  Unknown *caller, 
  const std::string &name,
  Direction *dir, 
  AnalogInput *ai,
  double speed ) : 
  BaseClass ( caller, name, dir, ai, speed ),
  _translateSpeed ( caller )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Translate::Translate ( const Translate &cb ) : 
  BaseClass ( cb ),
  _translateSpeed ( cb._translateSpeed )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Translate::~Translate()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate.
//
///////////////////////////////////////////////////////////////////////////////

void Translate::operator()()
{
  USUL_TRACE_SCOPE;

  // The stick value is just a unit-less scalar in the range [-1,1].
  const double stick ( this->_analog() ); // no units

  // We need the world's radius in order to calculate the "relative speed".
  const double radius ( this->_worldRadius() ); // feet

  // This is the number of seconds since the last time we were here.
  const double interval ( this->_frameTime() ); // seconds

  // Get the current speed.
  const double speed ( this->currentSpeed() );

  // The distance to translate should be in units of feet.
  //
  //         feet  no-units  no-units  seconds
  // feet =  ----  --------  --------  -------
  //          1    no-units  seconds      1
  //
  const double dist ( radius * stick * - speed * interval ); // feet

  // Get the direction vector.
  Direction::Vector dir ( this->_direction() );

  // Scale by the distance.
  dir *= dist;

  // Translate in this direction.
  Matrix T;
  T.makeTranslation ( dir );
  this->_postMult ( T );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximum relative speed. The units are 
//  percentage-of-world-radius / seconds.
//
///////////////////////////////////////////////////////////////////////////////

void Translate::maxRelativeSpeed ( double s )
{
  USUL_TRACE_SCOPE;
  this->speed ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current speed.
//
///////////////////////////////////////////////////////////////////////////////

double Translate::currentSpeed() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( true == _translateSpeed.valid() ) ? _translateSpeed->translationSpeed () : this->speed() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the caller.
//
///////////////////////////////////////////////////////////////////////////////

void Translate::caller ( Unknown* caller )
{
  USUL_TRACE_SCOPE;

  // Call the base class.
  BaseClass::caller ( caller );

  // Set our internal query pointer to caller.
  Guard guard ( this->mutex() );
  _translateSpeed = caller;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Functors::Interaction::Common::BaseFunctor* Translate::clone()
{
  return new Translate ( *this );
}
