
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Animate/RecordedPath.h"

#include "Usul/Interfaces/IViewMatrix.h"

using namespace VRV::Animate;

//USUL_FACTORY_REGISTER_CREATOR_WITH_NAME ( "MatrixPath", Path );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RecordedPath::RecordedPath () : 
  BaseClass (),
  _frames (),
  _current ()
{
  this->_addMember ( "Frames", _frames );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RecordedPath::~RecordedPath ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append a frame.
//
///////////////////////////////////////////////////////////////////////////////

void RecordedPath::_append ( Frame* frame )
{
  {
    Guard guard ( this->mutex () );
    _frames.push_back ( frame );
  }
  this->dirty ( true );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation from beginning.
//
///////////////////////////////////////////////////////////////////////////////

void RecordedPath::start ( Usul::Interfaces::IUnknown * caller )
{
  if ( false == _frames.empty () )
    _current = _frames.begin ();
  else
    _current == _frames.end ();

  BaseClass::start ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate one step.
//
///////////////////////////////////////////////////////////////////////////////

void RecordedPath::animate ( Usul::Interfaces::IUnknown * caller )
{
  if ( false == this->animating () )
    return;

  Usul::Interfaces::IViewMatrix::QueryPtr vm ( caller );

  if ( vm.valid () )
  {
    Guard guard ( this->mutex () );
    if ( _current != _frames.end () )
    {
      vm->setViewMatrix ( (*_current)->matrix () );
      ++_current;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Clear.
//
///////////////////////////////////////////////////////////////////////////////

void RecordedPath::clear ()
{
  {
    Guard guard ( this->mutex () );
    _frames.clear();
  }
  this->dirty ( true );
}

