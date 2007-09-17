
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/WRF/WrfModel/NextTimestep.h"

#include "Usul/Trace/Trace.h"

USUL_IMPLEMENT_COMMAND ( NextTimestep );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

NextTimestep::NextTimestep ( WRFDocument* doc ) :
  BaseClass ( 0x0 ),
  _document ( doc )
{
  USUL_TRACE_SCOPE;
  this->text ( "Next Timestep" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

NextTimestep::~NextTimestep ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void NextTimestep::_execute ()
{
  USUL_TRACE_SCOPE;
  if ( _document.valid () )
  {
    unsigned int current ( _document->getCurrentTimeStep () );
    _document->setCurrentTimeStep ( ++current );
  }
}
