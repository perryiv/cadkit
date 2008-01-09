
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that initializes and cleans up the STXXL library.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SCOPED_STXXL_CONFIG_H_
#define _USUL_SCOPED_STXXL_CONFIG_H_

// #include "stxxl/stxxl.h" <-- Intentionally not including this big file.

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/File/Remove.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/SafeCall.h"


namespace Usul {
namespace Scope {


template < class T > struct StxxlConfig
{
  enum CommonSizes
  {
    MEGA_BYTE = 1024 * 1024,
    GIGA_BYTE = 1000 * MEGA_BYTE // 1024 will overflow
  };

  StxxlConfig ( unsigned int maxNumElements )
  {
    // Make a temporary file.
    _file = Usul::File::Temp::file();

    // Make sure the number of bytes is in multiples of megabytes.
    const stxxl::int64 requested ( std::max<stxxl::int64> ( MEGA_BYTE, maxNumElements * sizeof ( T ) ) );
    const stxxl::int64 bytes ( ( ( requested / MEGA_BYTE ) * MEGA_BYTE ) + MEGA_BYTE );

    // Initialize the library.
    stxxl::config::init ( _file, bytes );

    // This will throw if there is not enough disk space.
    stxxl::vector<T> temp;
  }

  ~StxxlConfig()
  {
    Usul::Functions::safeCall ( stxxl::config::finish, "1829949087" );
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &StxxlConfig::_removeFile ), "2293719233" );
  }

private:

  void _removeFile()
  {
    Usul::File::remove ( _file );
  }

  std::string _file;
};


}; // namespace Scope
}; // namespace Usul


#endif // _USUL_SCOPED_STXXL_CONFIG_H_
