
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

#include <sys/types.h>
#include <sys/stat.h>
#include <string>

#ifdef _WIN32
# define STAT_STRUCT_64   struct __stat64
# define STAT_STRUCT      struct _stat
# define STAT_FUNCTION_64 _stat64
# define STAT_FUNCTION    _stat
#else
//# define STAT_STRUCT_64   struct stat64
# define STAT_STRUCT      struct stat
//# define STAT_FUNCTION_64 stat64
# define STAT_FUNCTION    ::stat

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
    typedef STAT_STRUCT StructType;
    static bool stat ( const std::string &file, StructType &s )
    {
      return ( 0 == STAT_FUNCTION ( file.c_str(), &s ) );
    }
  };
#ifdef __APPLE__
#warning Darwin does not define stat64
#else 
  template <> struct Traits < Usul::Types::Uint64 >
  {
    typedef STAT_STRUCT_64 StructType;
    static bool stat ( const std::string &file, StructType &s )
    {
      return ( 0 == STAT_FUNCTION_64 ( file.c_str(), &s ) );
    }
  };
  
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Struct for getting file statistics.
//
///////////////////////////////////////////////////////////////////////////////

template < class T = unsigned int > struct Stats
{
  typedef Usul::File::Detail::Traits<T> Traits;
  typedef typename Traits::StructType StructType;

protected:

    template < class T2 > static T _convert ( bool result, T2 value )
  {
#ifdef MSC_VER
      USUL_STATIC_ASSERT ( sizeof ( T ) == sizeof ( T2 ) );
#else
      // Runtime check of integer overflow. Could throw here instead of assert.
      USUL_ASSERT ( value <= ( static_cast < T2 > ( std::numeric_limits<T>::max() ) ) );
#endif
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
};


///////////////////////////////////////////////////////////////////////////////
//
//  For backward compatability.
//
///////////////////////////////////////////////////////////////////////////////

inline unsigned int size ( const std::string &name )
{
  return Usul::File::Stats<unsigned int>::size ( name );
}


} // namespace File
} // namespace Usul


#undef STAT_STRUCT
#undef STAT_STRUCT_64
#undef STAT_FUNCTION
#undef STAT_FUNCTION_64


#endif // _USUL_FILE_STATISTICS_H_
