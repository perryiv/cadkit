
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "LoadNextTimestep.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IDldNavigator.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadNextTimestep::LoadNextTimestep ( Usul::Interfaces::IUnknown * caller ) :
  BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "Load Next Year" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadNextTimestep::~LoadNextTimestep ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void LoadNextTimestep::_execute ()
{
  USUL_TRACE_SCOPE;
  //this->_dummy();
#if 1

  Usul::Interfaces::IDldNavigator::QueryPtr nav ( this->caller() );
  if ( nav.valid () )
  {
    bool value = nav->incrementFilePosition();
    if( true == value )
    {
      nav->loadCurrentFile( true );
    }
  }
#endif
}

void LoadNextTimestep::_dummy ()
{
  USUL_TRACE_SCOPE;
  std::cout << "In Dummy" << std::endl;
}
