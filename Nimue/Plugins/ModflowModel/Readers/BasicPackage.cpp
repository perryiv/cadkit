
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
//  Basic package file reader.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowModel/Readers/BasicPackage.h"
#include "ModflowModel/Attributes/Attribute.h"
#include "ModflowModel/Constants.h"
#include "ModflowModel/ModflowDocument.h"

#include "Usul/Trace/Trace.h"

using namespace Modflow::Readers;

USUL_IMPLEMENT_TYPE_ID ( BasicPackage );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

BasicPackage::BasicPackage() : BaseClass()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

BasicPackage::~BasicPackage()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void BasicPackage::read ( Modflow::ModflowDocument *doc, const std::string &file, Unknown *progress )
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
  Layers &layers = _document->layers();

  // Determine grid size and number of layers.
  const Vec2ui gridSize ( _document->gridSize() );
  const unsigned int numCells ( gridSize[0] * gridSize[1] );
  const unsigned int numLayers ( _document->numLayers() );

  // For each layer...
  for ( unsigned int i = 0; i < numLayers; ++i )
  {
    // Read the grid.
    GridInfo bounds ( "", GridData ( numCells ) );
    this->_seekToLine ( "INTERNAL" );
    this->_checkStream();
    this->_readGrid ( 4, bounds );

    // Zap all the cells outside of the bounds.
    layers.at(i)->purge ( bounds.second );
  }

  // For each layer...
  for ( unsigned int i = 0; i < numLayers; ++i )
  {
    // Read the grid.
    GridInfo startHeads ( "", GridData ( numCells ) );
    this->_seekToLine ( "INTERNAL" );
    this->_checkStream();
    this->_readGrid ( 4, startHeads );

    // Get layer.
    Layer::RefPtr layer ( layers.at ( i ) );
    if ( true == layer.valid() )
    {
      // Have to think about this... you also show the heads over time...
      // Is the first time-step the same? Plus, you're not actually drawing anything...
      #if 0
      // Add attribute for entire layer.
      layer->addAttribute ( new Modflow::Attributes::Attribute 
        ( Modflow::Names::STARTING_HEAD, layer->queryInterface ( Usul::Interfaces::IUnknown::IID ) ) );
      // Need to do this too.
      //attribute->initVisibleState();
      #endif

      // Set the start heads.
      layer->vector ( Modflow::Names::STARTING_HEAD, 0, startHeads.second );
    }
  }
}
