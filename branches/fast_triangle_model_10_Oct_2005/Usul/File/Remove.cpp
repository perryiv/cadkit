
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

void Usul::File::remove ( const std::string &name )
{
  // Make sure there is no file with the given name.
  if ( Usul::Predicates::FileExists() ( name ) )
  {
    if ( 0 != ::remove ( name.c_str() ) )
    {
      // Format the error message.
      std::ostringstream out;
      out << "Error 1607791450: Failed to remove existing file: " << name;

      // Append any system error...
      if ( Usul::System::LastError::number() )
        out << "\n  Reason: " << Usul::System::LastError::message();

      // Throw the error.
      throw std::runtime_error ( out.str() );
    }
  }
}
