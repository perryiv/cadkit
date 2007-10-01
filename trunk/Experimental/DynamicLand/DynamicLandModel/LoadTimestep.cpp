
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "LoadTimestep.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IDldNavigator.h"

USUL_IMPLEMENT_COMMAND ( LoadTimestep );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadTimestep::LoadTimestep ( Usul::Interfaces::IUnknown * caller ) :
  BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "Load Current Year" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadTimestep::~LoadTimestep ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void LoadTimestep::_execute ()
{
  USUL_TRACE_SCOPE;
  //this->_dummy();
#if 1

  Usul::Interfaces::IDldNavigator::QueryPtr nav ( this->caller() );

  if ( nav.valid () )
  {
    nav->loadCurrentFile( true );
  }
  
#endif
}

void LoadTimestep::_dummy ()
{
  USUL_TRACE_SCOPE;
  std::cout << "In Dummy" << std::endl;
}
