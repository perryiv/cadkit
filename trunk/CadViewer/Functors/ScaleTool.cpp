
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for scaling a scene.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "ScaleTool.h"

using namespace CV;
using namespace CV::Functors;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ScaleTool::ScaleTool ( 
  Unknown *unknown, 
  Direction *dir,
  AnalogInput *ai, 
  const Vector &scale, 
  float speed,
  const std::string& name ) : 
  BaseClass ( unknown, dir, ai, name ),
  _scale ( scale ),
  _speed ( speed )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

ScaleTool::ScaleTool ( const ScaleTool &t ) : 
  BaseClass ( t ),
  _scale ( t._scale ),
  _speed ( t._speed )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ScaleTool::~ScaleTool()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scale.
//
///////////////////////////////////////////////////////////////////////////////

void ScaleTool::operator()()
{
  // The stick value is just a unit-less scalar in the range [-1,1].
  float stick ( this->_analog() ); // no units

  // This is the number of seconds since the last time we were here.
  float interval ( this->_frameTime() ); // seconds

  // This maps the joystick neutral position to 1, and the max position to 
  // "interval * speed". The speed is the maximum scale per second.
  float s ( 1 - stick * ( 1 - _speed ) * interval );

  // Get the vector.
  Vector scale ( _scale );

  // Scale by the amount.
  scale *= s;

  // Scale by this vector.
  Matrix M;
  M.makeScale ( scale );
  this->_preMult ( M );
}