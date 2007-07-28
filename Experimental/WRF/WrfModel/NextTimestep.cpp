
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "NextTimestep.h"

#include "Usul/Trace/Trace.h"

NextTimestep::NextTimestep ( WRFDocument* doc ) :
  BaseClass ( 0x0 ),
  _document ( doc )
{
  this->text ( "Next Timestep" );
}

NextTimestep::~NextTimestep ()
{
}

void NextTimestep::_execute ()
{
  if ( _document.valid () )
  {
    unsigned int current ( _document->getCurrentTimeStep () );
    _document->setCurrentTimeStep ( ++current );
  }
}
