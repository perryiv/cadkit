
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  File contents.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_CONTENTS_H_
#define _USUL_FILE_CONTENTS_H_

#include "Usul/File/Stats.h"

#include <fstream>


namespace Usul {
namespace File {


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file contents.
//
///////////////////////////////////////////////////////////////////////////////

template < class Container > inline void contents ( const std::string &filename, Container &container )
{
  // Get the size of the file.
  unsigned int size ( Usul::File::size ( filename ) );
  container.reserve ( size );

  // Open the file.
  std::ifstream in ( filename.c_str() );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 1406927723, failed to open file: " + filename );

  // Copy the entire file to the container.
  while ( EOF != in.peek() )
  {
    // Have to cast to character to get newlines.
    char c ( in.get() );
    container.push_back ( c );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file contents but skip any comment lines.
//
///////////////////////////////////////////////////////////////////////////////

inline void contents ( const std::string &filename, char comment, std::string &file )
{
  // Get the size of the file.
  unsigned int size ( Usul::File::size ( filename ) );
  file.reserve ( size );

  // Open the file.
  std::ifstream in ( filename.c_str() );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 1406927723, failed to open file: " + filename );

  // Make a buffer to hold the lines.
  const unsigned int bufSize ( 1023 );
  char buffer[bufSize+1];

  // Copy the entire file to the container.
  while ( EOF != in.peek() )
  {
    // Get a line.
    in.getline ( buffer, bufSize );

    // If it's not a comment character...
    if ( comment != buffer[0] )
    {
      // Append the line.
      file += buffer;
      file += '\n';
    }
  }
}


}; // namespace File
}; // namespace Usul


#endif // _USUL_FILE_CONTENTS_H_
