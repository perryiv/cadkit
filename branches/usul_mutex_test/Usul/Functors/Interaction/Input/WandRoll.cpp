
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor class for wand-pitch input.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Functors/Interaction/Input/WandRoll.h"
#include "Usul/Math/Constants.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Trace/Trace.h"

using namespace Usul::Functors::Interaction::Input;

USUL_IMPLEMENT_TYPE_ID ( WandRoll );


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

WandRoll::WandRoll ( Unknown *caller, const std::string &name ) : 
  BaseClass ( caller, name )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

WandRoll::WandRoll ( const WandRoll &wp ) : BaseClass ( wp )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WandRoll::~WandRoll()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertical analog input.
//
///////////////////////////////////////////////////////////////////////////////

void WandRoll::operator()()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Vector dir ( 0.0f, 1.0f, 0.0f );
  _value = this->_calculateValue ( 2, 0, dir );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Functors::Interaction::Common::BaseFunctor* WandRoll::clone()
{
  return new WandRoll ( *this );
}