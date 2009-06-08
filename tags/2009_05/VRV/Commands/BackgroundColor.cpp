
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Commands/BackgroundColor.h"
#include "Usul/Interfaces/IBackgroundColor.h"

using namespace VRV::Commands;

USUL_IMPLEMENT_COMMAND ( BackgroundColor );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BackgroundColor::BackgroundColor ( const std::string& name, const Usul::Math::Vec4f& color, Usul::Interfaces::IUnknown *caller ) : 
BaseClass ( caller ),
_color ( color )
{
  this->text ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BackgroundColor::~BackgroundColor()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void BackgroundColor::_execute()
{
  Usul::Interfaces::IBackgroundColor::QueryPtr bc ( this->caller().get() );

  if ( bc.valid() )
    bc->backgroundColor ( _color );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the check.
//
///////////////////////////////////////////////////////////////////////////////

bool BackgroundColor::updateCheck() const
{
  Usul::Interfaces::IBackgroundColor::QueryPtr bc ( const_cast < Usul::Interfaces::IUnknown * > ( this->caller().get() ) );
  return bc.valid() ? _color.equal ( bc->backgroundColor() ) : false;
}
