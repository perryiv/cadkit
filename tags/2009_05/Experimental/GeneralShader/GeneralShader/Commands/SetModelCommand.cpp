
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "SetModelCommand.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IGeneralShader.h"

USUL_IMPLEMENT_COMMAND ( SetModelCommand );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SetModelCommand::SetModelCommand ( Usul::Interfaces::IUnknown * caller, const std::string name ) :
  BaseClass ( caller ),
  _name ( name )
{
  USUL_TRACE_SCOPE;
  this->text ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SetModelCommand::~SetModelCommand ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void SetModelCommand::_execute ()
{
  USUL_TRACE_SCOPE;
  
#if 1
  Usul::Interfaces::IGeneralShader::QueryPtr nav ( this->caller() );

  if ( nav.valid () )
  {
    nav->setModel( _name );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the radio button.  Return true to have radio checked.
//
///////////////////////////////////////////////////////////////////////////////

bool SetModelCommand::updateCheck () const
{
  USUL_TRACE_SCOPE;
  //return false;
  // Implement me to update radio state.
  Usul::Interfaces::IGeneralShader::QueryPtr nav ( const_cast < Usul::Interfaces::IUnknown * > ( this->caller() ) );

  std::string name = nav->getCurrentModel();
  if( name == _name )
    return true;
  else
    return false;
}

