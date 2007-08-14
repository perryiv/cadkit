
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "PrevTimestep.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IDldNavigator.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PrevTimestep::PrevTimestep ( Usul::Interfaces::IUnknown * caller ) :
  BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "Previous Year" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PrevTimestep::~PrevTimestep ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void PrevTimestep::_execute ()
{
  USUL_TRACE_SCOPE;
  //this->_dummy();
#if 1

  Usul::Interfaces::IDldNavigator::QueryPtr nav ( this->caller() );
  
  if ( nav.valid () )
  {
    bool value = nav->decrementFilePosition();
    /*if( true == value )
    {
      nav->loadCurrentFile( true );
    }*/
  }
#endif
}

void PrevTimestep::_dummy ()
{
  USUL_TRACE_SCOPE;
  std::cout << "In Dummy" << std::endl;
}
