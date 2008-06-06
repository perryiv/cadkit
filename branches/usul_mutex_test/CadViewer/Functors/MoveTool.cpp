
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for moving a scene.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "MoveTool.h"

using namespace CV;
using namespace CV::Functors;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MoveTool::MoveTool ( 
  Unknown *unknown, 
  Direction *dir, 
  AnalogInput *ai, 
  float speed,
  const std::string& name ) : 
  BaseClass ( unknown, dir, ai, name ),
  _speed ( speed )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

MoveTool::MoveTool ( const MoveTool &t ) : 
  BaseClass ( t ),
  _speed ( t._speed )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MoveTool::~MoveTool()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move.
//
///////////////////////////////////////////////////////////////////////////////

void MoveTool::operator()()
{
  // The stick value is just a unit-less scalar in the range [-1,1].
  float stick ( this->_analog() ); // no units

  // We need the world's radius in order to calculate the "relative speed".
  float radius ( this->_worldRadius() ); // feet

  // This is the number of seconds since the last time we were here.
  float interval ( this->_frameTime() ); // seconds

  // The distance to translate should be in units of feet.
  //
  //         feet  no-units  no-units  seconds
  // feet =  ----  --------  --------  -------
  //          1    no-units  seconds      1
  //
  float dist ( radius * stick * -_speed * interval ); // feet

  // Get the direction vector.
  Direction::Vector dir ( this->_direction() );

  // Scale by the distance.
  dir *= dist;

  // Translate in this direction.
  Matrix T;
  T.makeTranslation ( dir );
  this->_postMult ( T );
}
