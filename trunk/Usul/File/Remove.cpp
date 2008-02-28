
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Remove the file.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/File/Remove.h"
#include "Usul/System/LastError.h"
#include "Usul/Predicates/FileExists.h"

#include <stdexcept>
#include <sstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the file.
//
///////////////////////////////////////////////////////////////////////////////

void Usul::File::remove ( const std::string &name, bool allowThrow, std::ostream *stream )
{
  try
  {
    // Make sure there is a file with the given name.
    if ( true == Usul::Predicates::FileExists::test ( name ) )
    {
      // Remove the file.
      if ( 0 != ::remove ( name.c_str() ) )
      {
        // Format the error message.
        std::ostringstream out;
        out << "Error 1607791450: Failed to remove file: " << name;

        // Append any system error...
        if ( Usul::System::LastError::number() )
        {
          out << ", Reason: " << Usul::System::LastError::message();
        }

        // Are we supposed to print?
        if ( 0x0 != stream )
        {
          (*stream) << out.str() << std::endl;
        }

        // Are we supposed to throw?
        if ( true == allowThrow )
        {
          throw std::runtime_error ( out.str() );
        }
      }
    }
  }
  catch ( ... )
  {
    if ( true == allowThrow )
    {
      throw;
    }
  }
}
