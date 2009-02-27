
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for reading a binary file.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/File/BinaryInputFile.h"
#include "Usul/Exceptions/Exception.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/LastError.h"

#include <fstream>
#include <limits>

using namespace Usul::File;

USUL_IMPLEMENT_TYPE_ID ( BinaryInputFile );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BinaryInputFile::BinaryInputFile ( const std::string &name ) : BaseClass ( name ),
  _file ( 0x0 )
{
  // Open the file.
  Usul::System::LastError::init();
  _file = new std::ifstream ( name.c_str(), std::fstream::in | std::fstream::binary );
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

BinaryInputFile::~BinaryInputFile()
{
  delete _file;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the remainder of the file.
//
///////////////////////////////////////////////////////////////////////////////

void BinaryInputFile::read ( Buffer &buffer )
{
  // Initialize.
  buffer.clear();

  // Get the size of the file.
  SizeType size ( this->size() );

  // See if it is too big.
  if ( size > static_cast<SizeType> ( std::numeric_limits<Buffer::size_type>::max() ) )
  {
    throw Usul::Exceptions::Exception ( Usul::Strings::format 
      ( "Error 1288933473: File '", this->name(), "' is too big for buffer" ) );
  }

  // Size the given buffer so that it can hold the file contents.
  buffer.resize ( static_cast < Buffer::size_type > ( size ), '\0' );

  // Read the entire file.
  WriteLock lock ( this );
  {
    _file->read ( &buffer[0], buffer.size() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the remainder of the file.
//
///////////////////////////////////////////////////////////////////////////////

void BinaryInputFile::read ( std::string &contents )
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
//  Read the entire file.
//
///////////////////////////////////////////////////////////////////////////////

void BinaryInputFile::read ( const std::string &file, Buffer &buffer, bool throwIfError )
{
  // Initialize.
  buffer.clear();

  USUL_TRY_BLOCK
  {
    BinaryInputFile::RefPtr file ( new BinaryInputFile ( file ) );
    file->read ( buffer );
  }
  catch ( ... )
  {
    if ( true == throwIfError )
    {
      throw;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the entire file.
//
///////////////////////////////////////////////////////////////////////////////

void BinaryInputFile::read ( const std::string &file, std::string &contents, bool throwIfError )
{
  // Initialize.
  contents.clear();

  // Make the buffer.
  Buffer buffer;
  BinaryInputFile::read ( file, buffer, throwIfError );

  // Copy to the string if there is content.
  if ( false == buffer.empty() )
  {
    contents.reserve ( buffer.size() );
    contents.assign ( buffer.begin(), buffer.end() );
  }
}
