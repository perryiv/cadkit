
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
#include "Usul/System/LastError.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Predicates/FileExists.h"

#include <stdio.h>
#include <sstream>
#include <fstream>
#include <stdexcept>

using namespace Usul;
using namespace Usul::File;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Temp::Temp ( Format f ) : 
  _name ( ::tmpnam ( 0x0 ) ), 
  _stream ( 0x0 ),
  _remove ( true )
{
  // Initialize the last error.
  typedef Usul::System::LastError LastError;
  LastError::init();

  // Open the file in the proper mode.
  std::ofstream *stream ( 0x0 );
  if ( Temp::BINARY == f )
    stream = new std::ofstream ( _name.c_str(), std::ofstream::out | std::ofstream::binary );
  else
    stream = new std::ofstream ( _name.c_str() );

  // Assign stream.
  _stream = stream;

  // Check.
  if ( 0x0 == stream || false == stream->is_open() )
  {
    // Format the error message.
    std::ostringstream out;
    out << "Error 1952309409: Failed to open temporary file: " << _name;

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
  USUL_EXCEPTION_SAFE_START;

  // Make sure the stream is closed.
  this->_close();

  // Remove the temporary file if we should.
  if ( _remove )
  {
    if ( 0 != ::remove ( _name.c_str() ) )
    {
      Usul::Errors::Stack::instance().push ( 3313814027u, "Failed to remove temporary file: " + _name );
    }
  }

  USUL_EXCEPTION_SAFE_END ( 2355835989 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close the stream.
//
///////////////////////////////////////////////////////////////////////////////

void Temp::_close()
{
  if ( _stream )
  {
    delete _stream;
    _stream = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rename the file.
//
///////////////////////////////////////////////////////////////////////////////

void Temp::rename ( const std::string &name )
{
  // Close the file.
  this->_close();

  // Initialize the last error.
  typedef Usul::System::LastError LastError;
  LastError::init();

  // Make sure there is no file with the given name.
  if ( Usul::Predicates::FileExists() ( name ) )
  {
    if ( 0 != ::remove ( name.c_str() ) )
    {
      // Format the error message.
      std::ostringstream out;
      out << "Error 1836817535: Failed to remove existing file: " << name;

      // Append any system error...
      if ( LastError::number() )
        out << "\n  Reason: " << LastError::message();

      // Throw the error.
      throw std::runtime_error ( out.str() );
    }
  }

  // Try to rename the file.
  if ( 0 != ::rename ( _name.c_str(), name.c_str() ) )
  {
    // Format the error message.
    std::ostringstream out;
    out << "Error 4003249375: Failed to rename temporary file from '" << _name << "' to '" << name;

    // Append any system error...
    if ( LastError::number() )
      out << "\n  Reason: " << LastError::message();

    // Throw the error.
    throw std::runtime_error ( out.str() );
  }

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
