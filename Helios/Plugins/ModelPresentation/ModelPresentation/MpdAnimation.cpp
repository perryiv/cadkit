
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/ModelPresentation/ModelPresentation/MpdAnimation.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IMpdNavigator.h"

USUL_IMPLEMENT_COMMAND ( MpdAnimation );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdAnimation::MpdAnimation ( Usul::Interfaces::IUnknown * caller ) :
  BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "Animate" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdAnimation::~MpdAnimation ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void MpdAnimation::_execute ()
{
  USUL_TRACE_SCOPE;
  //this->_dummy();
#if 1
  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( this->caller().get() );
  if ( nav.valid () )
  {
    if( true == nav->animate() )
    {
      nav->animate( false );
    }
    else
    {
      nav->animate( true );
    }
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the toggle button.  Return true to have radio checked.
//
///////////////////////////////////////////////////////////////////////////////

bool MpdAnimation::updateCheck () const
{
  USUL_TRACE_SCOPE;

  // Implement me to update radio state.
  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( const_cast < Usul::Interfaces::IUnknown * > ( this->caller().get() ) );

  if ( nav.valid () )
  {
    return nav->animate();
  }
  else
    return false;
}
