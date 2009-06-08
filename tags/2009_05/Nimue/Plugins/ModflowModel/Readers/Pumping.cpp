
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
//  Pumping input reader.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowModel/Readers/Pumping.h"
#include "ModflowModel/Constants.h"
#include "ModflowModel/ModflowDocument.h"
#include "ModflowModel/Attributes/Cylinders.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/MPL/SameType.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Split.h"
#include "Usul/Trace/Trace.h"

#include <algorithm>
#include <functional>
#include <list>
#include <string>

using namespace Modflow::Readers;

USUL_IMPLEMENT_TYPE_ID ( Pumping );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Pumping::Pumping() : BaseClass()
{
  USUL_TRACE_SCOPE;
  this->name ( Modflow::Names::PUMPING );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Pumping::~Pumping()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the pumping values.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void addPumping ( const Modflow::Readers::Pumping::TimeStamp &stamp, 
                           const Modflow::Readers::Pumping::GridData &v, 
                           Modflow::Attributes::Cylinders::RefPtr attribute )
  {
    USUL_TRACE_SCOPE_STATIC;

    if ( true == attribute.valid() )
    {
      attribute->addValues ( stamp, v );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void Pumping::read ( Modflow::ModflowDocument *doc, const std::string &file, Unknown *progress )
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

  // Used when parsing.
  std::string dummy;
  const unsigned int numLayers ( doc->numLayers() );
  if ( 0 == numLayers )
    return;

  // Get the grid size.
  const Vec2ui gridSize ( doc->gridSize() );
  const unsigned int numCells ( gridSize[0] * gridSize[1] );
  if ( 0 == numCells )
    return;

  // Skip the first line.
  this->_getLine();

  // Keep track of the line. We already skipped the first one.
  unsigned int lineNumber ( 1 );


  // Make the vectors. We re-use them below.
  typedef std::vector < GridData > AllLayersGridData;
  AllLayersGridData data ( numLayers );
  {
    for ( AllLayersGridData::iterator i = data.begin(); i != data.end(); ++i )
    {
      i->resize ( numCells );
    }
  }

  // Make the attributes. Need one per layer.
  typedef Modflow::Attributes::Cylinders::RefPtr CylindersPtr;
  typedef std::vector < CylindersPtr > AllCylinders;
  AllCylinders cylinders ( numLayers );
  {
    for ( unsigned int i = 0; i < numLayers; ++i )
    {
      Modflow::Attributes::Cylinders::RefPtr &attribute ( cylinders.at ( i ) );
      attribute = new Modflow::Attributes::Cylinders ( Modflow::Names::PUMPING, true, this->registryPath() );
      attribute->color ( this->fileAttribute ( Modflow::Attributes::COLOR ) );
      attribute->heightScale ( this->fileAttribute ( Modflow::Attributes::HEIGHT_SCALE ) );
      attribute->radiusScale ( this->fileAttribute ( Modflow::Attributes::RADIUS_SCALE ) );
      attribute->sortTriangles ( this->fileAttribute ( Modflow::Attributes::SORT_TRIANGLES ) );
      //attribute->initVisibleState();
      Modflow::Model::Layer::RefPtr layer ( doc->layers().at ( i ) );
      if ( true == layer.valid() )
      {
        layer->addAttribute ( attribute.get() );
      }
    }
  }

  // Loop until the end of the file.
  while ( EOF != _in.peek() )
  {
    // Increment the line number.
    ++lineNumber;

    // Read the record header.
    unsigned int numEntries ( 0 );
    std::string timeUnit;
    TimeStamp stamp ( 0 );
    _in >> numEntries >> dummy >> dummy >> dummy >> timeUnit >> stamp;
    this->_getLine();

    // If we have a year...
    if ( "year" == Usul::Strings::lowerCase ( timeUnit ) )
    {
      {
        // Populate data with zeros.
        for ( AllLayersGridData::iterator i = data.begin(); i != data.end(); ++i )
        {
          GridData &layer ( *i );
          layer.assign ( layer.size(), 0 );
        }
      }

      {
        // Loop through the entries.
        for ( unsigned int i = 0; i < numEntries; ++i )
        {
          // Increment the line number.
          ++lineNumber;

          // Read the data.
          unsigned int layer ( 0 ), row ( 0 ), col ( 0 );
          _in >> layer >> row >> col;
          if ( ( layer > 0 ) && ( layer <= numLayers ) && ( row > 0 ) && ( row <= gridSize[0] ) && ( col > 0 ) && ( col <= gridSize[1] ) )
          {
            // Calculate index into container of data.
            const unsigned int index ( ( ( row - 1 ) * gridSize[1] ) + ( col - 1 ) );

            // Read the value.
            double value ( 0 );
            _in >> value;

            // Add the value, do not just set it.
            double &element ( data.at ( layer - 1 ).at ( index ) );
            element += value;

            // Skip the rest of the line.
            this->_getLine();
          }
          else
          {
            std::cout << Usul::Strings::format ( "Warning 2644544193: Line ", lineNumber, ", parameter out of range: layer = ", layer, ", row = ", row, ", column = ", col ) << std::endl;
          }
        }
      }

      {
        // Loop through the layers.
        Layers &layers ( doc->layers() );
        for ( unsigned int i = 0; i < layers.size(); ++i )
        {
          Modflow::Model::Layer::RefPtr layer ( layers.at ( i ) );
          if ( true == layer.valid() )
          {
            // Get the attribute.
            Modflow::Attributes::Cylinders::RefPtr attribute ( cylinders.at ( i ) );
            if ( true == attribute.valid() )
            {
              // Add the data to the attribute.
              Usul::Functions::safeCallR1R2R3 ( &Helper::addPumping, stamp, data.at ( i ), attribute, "3011930880" );
            }
          }
        }
      }
    }
    else
    {
      std::cout << Usul::Strings::format ( "Warning 4293402230: Line ", lineNumber, ", expected 5th word to be 'Year', read '", timeUnit, "' instead" ) << std::endl;
    }
  }
}
