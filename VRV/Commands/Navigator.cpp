
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Commands/Navigator.h"

#include "Usul/Interfaces/INavigationFunctor.h"

using namespace VRV::Commands;

USUL_IMPLEMENT_COMMAND ( Navigator );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Navigator::Navigator ( BaseFunctor *functor, Usul::Interfaces::IUnknown *caller ) : 
  BaseClass ( caller ),
  _functor ( functor )
{
  if ( _functor.valid() )
    this->text ( _functor->name() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Navigator::~Navigator ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::_execute()
{
  Usul::Interfaces::INavigationFunctor::QueryPtr nf ( this->caller().get() );

  if ( nf.valid() )
  {
    if ( this->updateCheck() )
      nf->navigator ( 0x0 );
    else
      nf->navigator ( _functor.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the check.
//
///////////////////////////////////////////////////////////////////////////////

bool Navigator::updateCheck() const
{
  Usul::Interfaces::INavigationFunctor::QueryPtr nf ( const_cast < Usul::Interfaces::IUnknown * > ( this->caller().get() ) );

  if ( nf.valid() )
  {
    BaseFunctor::RefPtr functor ( nf->navigator() );
    return ( functor.valid() && _functor.valid() ) ? functor->name() == _functor->name() : false;
  }

  return false;
}
