
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/ModelPresentation/ModelPresentation/MpdMenuCommand.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IMpdNavigator.h"

USUL_IMPLEMENT_COMMAND ( MpdMenuCommand );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdMenuCommand::MpdMenuCommand ( Usul::Interfaces::IUnknown * caller, const std::string text, unsigned int set, unsigned int group ) :
  BaseClass ( caller ),
  _text ( text ),
  _set ( set ),
  _group( group )
{
  USUL_TRACE_SCOPE;
  this->text ( text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdMenuCommand::~MpdMenuCommand ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void MpdMenuCommand::_execute ()
{
  USUL_TRACE_SCOPE;
  
#if 1
  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( this->caller().get() );

  if ( nav.valid () )
  {
    nav->setGroup( _set, _group );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the radio button.  Return true to have radio checked.
//
///////////////////////////////////////////////////////////////////////////////

bool MpdMenuCommand::updateCheck () const
{
  USUL_TRACE_SCOPE;

  // Implement me to update radio state.
  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( const_cast < Usul::Interfaces::IUnknown * > ( this->caller().get() ) );

  unsigned int currentGroup = nav->getCurrentGroupFromSet( _set );
  if( currentGroup == _group )
    return true;
  else
    return false;
}

