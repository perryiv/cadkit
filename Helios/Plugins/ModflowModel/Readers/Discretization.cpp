
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

#include "Helios/Plugins/ModflowModel/Readers/Discretization.h"
#include "Helios/Plugins/ModflowModel/Attributes/Boxes.h"
#include "Helios/Plugins/ModflowModel/Constants.h"
#include "Helios/Plugins/ModflowModel/ModflowDocument.h"

#include "Usul/Strings/Format.h"
#include "Usul/Trace/Trace.h"

using namespace Modflow::Readers;

USUL_IMPLEMENT_TYPE_ID ( Discretization );


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
    for ( unsigned int i = 0; i < layers.size(); ++i )
    {
      // Make the name.
      const std::string name ( Usul::Strings::format ( "Layer ", i + 1 ) );

      // This has to be by reference!
      Discretization::Layer::RefPtr &layer ( layers.at ( i ) );
      layer = new Discretization::Layer ( name, gridSize[0], gridSize[1], cellSize );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void Discretization::read ( Modflow::ModflowDocument *doc, const std::string &file, Unknown *progress )
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
  Modflow::ModflowDocument::Vec2ui gridSize ( 0, 0 );
  _in >> numLayers >> gridSize[0] >> gridSize[1];
  const unsigned int numCells ( gridSize[0] * gridSize[1] );

  // Skip to the grid points.
  this->_seekToLine ( "CONSTANT" );

  // Read the cell size.
  std::string dummy;
  this->_checkStream();
  Modflow::ModflowDocument::Vec2d cellSize ( 0.0, 0.0 );
  _in >> dummy >> cellSize[0]; this->_getLine();
  _in >> dummy >> cellSize[1]; this->_getLine();

  // Make the layers and fill with empty cells.
  Layers layers ( numLayers );
  Helper::fill ( layers, gridSize, cellSize );

  // Read the land surface.
  this->_seekToLine ( "INTERNAL" );
  this->_checkStream();
  GridInfo top ( "", GridData ( numCells ) );
  this->_readGrid ( 4, top );

  // For each layer...
  for ( unsigned int i = 0; i < numLayers; ++i )
  {
    // Read the grid.
    GridInfo bottom ( "", GridData ( numCells ) );
    this->_seekToLine ( "INTERNAL" );
    this->_checkStream();
    this->_readGrid ( 4, bottom );

    // Add a layer.
    Layer::RefPtr layer = layers.at ( i );
    layer->zRange ( top.second, bottom.second );

    // Make attribute for the boundary.
    Modflow::Attributes::Boxes::RefPtr boxes ( new Modflow::Attributes::Boxes 
      ( Modflow::Names::CELL_BOUNDARY, layer->queryInterface ( Usul::Interfaces::IUnknown::IID ) ) );

    // Set the attribute's color.
    boxes->minColor ( doc->color ( Modflow::Names::MIN_ELEVATION ) );
    boxes->maxColor ( doc->color ( Modflow::Names::MAX_ELEVATION ) );

    // Add attribute.
    layer->addAttribute ( boxes.get() );

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
