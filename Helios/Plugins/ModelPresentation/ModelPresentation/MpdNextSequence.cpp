
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/ModelPresentation/ModelPresentation/MpdNextSequence.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IMpdNavigator.h"

USUL_IMPLEMENT_COMMAND ( MpdNextSequence );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdNextSequence::MpdNextSequence ( Usul::Interfaces::IUnknown * caller ) :
  BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "Next" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdNextSequence::~MpdNextSequence ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void MpdNextSequence::_execute ()
{
  USUL_TRACE_SCOPE;
  //this->_dummy();
#if 1
  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( this->caller() );
  if ( nav.valid () )
  {
    nav->nextSequenceStep();
  }
#endif
}
