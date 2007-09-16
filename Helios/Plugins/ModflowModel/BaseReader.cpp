
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all readers.
//
///////////////////////////////////////////////////////////////////////////////

#include "BaseReader.h"
#include "ModflowDocument.h"

#include "Usul/Exceptions/Thrower.h"
#include "Usul/Trace/Trace.h"
#include "Usul/System/LastError.h"

#include "boost/algorithm/string/trim.hpp"


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
  _document ( 0x0 )
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

void BaseReader::_set ( ModflowDocument *document, Unknown *progress )
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

    std::istringstream line ( &buffer[0] );
    line >> s;

    if ( word == s )
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

void BaseReader::_readGrid ( GridInfo &grid )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Read the name.
  std::string dummy;
  _in >> dummy >> dummy >> dummy >> dummy;
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
