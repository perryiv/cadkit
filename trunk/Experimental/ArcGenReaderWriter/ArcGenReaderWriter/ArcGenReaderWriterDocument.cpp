
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/ArcGenReaderWriter/ArcGenReaderWriter/ArcGenReaderWriterDocument.h"

#include "Usul/Components/Manager.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Format.h"
#include "Usul/Trace/Trace.h"

#include "osg/Vec3d"

#include <iterator>
#include <vector>
#include <fstream>
#include <iostream>


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ArcGenReaderWriterDocument, ArcGenReaderWriterDocument::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

ArcGenReaderWriterDocument::ArcGenReaderWriterDocument() : 
  BaseClass ( "Arc Gen Reader Writer Document" ),
  _positions( 0 ),
  _measurement( 0.0 )
{
  USUL_TRACE_SCOPE;
   
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ArcGenReaderWriterDocument::~ArcGenReaderWriterDocument()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void ArcGenReaderWriterDocument::clear ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ArcGenReaderWriterDocument::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
   case Usul::Interfaces::IArcGenReaderWriter::IID:
    return static_cast < Usul::Interfaces::IArcGenReaderWriter* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ArcGenReaderWriterDocument::canExport ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ArcGenReaderWriterDocument::canInsert ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ArcGenReaderWriterDocument::canOpen ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "gen" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ArcGenReaderWriterDocument::canSave ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "gen" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void ArcGenReaderWriterDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  this->_writePolylineZ( name, caller, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void ArcGenReaderWriterDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

ArcGenReaderWriterDocument::Filters ArcGenReaderWriterDocument::filtersSave() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "Arc Generate (*.gen)", "*.gen" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

ArcGenReaderWriterDocument::Filters ArcGenReaderWriterDocument::filtersExport() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

ArcGenReaderWriterDocument::Filters ArcGenReaderWriterDocument::filtersOpen() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "Arc Generate (*.gen)", "*.gen" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

ArcGenReaderWriterDocument::Filters ArcGenReaderWriterDocument::filtersInsert() const
{
  USUL_TRACE_SCOPE;
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a PolylineZ file
//
///////////////////////////////////////////////////////////////////////////////

void ArcGenReaderWriterDocument::_writePolylineZ( const std::string &filename, Unknown *caller, Unknown *progress ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  std::ofstream outfile;
  outfile.open( filename.c_str() );
  if ( !outfile.is_open() )
    throw std::runtime_error ( "Error 2429662124: Failed to open file: " + filename );

	// Typedefs.
	typedef Usul::Interfaces::IPlanetCoordinates IPlanetCoordinates;
	typedef Usul::Convert::Type<double,std::string> Converter;

  // Get needed interfaces.
  IPlanetCoordinates::QueryPtr planetCoordinates ( Usul::Documents::Manager::instance().activeDocument() );

  // Write the gen file category descriptor.
  outfile << "ID,Length" << std::endl;

  // Write the id number and measurement value.
  outfile << "0," << Converter::convert ( _measurement ) << std::endl;

	// Write out each position.
  for( Positions::const_iterator iter = _positions.begin(); iter < _positions.end(); ++iter )
  {
		// Get the position.
		Positions::value_type position ( *iter );

		// Make a copy.
		Usul::Math::Vec3d from ( position[0], position[1], position[2] );

		Usul::Math::Vec3d out ( 0, 0, 0 );

    // Convert the planet point into lat/lon
    if( true == planetCoordinates.valid() )
    {
      planetCoordinates->convertFromPlanet( from, out );
    }
    else
    {
      out = from;
    }

    // Write the coordinate.
    outfile << Converter::convert ( out[0] ) << "," << Converter::convert ( out[1] ) << std::endl;
  }

  // End of the gen file notation
  outfile << "END" << std::endl;

  outfile.close();
}


///////////////////////////////////////////////////////////////////////////////
//
// set the positions vector
//
///////////////////////////////////////////////////////////////////////////////


void ArcGenReaderWriterDocument::setPolyLineVertices ( Positions p )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _positions = p;
}


///////////////////////////////////////////////////////////////////////////////
//
// set the recorded measurement value
//
///////////////////////////////////////////////////////////////////////////////


void ArcGenReaderWriterDocument::measurement( double m )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _measurement = m;
}
