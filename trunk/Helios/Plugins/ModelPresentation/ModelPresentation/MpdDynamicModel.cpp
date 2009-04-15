
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/ModelPresentation/ModelPresentation/MpdDynamicModel.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IMpdNavigator.h"

USUL_IMPLEMENT_COMMAND ( MpdDynamicModel );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdDynamicModel::MpdDynamicModel ( Usul::Interfaces::IUnknown * caller, const std::string text, unsigned int set ) :
  BaseClass ( caller ),
  _text ( text ),
  _set ( set )
{
  USUL_TRACE_SCOPE;
  this->text ( text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdDynamicModel::~MpdDynamicModel ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void MpdDynamicModel::_execute ()
{
  USUL_TRACE_SCOPE;
  
#if 1
  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( this->caller().get() );

  if ( nav.valid () )
  {
    if( true == nav->dynamicModelState( _set ) )
    {
      nav->dynamicModelState( _set, false );
    }
    else
    {
      nav->dynamicModelState( _set, true );
    }
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the radio button.  Return true to have radio checked.
//
///////////////////////////////////////////////////////////////////////////////

bool MpdDynamicModel::updateCheck () const
{
  USUL_TRACE_SCOPE;

  // Implement me to update radio state.
  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( const_cast < Usul::Interfaces::IUnknown * > ( this->caller().get() ) );

  
  if( true == nav->dynamicModelState( _set ) )
    return true;
  else
    return false;
}

