
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "State.h"


using namespace Usul::Devices;


State::State( ) : 
_buttons (),
_x( 0.0 ),
_y( 0.0 ),
_z( 0.0 ),
_numClicks( 0 ),
_rootX( 0 ),
_rootY( 0 )
{
}


State::State( float x, float y, float z ) : 
_buttons (),
_x( x ),
_y( y ),
_z( z ),
_numClicks( 0 ),
_rootX( 0 ),
_rootY( 0 )
{
}


bool State::buttonPressed ( unsigned int button ) const
{
  //Const cast voodoo.
  State *me = const_cast < State* > ( this );

  return me->_buttons[button];
}

void State::buttonPressed ( unsigned int button, bool pressed )
{
  _buttons[button] = pressed;
}


void State::x( float x )
{
  _x = x;
}

float State::x() const
{
  return _x;
}


void State::y( float y )
{
  _y = y;
}

float State::y() const
{
  return _y;
}

void State::z( float z )
{
  _z = z;
}

float State::z() const
{
  return _z;
}

void State::numClicks ( unsigned int num )
{
  _numClicks = num;
}

unsigned int State::numClicks() const
{
  return _numClicks;
}

void State::rootX ( unsigned int x )
{
  _rootX = x;
}

unsigned int State::rootX() const
{
  return _rootX;
}

void State::rootY ( unsigned int y )
{
  _rootY = y;
}

unsigned int State::rootY() const
{
  return _rootY;
}
