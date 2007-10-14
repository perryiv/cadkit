
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Basic package file reader.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/ModflowModel/Readers/BasicPackage.h"
#include "Helios/Plugins/ModflowModel/Attributes/Attribute.h"
#include "Helios/Plugins/ModflowModel/Constants.h"
#include "Helios/Plugins/ModflowModel/ModflowDocument.h"

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

    // Add attribute for entire layer.
    Layer::RefPtr layer = layers.at ( i );
    layer->addAttribute ( new Modflow::Attributes::Attribute 
      ( Modflow::Names::STARTING_HEAD, layer->queryInterface ( Usul::Interfaces::IUnknown::IID ) ) );

    // Set the start heads.
    layer->vector ( Modflow::Names::STARTING_HEAD, 0, startHeads.second );
  }
}
