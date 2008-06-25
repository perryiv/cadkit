
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Block-centered flow reader.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowModel/Readers/BlockCenteredFlow.h"
#include "ModflowModel/ModflowDocument.h"

#include "Usul/Trace/Trace.h"

using namespace Modflow::Readers;

USUL_IMPLEMENT_TYPE_ID ( BlockCenteredFlow );


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

void BlockCenteredFlow::read ( Modflow::ModflowDocument *doc, const std::string &file, Unknown *progress )
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
  Modflow::ModflowDocument::Guard ( _document->mutex() );

  // Determine grid size and number of layers.
  const Vec2ui gridSize ( _document->gridSize() );
  const unsigned int numLayers ( _document->numLayers() );

  // For each layer...
  for ( unsigned int i = 0; i < numLayers; ++i )
  {
    // TODO...
  }
}
