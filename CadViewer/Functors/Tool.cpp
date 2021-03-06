
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all tools.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Tool.h"

#include "OsgTools/Convert.h"

using namespace CV;
using namespace CV::Functors;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Tool::Tool ( Unknown *unknown, Direction *di, AnalogInput *ai, const std::string& name ) : 
  BaseClass ( unknown, name ),
  _di  ( di ),
  _dir ( 1, 0, 0 ),
  _ai  ( ai ),
  _fi  ( unknown ),
  _wi  ( unknown ),
  _mt  ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Tool::Tool ( const Tool &t ) : 
  BaseClass ( t ),
  _ai  ( t._ai ),
  _fi  ( t._fi ),
  _wi  ( t._wi ),
  _mt  ( t._mt )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Tool::~Tool()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the internal transforms.
//
///////////////////////////////////////////////////////////////////////////////

Tool::Transforms &Tool::transforms()
{
  return _mt;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the internal transforms.
//
///////////////////////////////////////////////////////////////////////////////

const Tool::Transforms &Tool::transforms() const
{
  return _mt;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the internal transforms.
//
///////////////////////////////////////////////////////////////////////////////

void Tool::transforms ( const Tool::Transforms &mt )
{
  _mt = mt;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update and return the analog input.
//
///////////////////////////////////////////////////////////////////////////////

float Tool::_analog()
{
  (*_ai)();
  return _ai->value();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update and return the direction.
//
///////////////////////////////////////////////////////////////////////////////

const Tool::Direction::Vector &Tool::_direction()
{
  if ( _di.valid() )
  {
    (*_di)();
    _dir = _di->vector();
  }
  return _dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the amount of time the last frame took.
//
///////////////////////////////////////////////////////////////////////////////

float Tool::_frameTime() const
{
  return _fi->frameTime();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the radius of the "world".
//
///////////////////////////////////////////////////////////////////////////////

float Tool::_worldRadius() const
{
  return _wi->worldRadius();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post multiply the current matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Tool::_postMult ( const Matrix &M )
{
  // Convert to osg's matrix type.
  osg::Matrixf mat;
  OsgTools::Convert::matrix ( M, mat );

  // Loop through all the transforms.
  for ( Transforms::iterator i = _mt.begin(); i != _mt.end(); ++i )
  {
    if ( i->get() )
      i->get()->setMatrix ( i->get()->getMatrix() * mat );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post multiply the current matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Tool::_preMult ( const Matrix &M )
{
  // Convert to osg's matrix type.
  osg::Matrixf mat;
  OsgTools::Convert::matrix ( M, mat );

  // Loop through all the transforms.
  for ( Transforms::iterator i = _mt.begin(); i != _mt.end(); ++i )
  {
    if ( i->get() )
      i->get()->setMatrix ( mat * i->get()->getMatrix() );
  }
}
