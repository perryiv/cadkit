
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate class that determines if the file exists.
//  TODO, figure out how to do this without opening/closing the file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREDICATES_DIR_EXISTS_H_
#define _USUL_PREDICATES_DIR_EXISTS_H_

// Disable deprecated warning in Visual Studio 8 for fopen
#if defined ( _MSC_VER ) && _MSC_VER == 1400
#pragma warning ( disable : 4996 )
#endif

#include <string>
#include <functional>
#include <iostream>

namespace Usul {
namespace Predicates {


struct DirExists : public std::unary_function < const std::string &, bool >
{
public:

  DirExists()
  {
  }
  bool IsDirectory ( const std::basic_string<char> &name )
  {
    //SL_ASSERT ( false == name.empty() );

    #ifdef _WIN32

      // Get the file attributes.
      DWORD attributes = ::GetFileAttributesA ( name.c_str() );

      // Did it work?
      if ( -1 == attributes )
        return false;

      // Is it a directory?
      return ( FILE_ATTRIBUTE_DIRECTORY == ( attributes & FILE_ATTRIBUTE_DIRECTORY ) );

      // Does it exist?
      return ( -1 != ::_access ( name.c_str(), 0 ) );

    #else

      TODO

    #endif
  }
  static bool test ( const std::string &s )
  {
    try
    {
      
      return IsDirectory( s );
    }
    catch ( ... )
    {
    }

    return false;
  }

  bool operator () ( const std::string &s ) const
  {
    return Usul::Predicates::DirExists::test ( s );
  }
};


}; // namespace Predicates
}; // namespace Usul


#endif // _USUL_PREDICATES_DIR_EXISTS_H_

/*
///////////////////////////////////////////////////////////////////////////////
//
//  Is the name a directory?
//
///////////////////////////////////////////////////////////////////////////////

bool DirExists ( const std::basic_string<char> &name )
{
  SL_ASSERT ( false == name.empty() );

#ifdef _WIN32

  // Get the file attributes.
  DWORD attributes = ::GetFileAttributesA ( name.c_str() );

  // Did it work?
  if ( -1 == attributes )
    return false;

  // Is it a directory?
  return ( FILE_ATTRIBUTE_DIRECTORY == ( attributes & FILE_ATTRIBUTE_DIRECTORY ) );

  // Does it exist?
  return ( -1 != ::_access ( name.c_str(), 0 ) );

#else

  TODO

#endif
}

*/