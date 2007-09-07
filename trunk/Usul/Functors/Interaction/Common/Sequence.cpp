
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor sequence.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Functors/Interaction/Common/Sequence.h"
#include "Usul/Trace/Trace.h"

using namespace Usul::Functors::Interaction::Common;

USUL_IMPLEMENT_TYPE_ID ( Sequence );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Sequence::Sequence ( Unknown *caller, const std::string &name ) : 
  BaseClass ( caller, name ),
  _functors ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Sequence::Sequence ( const Sequence &s ) : BaseClass ( s ),
  _functors ( s._functors )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Sequence::~Sequence()
{
  USUL_TRACE_SCOPE;
  _functors.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Have the functors do their thing.
//
///////////////////////////////////////////////////////////////////////////////

void Sequence::operator()()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  for ( Functors::iterator i = _functors.begin(); i != _functors.end(); ++i )
  {
    BaseFunctor::RefPtr functor ( i->get() );
    if ( true == functor.valid() )
    {
      (*functor)();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append a functor.
//
///////////////////////////////////////////////////////////////////////////////

void Sequence::append ( BaseClass *functor )
{
  USUL_TRACE_SCOPE;
  if ( 0x0 != functor )
  {
    Guard guard ( this->mutex() );
    _functors.push_back ( BaseFunctor::RefPtr ( functor ) );
  }
}
