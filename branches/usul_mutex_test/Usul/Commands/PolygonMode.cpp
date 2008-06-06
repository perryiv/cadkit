
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Commands/PolygonMode.h"

using namespace Usul::Commands;

USUL_IMPLEMENT_COMMAND ( PolygonMode );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonMode::PolygonMode ( const std::string& name, Mode mode, Usul::Interfaces::IUnknown *caller ) : 
BaseClass ( caller ),
_mode ( mode )
{
  this->text ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonMode::~PolygonMode ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonMode::_execute ()
{
  Usul::Interfaces::IPolygonMode::QueryPtr pm ( this->caller () );

  if ( pm.valid () )
    pm->polygonMode ( _mode );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the check.
//
///////////////////////////////////////////////////////////////////////////////

bool PolygonMode::updateCheck () const
{
  Usul::Interfaces::IPolygonMode::QueryPtr pm ( const_cast < Usul::Interfaces::IUnknown * > ( this->caller () ) );

  if ( pm.valid () )
    return _mode == pm->polygonMode ();

  return false;
}
