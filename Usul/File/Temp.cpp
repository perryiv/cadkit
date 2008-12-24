
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Temporary file.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/File/Temp.h"
#include "Usul/File/Path.h"
#include "Usul/File/Rename.h"
#include "Usul/System/LastError.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/MPL/StaticAssert.h"

#include <stdio.h>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <cstring>

#ifdef _MSC_VER // Visual C++
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

using namespace Usul;
using namespace Usul::File;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Temp::Temp ( Format f ) : 
  _name ( Temp::file() ),
  _stream ( 0x0 ),
  _remove ( true )
{
  // Initialize the last error.
  typedef Usul::System::LastError LastError;
  LastError::init();

  // Open the file in the proper mode.
  std::ofstream *stream ( 0x0 );
  if ( Temp::BINARY == f )
    stream = new std::ofstream ( this->name().c_str(), std::ofstream::out | std::ofstream::binary );
  else
    stream = new std::ofstream ( this->name().c_str() );

  // Assign stream.
  _stream = stream;

  // Check.
  if ( 0x0 == stream || false == stream->is_open() )
  {
    // Format the error message.
    std::ostringstream out;
    out << "Error 1952309409: Failed to open temporary file: " << this->name();

    // Append any system error...
    if ( LastError::number() )
      out << "\n  Reason: " << LastError::message();

    // Throw the error.
    throw std::runtime_error ( out.str() );
  }
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Temp::~Temp()
{
  try
  {
    // Make sure the stream is closed.
    this->close();

    // Remove the temporary file if we should.
    if ( _remove )
    {
      const int result ( ::remove ( this->name().c_str() ) );
      if ( result != 0 )
      {
        Usul::Errors::Stack::instance().push ( "Error 2034122402: Failed to remove temporary file: " + this->name() );
      }
    }
  }

  catch ( const std::exception &e )
  {
    Usul::Errors::Stack::instance().push ( e.what() );
    Usul::Errors::Stack::instance().push ( "Error 2355835989: Standard exception caught while removing temporary file: " + this->name() );
  }

  catch ( ... )
  {
    Usul::Errors::Stack::instance().push ( "Error 2036890444: Unknown exception caught while removing temporary file: " + this->name() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a valid temporary file name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Temp::file()
{
  // Initialize the last error.
  typedef Usul::System::LastError LastError;
  LastError::init();

  #ifdef _MSC_VER // Visual C++

  // Work-around for Win32's tmpnam() implementation. It does not consider if 
  // the directory is write-protected.

  // Compile-time sanity check.
  const unsigned int bufSize ( 16383 ); // 2^14 - 1
  USUL_STATIC_ASSERT ( bufSize > MAX_PATH );

  // Get the directory where temporary files can be created.
  char directory[bufSize + 1];
  DWORD result ( ::GetTempPath ( bufSize, directory ) );

  // Check for errors.
  if ( 0 == result || bufSize < result )
    throw std::runtime_error ( std::string ( "Error 3143231617: failed get path for temporary files. " ) + LastError::message() );

  // Get the file name.
  char pathname[bufSize + 1];
  result = ::GetTempFileName ( directory, "temp_", 0, pathname );

  // Check for errors.
  if ( 0 == result )
    throw std::runtime_error ( std::string ( "Error 2499845465: failed to create temporary file. " ) + LastError::message() );

  // Return the name.
  return std::string ( pathname );

  #else

  return ::tmpnam ( 0x0 );

  #endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close the stream.
//
///////////////////////////////////////////////////////////////////////////////

void Temp::close()
{
  if ( _stream )
  {
    delete _stream;
    _stream = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Release ownership of the temporary file.
//
///////////////////////////////////////////////////////////////////////////////

void Temp::release()
{
  this->close();
  _remove = false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rename the file.
//
///////////////////////////////////////////////////////////////////////////////

void Temp::rename ( const std::string &name )
{
  // Close the file.
  this->close();

  // Try to rename the file.
  Usul::File::rename ( this->name(), name, true );

  // Set this flag so that the destructor does not try to remove the file.
  _remove = false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the name.
//
///////////////////////////////////////////////////////////////////////////////

const std::string &Temp::name() const
{
  return _name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the stream.
//
///////////////////////////////////////////////////////////////////////////////

std::ostream &Temp::stream()
{
  USUL_ASSERT ( 0x0 != _stream );
  return *_stream;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return name of directory where temporary files can be created.
//
///////////////////////////////////////////////////////////////////////////////

std::string Temp::directory ( bool wantSlash )
{
  Temp file;
  const std::string dir ( Usul::File::directory ( file.name(), wantSlash ) );
  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the file if it exists.
//
///////////////////////////////////////////////////////////////////////////////

void Temp::remove ( const std::string &file, bool allowThrow )
{
  try
  {
    if ( false == file.empty() )
    {
      const int result ( ::remove ( file.c_str() ) );
      if ( result != 0 && true == allowThrow )
      {
        throw std::runtime_error ( "Error 2210803015: Failed to remove file: " + file );
      }
    }
  }
  catch ( const std::exception &e )
  {
    if ( true == allowThrow )
      throw e;
    else
      std::cout << "Error 1331448352: Failed to remove file: " << file << std::endl;
  }
  catch ( ... )
  {
    if ( true == allowThrow )
      throw;
    else
      std::cout << "Error 5249566150: Failed to remove file: " << file << std::endl;
  }
}
