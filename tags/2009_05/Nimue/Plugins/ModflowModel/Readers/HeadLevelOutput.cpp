
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
//  Head-level output reader.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowModel/Readers/HeadLevelOutput.h"
#include "ModflowModel/Attributes/CellBoundary.h"
#include "ModflowModel/Attributes/HeadSurface.h"
#include "ModflowModel/Constants.h"
#include "ModflowModel/ModflowDocument.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Strings/Split.h"
#include "Usul/Trace/Trace.h"

#include <algorithm>
#include <list>
#include <string>

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
  this->name ( Modflow::Names::HEAD_LEVELS );
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
  unsigned int numLines ( this->_countLines ( label, file, false ) );

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

      // Set the heads for this time step.
      layers.at(i)->vector ( Modflow::Names::HEAD_LEVELS, t, headLevel.second );

      // Let the document know the time-stamp for this time-step.
      if ( 0 == i )
      {
        // Get the time-stamp.
        TimeStamp stamp ( 0 );
        const bool hasTimeStamp ( this->_getTimeStamp ( headLevel.first, stamp ) );
        if ( hasTimeStamp )
        {
          _document->timeStampSet ( t, stamp );
        }
      }
    }

    // Feedback.
    std::cout << "Reading head levels for time-step: " << t << std::endl;
  }

  // Tell the document the number of time steps.
  _document->setNumberOfTimeSteps ( numTimeSteps );

  // If we have time-steps...
  if ( numTimeSteps > 0 )
  {
    // Add attribute for each layer...
    for ( unsigned int i = 0; i < numLayers; ++i )
    {
      Modflow::Model::Layer::RefPtr layer ( layers.at(i) );
      if ( true == layer.valid() )
      {
        {
          Modflow::Attributes::HeadLevels::RefPtr attribute ( new Modflow::Attributes::HeadLevels ( Modflow::Names::HEAD_LEVELS, layer, this->registryPath() ) );
          attribute->init ( _document, layer );
          //attribute->initVisibleState();
          layer->addAttribute ( attribute );
        }

        #ifndef _DEBUG // Too slow in debug...

        {
          Modflow::Attributes::HeadLevels::RefPtr attribute ( new Modflow::Attributes::HeadSurface ( Modflow::Names::HEAD_SURFACE_COARSE, layer, 0, 0, this->registryPath() ) );
          attribute->init ( _document, layer );
          //attribute->initVisibleState();
          layer->addAttribute ( attribute );
        }
        
        {
          Modflow::Attributes::HeadLevels::RefPtr attribute ( new Modflow::Attributes::HeadSurface ( Modflow::Names::HEAD_SURFACE_SMOOTH, layer, 1, 0, this->registryPath() ) );
          attribute->init ( _document, layer );
          //attribute->initVisibleState();
          layer->addAttribute ( attribute );
        }

        #endif
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the time-step.
//
///////////////////////////////////////////////////////////////////////////////

bool HeadLevelOutput::_getTimeStamp ( const std::string &s, TimeStamp &stamp )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  typedef std::vector<std::string> StringList;

  // Initialize the result.
  bool result ( false );

  // Split on the white space.
  StringList sl;
  Usul::Strings::split ( s, ' ', false, sl );
  if ( sl.size() < 2 )
    return result;

  // Get the time.
  const std::string a ( sl.at ( 1 ) );
  const bool isNum ( a.size() == std::count_if ( a.begin(), a.end(), ::isdigit ) );
  if ( true == isNum )
  {
    const TimeStamp t ( Usul::Convert::Type<std::string,TimeStamp>::convert ( a ) );
    stamp = t;
    result = true;
  }

  // Return the result, which may still be false.
  return result;
}
