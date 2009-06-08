
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/WRF/WrfModel/LoadDataJob.h"
#include "Experimental/WRF/WrfModel/WRFDocument.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadDataJob::LoadDataJob ( const ReadRequest& request, WRFDocument* document, const Parser& parser ) :
  BaseClass (),
  _request ( request ),
  _document ( document ),
  _parser ( parser )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != _document )
    _document->ref ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadDataJob::~LoadDataJob ()
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != _document )
    _document->unref ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data.
//
///////////////////////////////////////////////////////////////////////////////

void LoadDataJob::_started ()
{
  USUL_TRACE_SCOPE;

  // Return if we don't have any.
  if ( 0x0 == _document )
    return;

  // Vector for raw floating point data.
  Parser::Data data;

  // Get the timestep and channel we are suppose to load.
  unsigned int timestep ( _request.first );
  unsigned int channel ( _request.second );

  std::cout << "Reading data.  Timestep: " << timestep << " Channel: " << channel << std::endl;

  // Read the data from disk.
  _parser.data ( data, timestep, channel );

  // Give the document the data.
  _document->addData ( timestep, channel, data );

  // Let the document know we are done.
  _document->loadJobFinished ( this );
}
