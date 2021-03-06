
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wand matrix functor.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Functors/Interaction/Wand/WandMatrix.h"
#include "Usul/Trace/Trace.h"

using namespace Usul::Functors::Interaction::Wand;

USUL_IMPLEMENT_TYPE_ID ( WandMatrix );


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructors.
//
///////////////////////////////////////////////////////////////////////////////

WandMatrix::WandMatrix ( Unknown *caller, const std::string &name ) : BaseClass ( caller, name ),
  _ws ( caller )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructors.
//
///////////////////////////////////////////////////////////////////////////////

WandMatrix::WandMatrix ( const WandMatrix &m ) : BaseClass ( m ),
  _ws ( m._ws )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructors.
//
///////////////////////////////////////////////////////////////////////////////

WandMatrix::~WandMatrix()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the wand's matrix.
//
///////////////////////////////////////////////////////////////////////////////

void WandMatrix::operator()()
{
  USUL_TRACE_SCOPE;

  WandState *state ( this->_wandState() );
  if ( 0x0 != state )
  {
    Guard guard ( this->mutex() );
    state->wandMatrix ( _matrix );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the wand's state.
//
///////////////////////////////////////////////////////////////////////////////

const WandMatrix::WandState *WandMatrix::_wandState() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _ws.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the wand's state.
//
///////////////////////////////////////////////////////////////////////////////

WandMatrix::WandState *WandMatrix::_wandState()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _ws.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the caller.
//
///////////////////////////////////////////////////////////////////////////////

void WandMatrix::caller ( Unknown* caller )
{
  USUL_TRACE_SCOPE;

  // Call the base class.
  BaseClass::caller ( caller );

  // Set our internal query pointer to caller.
  Guard guard ( this->mutex() );
  _ws = caller;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Functors::Interaction::Common::BaseFunctor* WandMatrix::clone()
{
  return new WandMatrix ( *this );
}
