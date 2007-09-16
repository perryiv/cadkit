
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Block-centered flow reader.
//
///////////////////////////////////////////////////////////////////////////////

#include "BlockCenteredFlow.h"
#include "ModflowDocument.h"

#include "Usul/Trace/Trace.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

BlockCenteredFlow::BlockCenteredFlow() : BaseClass()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

BlockCenteredFlow::~BlockCenteredFlow()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void BlockCenteredFlow::read ( ModflowDocument *doc, const std::string &file, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Ignore empty strings and null documents.
  if ( true == file.empty() || 0x0 == doc )
    return;

  std::cout << "Reading: " << file << std::endl;

  // Open the file and set the members.
  this->_open ( file );
  this->_set ( doc, progress );

  // Get layers.
  ModflowDocument::Guard ( _document->mutex() );
  Layers &layers = _document->layers();

  // Determine grid size and number of layers.
  const Vec2ui gridSize ( _document->gridSize() );
  const unsigned int numCells ( gridSize[0] * gridSize[1] );
  const unsigned int numLayers ( _document->numLayers() );

  // For each layer...
  for ( unsigned int i = 0; i < numLayers; ++i )
  {
    // TODO...
  }
}
