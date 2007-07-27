
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

#include "Translate.h"

using namespace VRV::Functors;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Translate::Translate ( 
  Unknown *unknown, 
  Direction *dir, 
  AnalogInput *ai,
  float speed,
  unsigned int id ) : 
  BaseClass ( unknown, dir, ai, id ),
  _translateSpeed ( unknown ),
  _speed ( speed )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Translate::Translate ( const Translate &cb ) : BaseClass ( cb ),
  _speed ( cb._speed )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Translate::~Translate()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate.
//
///////////////////////////////////////////////////////////////////////////////

void Translate::operator()()
{
  // The stick value is just a unit-less scalar in the range [-1,1].
  float stick ( this->_analog() ); // no units

  // We need the world's radius in order to calculate the "relative speed".
  float radius ( this->_worldRadius() ); // feet

  // This is the number of seconds since the last time we were here.
  float interval ( this->_frameTime() ); // seconds

  // Get the speed.
  float speed ( _translateSpeed.valid() ? _translateSpeed->translationSpeed () : _speed );

  // The distance to translate should be in units of feet.
  //
  //         feet  no-units  no-units  seconds
  // feet =  ----  --------  --------  -------
  //          1    no-units  seconds      1
  //
  float dist ( radius * stick * - speed * interval ); // feet

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

void Translate::maxRelativeSpeed ( float s )
{
  _speed = s;
}
