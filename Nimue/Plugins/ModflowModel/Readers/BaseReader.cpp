
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
//  Base class for all readers.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowModel/Readers/BaseReader.h"
#include "ModflowModel/ModflowDocument.h"

#include "Usul/Exceptions/Thrower.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"
#include "Usul/System/LastError.h"

#include "boost/algorithm/string/find.hpp"
#include "boost/algorithm/string/trim.hpp"

#include "ogr_spatialref.h"
#include "cpl_error.h"

#include <sstream>

using namespace Modflow::Readers;

USUL_IMPLEMENT_TYPE_ID ( BaseReader );


///////////////////////////////////////////////////////////////////////////////
//
//  Constants.
//
///////////////////////////////////////////////////////////////////////////////

namespace Modflow
{
  const unsigned int LINE_BUFFER_SIZE ( 512 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

BaseReader::BaseReader() : BaseClass(),
  _file(),
  _in(),
  _progress ( 0x0 ),
  _document ( 0x0 ),
  _fileAttributes(),
  _registryPath()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

BaseReader::~BaseReader()
{
  USUL_TRACE_SCOPE;
  _file.clear();
  _in.close();
  _progress = 0x0;
  Usul::Pointers::unreference ( _document ); _document = 0x0;
  _fileAttributes.clear();
  _registryPath.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the file mode.
//
///////////////////////////////////////////////////////////////////////////////

void BaseReader::_open ( const std::string &name )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _file = name;
  Usul::System::LastError::init();

  _in.open ( _file.c_str() );

  if ( false == _in.is_open() )
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 1296046214: failed to open file '", _file, "', ", Usul::System::LastError::message() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the document and progress interfaces.
//
///////////////////////////////////////////////////////////////////////////////

void BaseReader::_set ( Modflow::ModflowDocument *document, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _progress = progress;

  Usul::Pointers::unreference ( _document );
  _document = document;
  Usul::Pointers::reference ( _document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Seek past all lines that start with the given character.
//
///////////////////////////////////////////////////////////////////////////////

void BaseReader::_skipLines ( char c )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  std::vector<char> buffer ( Modflow::LINE_BUFFER_SIZE, '\0' );

  while ( false == _in.eof() )
  {
    const char next ( _in.peek() );
    if ( next == c )
    {
      _in.getline ( &buffer[0], buffer.size() - 1 );
    }
    else
    {
      return;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the stream state.
//
///////////////////////////////////////////////////////////////////////////////

void BaseReader::_checkStream()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  if ( true == _in.eof() )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 2214077531: unexpected end-of-file encountered while reading file: ", _file );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Seek until you read the given word. Put cursor at beginning of the line.
//
///////////////////////////////////////////////////////////////////////////////

void BaseReader::_seekToLine ( const std::string &word )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  std::vector<char> buffer ( Modflow::LINE_BUFFER_SIZE, '\0' );
  std::string s;

  while ( false == _in.eof() )
  {
    std::fill ( buffer.begin(), buffer.end(), '\0' );

    const std::istream::pos_type pos ( _in.tellg() );
    _in.getline ( &buffer[0], buffer.size() - 1 );
    const std::string line ( buffer.begin(), buffer.end() );

    // Return value is convertable to bool.
    if ( boost::algorithm::find_first ( line, word ) )
    {
      _in.seekg ( pos );
      return;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the array.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class ArrayType > inline void readAsciiArray ( ArrayType &data, std::istream &in )
  {
    USUL_TRACE_SCOPE_STATIC;
    const unsigned int num ( data.size() );
    for ( unsigned int i = 0; i < num; ++i )
    {
      in >> data[i];
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the grid.
//
///////////////////////////////////////////////////////////////////////////////

void BaseReader::_readGrid ( unsigned int numWordsToSkip, GridInfo &grid )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Read the name.
  std::string dummy;
  for ( unsigned int i = 0; i < numWordsToSkip; ++i )
    _in >> dummy;
  this->_checkStream();
  std::string name ( this->_getLine() );
  boost::algorithm::trim ( name );

  // Make the map entry and get reference to data.
  grid.first = name;
  GridData &data = grid.second;

  // Read the grid points.
  const unsigned int numCells ( grid.second.size() );
  data.resize ( numCells );
  Helper::readAsciiArray ( data, _in );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line.
//
///////////////////////////////////////////////////////////////////////////////

std::string BaseReader::_getLine()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  this->_checkStream();

  std::vector<char> buffer ( Modflow::LINE_BUFFER_SIZE, '\0' );
  _in.getline ( &buffer[0], buffer.size() - 1 );

  return std::string ( &buffer[0] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Count the lines with the given substring.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int BaseReader::_countLines ( const std::string &label, const std::string &file, bool atStart )
{
  USUL_TRACE_SCOPE_STATIC;

  // Initialize the count.
  unsigned int count ( 0 );

  // Handle bad state.
  std::ifstream in ( file.c_str() );
  if ( false == in.is_open() )
    return count;

  // Declare outside of the loop.
  std::vector<char> buffer ( Modflow::LINE_BUFFER_SIZE, '\0' );

  // While we're not at the end of the file.
  while ( false == in.eof() )
  {
    // Fill the vector.
    buffer.assign ( Modflow::LINE_BUFFER_SIZE, '\0' );

    // Get the line.
    in.getline ( &buffer[0], buffer.size() - 1 );

    // Just checking the first word?
    if ( true == atStart )
    {
      // Get the first word.
      std::istringstream in ( &buffer[0] );
      std::string word;
      in >> word;

      // If the first word is the label, increment the count.
      if ( label == word )
      {
        ++count;
      }
    }
    
    // Or, anywhere in the line?
    else
    {
      const std::string line ( buffer.begin(), buffer.end() );

      // Return value is convertable to bool.
      if ( boost::algorithm::find_first ( line, label ) )
      {
        ++count;
      }
    }
  }
  
  // Return the count.
  return count;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the "no data" value.
//
///////////////////////////////////////////////////////////////////////////////

void BaseReader::noDataSet ( Modflow::ModflowDocument *doc, long value )
{
  USUL_TRACE_SCOPE;

  if ( ( 0x0 != doc ) && ( false == this->name().empty() ) )
  {
    doc->noDataSet ( this->name(), value );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the file attributes.
//
///////////////////////////////////////////////////////////////////////////////

void BaseReader::fileAttributes ( const Modflow::Readers::BaseReader::FileAttributes &a )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _fileAttributes = a;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file attribute.
//
///////////////////////////////////////////////////////////////////////////////

std::string BaseReader::fileAttribute ( const std::string &key ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  FileAttributes::const_iterator i ( _fileAttributes.find ( key ) );
  return ( ( _fileAttributes.end() == i ) ? std::string() : i->second );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the coordinate system.
//
///////////////////////////////////////////////////////////////////////////////

OGRSpatialReference * BaseReader::_coordinateSystem() const
{
  USUL_TRACE_SCOPE;
  
  // Look for coordinate system.
  const std::string wkt ( this->fileAttribute ( Modflow::Attributes::WELL_KNOWN_TEXT ) );
  if ( false == wkt.empty() )
  {
    // Make a new spatial reference system.
    std::auto_ptr<OGRSpatialReference> srs ( new OGRSpatialReference );
    std::vector<char> text ( wkt.size() + 1, '\0' );
    std::copy ( wkt.begin(), wkt.end(), text.begin() );
    char *ptr ( &text[0] );
    if ( CE_None == srs->importFromWkt ( &ptr ) )
      return srs.release();
  }
  
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the registry path.
//
///////////////////////////////////////////////////////////////////////////////

void BaseReader::registryPath ( const RegistryPath &path )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _registryPath = path;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the registry path.
//
///////////////////////////////////////////////////////////////////////////////

BaseReader::RegistryPath BaseReader::registryPath() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _registryPath;
}
