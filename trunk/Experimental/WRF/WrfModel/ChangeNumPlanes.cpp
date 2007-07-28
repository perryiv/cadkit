
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "ChangeNumPlanes.h"

#include "Usul/Trace/Trace.h"

#include "Usul/Strings/Convert.h"

ChangeNumPlanes::ChangeNumPlanes ( double multiplier, WRFDocument* doc ) :
  BaseClass ( 0x0 ),
  _multiplier ( multiplier ),
  _document ( doc )
{
  this->text ( "Multiply planes x " + Usul::Strings::toString ( multiplier ) );
}

ChangeNumPlanes::~ChangeNumPlanes ()
{
}

void ChangeNumPlanes::_execute ()
{
  if ( _document.valid () )
  {
    unsigned int numPlanes ( _document->numPlanes () );
    _document->numPlanes ( static_cast < unsigned int > ( numPlanes * _multiplier ) );
  }
}
