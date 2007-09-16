
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Discretization file reader.
//
///////////////////////////////////////////////////////////////////////////////

#include "Discretization.h"
#include "ModflowDocument.h"

#include "Usul/Trace/Trace.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Discretization::Discretization() : BaseClass()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Discretization::~Discretization()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Fill the layers with empty cells.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void fill ( Discretization::Layers &layers, const Discretization::Vec2ui &gridSize, const Discretization::Vec2d &cellSize )
  {
    typedef Discretization::Layers Layers;
    for ( Layers::iterator i = layers.begin(); i != layers.end(); ++i )
    {
      // This has to be by reference!
      Layer::RefPtr &layer ( *i );
      layer = new Layer ( gridSize[0], gridSize[1], cellSize );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void Discretization::read ( ModflowDocument *doc, const std::string &file, Unknown *progress )
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

  // Skip comment lines.
  this->_skipLines ( '#' );

  // Read the header.
  this->_checkStream();
  unsigned int numLayers ( 0 );
  ModflowDocument::Vec2ui gridSize ( 0, 0 );
  _in >> numLayers >> gridSize[0] >> gridSize[1];
  const unsigned int numCells ( gridSize[0] * gridSize[1] );

  // Skip to the grid points.
  this->_seekToLine ( "CONSTANT" );

  // Read the cell size.
  std::string dummy;
  this->_checkStream();
  ModflowDocument::Vec2d cellSize ( 0.0, 0.0 );
  _in >> dummy >> cellSize[0]; this->_getLine();
  _in >> dummy >> cellSize[1]; this->_getLine();

  // Make the layers and fill with empty cells.
  Layers layers ( numLayers );
  Helper::fill ( layers, gridSize, cellSize );

  // Read the land surface.
  this->_seekToLine ( "INTERNAL" );
  this->_checkStream();
  GridInfo top ( "", GridData ( numCells ) );
  this->_readGrid ( top );

  // For each layer...
  for ( unsigned int i = 0; i < numLayers; ++i )
  {
    // Read the grid.
    GridInfo bottom ( "", GridData ( numCells ) );
    this->_seekToLine ( "INTERNAL" );
    this->_checkStream();
    this->_readGrid ( bottom );

    // Add a layer.
    Layer::RefPtr layer = layers.at ( i );
    layer->zRange ( top.second, bottom.second );

    // The bottom becomes the new top.
    top = bottom;
  }

  // Hook up the neighbors.
  Layer::RefPtr above ( 0x0 );
  for ( unsigned int j = 0; j < numLayers; ++j )
  {
    // Get the layers.
    Layer::RefPtr layer = layers.at ( j );
    Layer::RefPtr below ( ( j + 1 < numLayers ) ? layers.at ( j + 1 ) : 0x0 );

    // Add neighbors.
    layer->above ( above.get() );
    layer->below ( below.get() );

    // Current layer becomes the above.
    above = layer;
  }

  // Set document's layers.
  _document->layers ( layers );
}
