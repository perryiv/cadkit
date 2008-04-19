
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Misc functions.
//
///////////////////////////////////////////////////////////////////////////////

#include "Functions.h"

#include "Usul/Registry/Node.h"

#include <algorithm>


///////////////////////////////////////////////////////////////////////////////
//
//  Find the directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::directory ( Usul::Registry::Node &node )
{
  // Get the directory.
  std::string dir ( node.get ( "./" ) );

  // Fix the slashes.
  std::replace ( dir.begin(), dir.end(), '\\', '/' );

  // Make sure we have something.
  dir = ( ( true == dir.empty() ) ? std::string ( "./" ) : dir );

  // Make sure there is a trailing slash.
  const unsigned int last ( dir.size() - 1 );
  if ( '/' != dir.at(last) )
    dir += "/";

  // Return the string.
  return dir;
}
