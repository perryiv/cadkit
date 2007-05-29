
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  File statistics.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_STATISTICS_H_
#define _USUL_FILE_STATISTICS_H_

#include "Usul/Types/Types.h"
#include "Usul/MPL/StaticAssert.h"
#include "Usul/Errors/Assert.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <limits>

#ifdef _WIN32
# define STAT_STRUCT_32      struct _stat32
# define STAT_STRUCT_64      struct _stat64
# define STAT_FUNCTION_32    _stat32
# define STAT_FUNCTION_64    _stat64
#else
# define STAT_STRUCT      struct stat
# define STAT_FUNCTION    ::stat
# define STAT_STRUCT_32     STAT_STRUCT
# define STAT_STRUCT_64     STAT_STRUCT
# define STAT_FUNCTION_32   STAT_FUNCTION
# define STAT_FUNCTION_64   STAT_FUNCTION

#endif


namespace Usul {
namespace File {


///////////////////////////////////////////////////////////////////////////////
//
//  Traits class for selecting the proper function. Add more as necessary.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T > struct Traits;

  template <> struct Traits < Usul::Types::Uint32 >
  {
    typedef STAT_STRUCT_32 StructType;
    static bool stat ( const std::string &file, StructType &s )
    {
      return ( 0 == STAT_FUNCTION_32 ( file.c_str(), &s ) );
    }
  };

  template <> struct Traits < Usul::Types::Uint64 >
  {
    typedef STAT_STRUCT_64 StructType;
    static bool stat ( const std::string &file, StructType &s )
    {
      return ( 0 == STAT_FUNCTION_64 ( file.c_str(), &s ) );
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Struct for getting file statistics.
//
///////////////////////////////////////////////////////////////////////////////

template < class T = Usul::Types::Uint64 > struct Stats
{
  typedef Usul::File::Detail::Traits<T> Traits;
  typedef typename Traits::StructType StructType;

protected:

  template < class T2 > static T _convert ( bool result, T2 value )
  {
      return ( ( result && value > 0 ) ? static_cast < T > ( value ) : 0 );
  }

public:

  //
  //  Get the file size.
  //

  static T size ( const std::string &name )
  {
    StructType filebuf;
    const bool result ( Traits::stat ( name, filebuf ) );
    return _convert ( result, filebuf.st_size );
  }


  //
  //  Get the time of last access.
  //

  static T accessed ( const std::string &name )
  {
    StructType filebuf;
    const bool result ( Traits::stat ( name, filebuf ) );
    return _convert ( result, filebuf.st_atime );
  }


  //
  //  Get the time of last modification.
  //

  static T modified ( const std::string &name )
  {
    StructType filebuf;
    const bool result ( Traits::stat ( name, filebuf ) );
    return _convert ( result, filebuf.st_mtime );
  }


  //
  //  Get the time of creation.
  //
  
  static T created ( const std::string &name )
  {
    StructType filebuf;
    const bool result ( Traits::stat ( name, filebuf ) );
    return _convert ( result, filebuf.st_ctime );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  For backward compatability.
//
///////////////////////////////////////////////////////////////////////////////

inline Usul::Types::Uint64 size ( const std::string &name )
{
  return Usul::File::Stats<Usul::Types::Uint64>::size ( name );
}


} // namespace File
} // namespace Usul


#undef STAT_STRUCT
#undef STAT_FUNCTION


#endif // _USUL_FILE_STATISTICS_H_
