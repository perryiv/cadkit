
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
//  Discretization file reader.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowModel/Readers/Discretization.h"
#include "ModflowModel/Attributes/CellBoundary.h"
#include "ModflowModel/Constants.h"
#include "ModflowModel/ModflowDocument.h"

#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Format.h"
#include "Usul/Strings/Split.h"
#include "Usul/Trace/Trace.h"

#include "ogr_spatialref.h"
#include "cpl_error.h"

#include <vector>

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

  // Set default cell-margin from cell's size.
  doc->setCellMargin ( 0.1 * ( 0.5 * ( cellSize[0] + cellSize[1] ) ) );

  // Look for coordinate system.
  {
    // Make a new spatial reference system.
    std::auto_ptr<OGRSpatialReference> srs ( this->_coordinateSystem() );
    if ( 0x0 != srs.get() )
      doc->sourceCoordinateSystem ( srs.release() );
  }

  // Look for a unit conversion.
  {
    const std::string conversion ( this->fileAttribute ( Modflow::Attributes::UNIT_CONVERSION ) );
    const double scale ( ( false == conversion.empty() ) ? Usul::Strings::fromString<double> ( conversion ) : 1 );
    doc->lengthConversion ( scale );
  }

  // Get coordinate of first cell in grid (the upper left).
  {
    std::vector<std::string> parts;
    Usul::Strings::split ( this->fileAttribute ( Modflow::Attributes::CELL_GRID_ORIGIN ), ' ', false, parts );
    const double xOffset ( ( parts.size() > 0 ) ? Usul::Strings::fromString<double> ( parts.at(0) ) : 0 );
    const double yOffset ( ( parts.size() > 1 ) ? Usul::Strings::fromString<double> ( parts.at(1) ) : 0 );
    const double zOffset ( ( parts.size() > 2 ) ? Usul::Strings::fromString<double> ( parts.at(2) ) : 0 );
    doc->offsetSet ( Modflow::Attributes::CELL_GRID_ORIGIN, Usul::Math::Vec3d ( xOffset, yOffset, zOffset ) );
  }

  // Make the layers and fill with empty cells.
  Layers layers ( numLayers );
  Helper::fill ( layers, gridSize, cellSize );

  // Read the land surface.
  this->_seekToLine ( "INTERNAL" );
  this->_checkStream();
  GridInfo top ( "", GridData ( numCells ) );
  this->_readGrid ( 4, top );

  // Tell document about the elevation data.
  doc->elevations ( top.second );

  // For each layer...
  {
    for ( unsigned int i = 0; i < numLayers; ++i )
    {
      // Read the grid.
      GridInfo bottom ( "", GridData ( numCells ) );
      this->_seekToLine ( "INTERNAL" );
      this->_checkStream();
      this->_readGrid ( 4, bottom );

      // Add a layer.
      Layer::RefPtr layer ( layers.at ( i ) );
      layer->zRange ( top.second, bottom.second );

      // Make attribute for the boundary.
      Modflow::Attributes::Attribute::RefPtr boxes ( new Modflow::Attributes::CellBoundary 
        ( Modflow::Names::CELL_BOUNDARY, Modflow::Geometry::CELL_TOP, layer->queryInterface ( Usul::Interfaces::IUnknown::IID ), this->registryPath() ) );
      //boxes->initVisibleState();

      // Set the attribute's color.
      boxes->minColor ( doc->color ( Modflow::Names::MIN_ELEVATION ) );
      boxes->maxColor ( doc->color ( Modflow::Names::MAX_ELEVATION ) );

      // Add attribute.
      layer->addAttribute ( boxes.get() );

      // The bottom becomes the new top.
      top = bottom;
    }
  }

  // Hook up the neighbors.
  Layer::RefPtr above ( 0x0 );
  {
    for ( unsigned int j = 0; j < numLayers; ++j )
    {
      // Get the layers.
      Layer::RefPtr layer ( layers.at ( j ) );
      Layer::RefPtr below ( ( j + 1 < numLayers ) ? layers.at ( j + 1 ) : 0x0 );

      // Add neighbors.
      layer->above ( above.get() );
      layer->below ( below.get() );

      // Current layer becomes the above.
      above = layer;
    }
  }

  // Set document's layers.
  _document->layers ( layers );
}
