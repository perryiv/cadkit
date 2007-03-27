
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that loads a library.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/DLL/Loader.h"
#include "Usul/DLL/Library.h"
#include "Usul/DLL/Exceptions.h"

#include "Usul/Errors/Assert.h"

#include "Usul/Exceptions/Thrower.h"

#include "Usul/System/LastError.h"

using namespace Usul;
using namespace Usul::DLL;


///////////////////////////////////////////////////////////////////////////////
//
//  Define the slash.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# define DELIMITER_SLASH '\\'
#else
# define DELIMITER_SLASH '/'
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Load the library.
//
///////////////////////////////////////////////////////////////////////////////

Library *Loader::load ( const std::string &filename )
{
  // The constructor throws if it fails to load, 
  // and the pointer throws if given null.
  Library::ValidRefPtr lib ( new Library ( filename ) );
  return lib.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the library.
//
///////////////////////////////////////////////////////////////////////////////

Library *Loader::load ( const std::string &dir, const std::string &filename )
{
  // Make a copy because we may change it.
  std::string directory ( dir );

  // If we have a valid string for the directory...
  if ( false == directory.empty() )
  {
    // Append a slash if needed.
    std::string::size_type last ( directory.size() - 1 );
    if ( DELIMITER_SLASH != directory[last] )
      directory += DELIMITER_SLASH;
  }

  // Call the other one.
  return Loader::load ( directory + filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the library.
//
///////////////////////////////////////////////////////////////////////////////

Library *Loader::load ( const Path &path, const std::string &filename )
{
  // Loop through the path.
  for ( Path::const_iterator i = path.begin(); i != path.end(); ++i )
  {
    // Get the directory.
    const std::string &dir = *i;

    // Safely...
    try
    {
      // Call the other one.
      Library::ValidRefPtr lib ( Loader::load ( dir, filename ) );
      return lib.release();
    }

    // Catch (and eat) expected exceptions.
    catch ( const Usul::DLL::Exceptions::FailedToLoad & )
    {
      // Do nothing, just try the next one...
    }
  }

  // If we get to here then we failed. Turn the path into a readable string.
  std::string libPath;
  for ( Path::const_iterator i = path.begin(); i != path.end(); ++i )
  {
    libPath += "\n\t";
    libPath += *i;
  }

  // Throw the exception.
  Usul::Exceptions::Thrower<Usul::DLL::Exceptions::FailedToLoad>
    ( "Error: 2751266987, failed to load library: ", filename,
      "\n\tPath used:            ", libPath,
      "\n\tSystem error number:  ", System::LastError::number(),
      "\n\tSystem error message: ", System::LastError::message() );

  // The compiler does not know that the above "Thrower" constructor throws 
  // an exception, and thus, we never get here.
  USUL_ASSERT ( 0 );
  return 0x0;
}
