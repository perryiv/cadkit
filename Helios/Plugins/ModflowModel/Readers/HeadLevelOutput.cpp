
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Head-level output reader.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/ModflowModel/Readers/HeadLevelOutput.h"
#include "Helios/Plugins/ModflowModel/Attributes/Attribute.h"
#include "Helios/Plugins/ModflowModel/Constants.h"
#include "Helios/Plugins/ModflowModel/ModflowDocument.h"

#include "Usul/Trace/Trace.h"

#include <sstream>

using namespace Modflow::Readers;

USUL_IMPLEMENT_TYPE_ID ( HeadLevelOutput );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

HeadLevelOutput::HeadLevelOutput() : BaseClass()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

HeadLevelOutput::~HeadLevelOutput()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void HeadLevelOutput::read ( Modflow::ModflowDocument *doc, const std::string &file, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Ignore empty strings and null documents.
  if ( true == file.empty() || 0x0 == doc )
    return;

  std::cout << "Reading: " << file << std::endl;

  // Determine the number of lines that start with the label.
  // Do this before we open the file below.
  const std::string label ( "HEAD" );
  unsigned int numLines ( this->_countLines ( label, file ) );

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

  // Figure out how many time-steps there are.
  const unsigned int numTimeSteps ( numLines / numLayers );

  // Loop through all the data sets. If there is a mis-match then 
  // one or more of the layers will get less.
  for ( unsigned int t = 0; t < numTimeSteps; ++t )
  {
    // For each layer...
    for ( unsigned int i = 0; i < numLayers; ++i )
    {
      // Read the grid.
      GridInfo headLevel ( "", GridData ( numCells ) );
      this->_seekToLine ( "HEAD" );
      this->_checkStream();
      this->_readGrid ( 0, headLevel );

      // Set the start heads.
      layers.at(i)->vector ( Modflow::Names::HEAD_LEVELS, t, headLevel.second );
    }

    // Feedback.
    std::cout << "Reading head levels for time-step: " << t << std::endl;
  }

  // If we have time-steps...
  if ( numTimeSteps > 0 )
  {
    // Add attribute for each layer...
    for ( unsigned int i = 0; i < numLayers; ++i )
    {
      Modflow::Model::Layer::RefPtr layer ( layers.at(i) );
      if ( true == layer.valid() )
      {
        layer->addAttribute ( new Modflow::Attributes::Attribute 
          ( Modflow::Names::HEAD_LEVELS, layer->queryInterface ( Usul::Interfaces::IUnknown::IID ) ) );
      }
    }
  }
}
