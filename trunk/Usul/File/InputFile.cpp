
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for input files.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/File/InputFile.h"
#include "Usul/Exceptions/Exception.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/LastError.h"

#include <fstream>
#include <limits>

using namespace Usul::File;

USUL_IMPLEMENT_TYPE_ID ( InputFile );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

InputFile::InputFile ( const std::string &name, bool binary ) : BaseClass ( name ),
  _file ( 0x0 )
{
  // Initialize last error.
  Usul::System::LastError::init();

  // Set the mode.
  const std::ifstream::openmode mode ( std::fstream::in | ( ( true == binary ) ? std::fstream::binary : 0 ) );

  // Open the file.
  _file = new std::ifstream ( name.c_str(), mode );
  if ( false == _file->is_open() )
  {
    const std::string message ( ( true == Usul::System::LastError::has() ) ? 
      ( std::string ( "System error: " ) + Usul::System::LastError::message() ) : std::string() );
    throw Usul::Exceptions::Exception ( Usul::Strings::format ( "Error 4570578960: Failed to open file: ", name, message ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

InputFile::~InputFile()
{
  delete _file;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the remainder of the file.
//
///////////////////////////////////////////////////////////////////////////////

void InputFile::read ( Buffer &buffer )
{
  // Initialize.
  buffer.clear();

  // Get the size of the file.
  SizeType size ( this->size() );

  // Handle zero-sized files as a special case because "&buffer[0]" is invalid.
  if ( 0 == size )
    return;

  // See if it is too big.
  if ( size > static_cast<SizeType> ( std::numeric_limits<Buffer::size_type>::max() ) )
  {
    throw Usul::Exceptions::Exception ( Usul::Strings::format 
      ( "Error 1288933473: File '", this->name(), "' is too big for buffer" ) );
  }

  // Call virtual function.
  this->_read ( buffer, size );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the remainder of the file.
//
///////////////////////////////////////////////////////////////////////////////

void InputFile::read ( std::string &contents )
{
  // Initialize.
  contents.clear();

  // Make the buffer.
  Buffer buffer;
  this->read ( buffer );

  // Copy to the string if there is content.
  if ( false == buffer.empty() )
  {
    contents.reserve ( buffer.size() );
    contents.assign ( buffer.begin(), buffer.end() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the stream position at the end of the file?
//
///////////////////////////////////////////////////////////////////////////////

bool InputFile::endOfFile() const
{
  return ( EOF == this->_getFile().peek() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file.
//
///////////////////////////////////////////////////////////////////////////////

std::ifstream &InputFile::_getFile() const
{
  return *_file;
}
