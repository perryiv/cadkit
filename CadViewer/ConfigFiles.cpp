
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions pertaining to the configuration files.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "ConfigFiles.h"

#include "Usul/Exceptions/Thrower.h"
#include "Usul/Predicates/FileExists.h"

#include <stdexcept>


using namespace CV;


///////////////////////////////////////////////////////////////////////////////
//
//  Get the configuration file given the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Config::filename ( const std::string &name )
{
  // Look for the home directory.
  const char *h = ::getenv ( "HOME" );
  std::string home ( ( h ) ? h : "" );
  if ( home.empty() )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 2810413288, environment variable 'HOME' is not set." );
  }

  // Append a trailing slash.
  std::string::size_type last ( home.size() - 1 );
  if ( '\\' != home[last] && '/' != home[last] )
    home += '/';

  // The configuration directory.
  std::string config ( ".cadviewer/" );

  // Add the strings.
  std::string path ( home + config + name + ".xml" );

  // Make sure the file exists.
  Usul::Predicates::FileExists exists;
  if ( !exists ( path ) )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 1327897828, configuration file does not exist: ", path );
  }

  // Return the path.
  return path;
}
