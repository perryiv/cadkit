
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "MpdTools.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IMpdNavigator.h"

USUL_IMPLEMENT_COMMAND ( MpdTools );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdTools::MpdTools ( Usul::Interfaces::IUnknown * caller, const std::string& text ) :
  BaseClass ( caller ),
  _text ( text )
{
  USUL_TRACE_SCOPE;
  this->text ( text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdTools::~MpdTools ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void MpdTools::_execute ()
{
  USUL_TRACE_SCOPE;
  
if( "ShowMatrix" == this->_text )
{
  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( this->caller() );
  if ( nav.valid () )
  {
    nav->displayViewMatrix();
  }
}
}

