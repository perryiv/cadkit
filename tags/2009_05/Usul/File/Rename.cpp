
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/File/Rename.h"
#include "Usul/File/Remove.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/System/LastError.h"

#include <cstdio>
#include <sstream>
#include <stdexcept>


void Usul::File::rename ( const std::string& oldFilename, const std::string& newFilename, bool allowThrow, std::ostream *stream )
{
  // Initialize the last error.
  typedef Usul::System::LastError LastError;
  LastError::init();

  // Make sure there is no file with the given name.
  Usul::File::remove ( newFilename, allowThrow, stream );

  // Try to rename the file.
  if ( 0 != ::rename ( oldFilename.c_str(), newFilename.c_str() ) )
  {
    // Format the error message.
    std::ostringstream out;
    out << "Error 2743740648: Failed to rename file from '" << oldFilename << "' to '" << newFilename;

    // Append any system error...
    if ( LastError::number() )
      out << "\n  Reason: " << LastError::message();

    // Are we supposed to print?
    if ( 0x0 != stream )
    {
      (*stream) << out.str() << std::endl;
    }

    // Throw the error.
    if ( allowThrow )
    {
      throw std::runtime_error ( out.str() );
    }
  }
}
