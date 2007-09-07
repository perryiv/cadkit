
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for functors.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Functors/Interaction/Common/BaseFunctor.h"
#include "Usul/Trace/Trace.h"

using namespace Usul::Functors::Interaction::Common;

USUL_IMPLEMENT_TYPE_ID ( BaseFunctor );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseFunctor::BaseFunctor ( Unknown *caller, const std::string &name ) : BaseClass(),
  _caller ( caller )
{
  USUL_TRACE_SCOPE;
  this->name ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseFunctor::BaseFunctor ( const BaseFunctor &cb ) : BaseClass ( cb ),
  _caller ( cb._caller )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseFunctor::~BaseFunctor()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the unknown pointer.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Interfaces::IUnknown *BaseFunctor::caller() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _caller.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the unknown pointer.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *BaseFunctor::caller()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _caller.get();
}
