
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
//  Recharge input reader.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowModel/Readers/Recharge.h"
#include "ModflowModel/Constants.h"
#include "ModflowModel/ModflowDocument.h"
#include "ModflowModel/Attributes/Cylinders.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/MPL/SameType.h"
#include "Usul/Strings/Split.h"
#include "Usul/Trace/Trace.h"

#include <algorithm>
#include <list>
#include <string>

using namespace Modflow::Readers;

USUL_IMPLEMENT_TYPE_ID ( Recharge );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Recharge::Recharge() : BaseClass()
{
  USUL_TRACE_SCOPE;
  this->name ( Modflow::Names::RECHARGE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Recharge::~Recharge()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the recharge values.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void addRecharge ( const Modflow::Readers::Recharge::TimeSpan &span, 
                            const Modflow::Readers::Recharge::GridData &v, 
                            Modflow::Attributes::Cylinders::RefPtr attribute )
  {
    USUL_TRACE_SCOPE_STATIC;

    if ( true == attribute.valid() )
    {
      attribute->addValues ( span.first, v, span.second );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void Recharge::read ( Modflow::ModflowDocument *doc, const std::string &file, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Ignore empty strings and null documents.
  if ( true == file.empty() || 0x0 == doc )
    return;

  std::cout << "Reading: " << file << std::endl;

  // Determine the number of lines that start with the label.
  // Do this before we open the file below.
  const std::string label ( "INTERNAL" );
  unsigned int numEntries ( this->_countLines ( label, file ) );

  // Punt if there are no entries.
  if ( 0 == numEntries )
    return;

  // Open the file and set the members.
  this->_open ( file );
  this->_set ( doc, progress );

  // Make the attribute. No need to have it update because we read the spans.
  Modflow::Attributes::Cylinders::RefPtr attribute ( new Modflow::Attributes::Cylinders ( Modflow::Names::RECHARGE, false, this->registryPath() ) );

  // Set properties.
  attribute->color ( this->fileAttribute ( Modflow::Attributes::COLOR ) );
  attribute->heightScale ( this->fileAttribute ( Modflow::Attributes::HEIGHT_SCALE ) );
  attribute->radiusScale ( this->fileAttribute ( Modflow::Attributes::RADIUS_SCALE ) );
  attribute->sortTriangles ( this->fileAttribute ( Modflow::Attributes::SORT_TRIANGLES ) );
  //attribute->initVisibleState();

  // Determine grid size and number of layers.
  const Vec2ui gridSize ( _document->gridSize() );
  const unsigned int numCells ( gridSize[0] * gridSize[1] );

  // Loop through all the entries.
  for ( unsigned int t = 0; t < numEntries; ++t )
  {
    // Read the grid.
    GridInfo recharge ( "", GridData ( numCells ) );
    this->_seekToLine ( "INTERNAL" );
    this->_checkStream();
    this->_readGrid ( 4, recharge );

    // Get the time-span.
    TimeSpan span ( 0, 1 );
    const bool hasTimeSpan ( this->_getTimeSpan ( recharge.first, span ) );

    // Feedback.
    std::cout << Usul::Strings::format ( ( ( hasTimeSpan ) ? "Reading" : "Skipping" ), " recharge amount for entry: ", recharge.first ) << std::endl;

    // Add the recharge value if we have a span.
    if ( true == hasTimeSpan )
    {
      Usul::Functions::safeCallR1R2R3 ( &Helper::addRecharge, span, recharge.second, attribute, "5030312670" );
    }
  }

  // Add the attribute.
  _document->addAttribute ( attribute.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the time-span.
//
///////////////////////////////////////////////////////////////////////////////

bool Recharge::_getTimeSpan ( const std::string &s, TimeSpan &span )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  USUL_ASSERT_SAME_TYPE ( TimeStamp, TimeSpan::first_type );
  USUL_ASSERT_SAME_TYPE ( TimeStamp, TimeSpan::second_type );

  typedef std::vector<std::string> StringList;

  // Initialize the result.
  bool result ( false );

  // Split on the white space.
  StringList sl;
  Usul::Strings::split ( s, ' ', false, sl );
  if ( true == sl.empty() )
    return result;

  // Get the first string.
  const std::string first ( sl.at ( 0 ) );
  if ( true == first.empty() )
    return result;

  // Is there a dash?
  sl.clear();
  Usul::Strings::split ( first, '-', false, sl );
  if ( sl.size() > 1 )
  {
    const std::string a ( sl.at ( 0 ) );
    const std::string b ( sl.at ( 1 ) );
    if ( ( false == a.empty() ) && ( false == b.empty() ) )
    {
      const bool aIsNum ( a.size() == std::count_if ( a.begin(), a.end(), ::isdigit ) );
      const bool bIsNum ( b.size() == std::count_if ( b.begin(), b.end(), ::isdigit ) );
      if ( aIsNum && bIsNum )
      {
        const TimeStamp start ( Usul::Convert::Type<std::string,TimeStamp>::convert ( a ) );
        const TimeStamp end   ( Usul::Convert::Type<std::string,TimeStamp>::convert ( b ) );
        if ( end > start )
        {
          // Assign the first year.
          span.first  = start;

          // The span needs to include the first year as well as the last, so we add one.
          // This will make "2016-2020" become a start value of 2016 that lasts for 5 years.
          span.second = ( end - start ) + 1;
          result = true;
        }
      }
    }
  }

  // No dash...
  else
  {
    const bool isNum ( first.size() == std::count_if ( first.begin(), first.end(), ::isdigit ) );
    if ( isNum )
    {
      span.first  = Usul::Convert::Type<std::string,TimeStamp>::convert ( first );
      span.second = 1;
      result = true;
    }
  }

  // Return the result, which may still be false.
  return result;
}
