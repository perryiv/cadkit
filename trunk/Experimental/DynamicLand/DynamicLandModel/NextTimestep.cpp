
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "NextTimestep.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IDldNavigator.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

NextTimestep::NextTimestep ( Usul::Interfaces::IUnknown * caller ) :
  BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "Next Timestep 2" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

NextTimestep::~NextTimestep ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void NextTimestep::_execute ()
{
  USUL_TRACE_SCOPE;
  //this->_dummy();
#if 1
  std::cerr << "Trace Call" << std::endl;
  std::cerr << "After Trace, Before QueryPtr Call" << std::endl;
  Usul::Interfaces::IDldNavigator::QueryPtr nav ( this->caller() );
  std::cerr << "After QueryPtr Call" << std::endl;
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

void NextTimestep::_dummy ()
{
  USUL_TRACE_SCOPE;
  std::cout << "In Dummy" << std::endl;
}
