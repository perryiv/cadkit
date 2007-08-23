
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/WRF/WrfModel/ChangeNumPlanes.h"
#include "Experimental/WRF/WrfModel/WRFDocument.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Strings/Convert.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ChangeNumPlanes::ChangeNumPlanes ( double multiplier, WRFDocument* doc ) :
  BaseClass ( 0x0 ),
  _multiplier ( multiplier ),
  _document ( doc )
{
  USUL_TRACE_SCOPE;
  this->text ( "Multiply planes x " + Usul::Strings::toString ( multiplier ) );

  if ( 0x0 != _document )
    _document->ref ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ChangeNumPlanes::~ChangeNumPlanes ()
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != _document )
    _document->unref ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void ChangeNumPlanes::_execute ()
{
  USUL_TRACE_SCOPE;
  if ( 0x0 != _document )
  {
    unsigned int numPlanes ( _document->numPlanes () );
    _document->numPlanes ( static_cast < unsigned int > ( numPlanes * _multiplier ) );
  }
}
