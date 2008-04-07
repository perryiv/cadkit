
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "MpdPrevSequence.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IMpdNavigator.h"

USUL_IMPLEMENT_COMMAND ( MpdPrevSequence );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdPrevSequence::MpdPrevSequence ( Usul::Interfaces::IUnknown * caller ) :
  BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "Prev" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdPrevSequence::~MpdPrevSequence ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void MpdPrevSequence::_execute ()
{
  USUL_TRACE_SCOPE;
#if 1
  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( this->caller() );
  if ( nav.valid () )
  {
    nav->prevSequenceStep();
  }
#endif
}