
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "PreviousTimestep.h"

#include "Usul/Trace/Trace.h"

PreviousTimestep::PreviousTimestep ( WRFDocument* doc ) :
  BaseClass ( 0x0 ),
  _document ( doc )
{
  this->text ( "Previous Timestep" );
}

PreviousTimestep::~PreviousTimestep()
{
}

void PreviousTimestep::_execute ()
{
  if ( _document.valid () )
  {
    unsigned int current ( _document->getCurrentTimeStep () );
    if ( 0 == current )
      current = _document->getNumberOfTimeSteps();
    _document->setCurrentTimeStep ( --current );
  }
}
