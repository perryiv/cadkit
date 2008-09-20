
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

#ifndef _USUL_PREDICATES_FILE_EXISTS_H_
#define _USUL_PREDICATES_FILE_EXISTS_H_

// Disable deprecated warning in Visual Studio 8 for fopen
#if defined ( _MSC_VER )
#pragma warning ( disable : 4996 )
#endif

#include <string>
#include <functional>
#include <iostream>

namespace Usul {
namespace Predicates {


struct FileExists : public std::unary_function < std::string, bool >
{
  typedef std::unary_function < std::string, bool > BaseClass;
  typedef BaseClass::argument_type argument_type;
  typedef BaseClass::result_type result_type;

protected:

  struct File
  {
    File ( const std::string &filename ) : _fp ( 0x0 )
    {
      if ( false == filename.empty() )
      {
        _fp = ::fopen ( filename.c_str(), "r" );
      }
    }

    ~File()
    {
      if ( _fp )
      {
        ::fclose ( _fp );
      }
    }

    bool exists() const
    {
      return ( 0x0 != _fp );
    }

  protected:

    File();                 // No default construction.
    File ( const File &f ); // No copy construction.

  private:

    FILE *_fp;
  };

public:

  FileExists()
  {
  }

  static bool test ( const std::string &s )
  {
    try
    {
      File file ( s );
      return file.exists();
    }
    catch ( ... )
    {
    }

    return false;
  }

  bool operator () ( const std::string &s ) const
  {
    return Usul::Predicates::FileExists::test ( s );
  }
};


} // namespace Predicates
} // namespace Usul


#endif // _USUL_PREDICATES_FILE_EXISTS_H_
