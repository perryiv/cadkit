
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/ModelPresentation/ModelPresentation/MpdFirstSequence.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IMpdNavigator.h"

USUL_IMPLEMENT_COMMAND ( MpdFirstSequence );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdFirstSequence::MpdFirstSequence ( Usul::Interfaces::IUnknown * caller ) :
  BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "First" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdFirstSequence::~MpdFirstSequence ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void MpdFirstSequence::_execute ()
{
  USUL_TRACE_SCOPE;
#if 1
  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( this->caller() );
  if ( nav.valid () )
  {
    nav->firstSequenceStep();
  }
#endif
}


