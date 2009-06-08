
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Redirect stdout and stderr to files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_IO_REDIRECT_OUTPUT_H_
#define _USUL_IO_REDIRECT_OUTPUT_H_

#include "Usul/File/Remove.h"

#include <stdio.h>
#include <string>


namespace Usul {
namespace IO {


struct Redirect
{
  Redirect ( const std::string &file, bool removeExisting, bool closeInDestructor ) : 
    _file ( file ), 
    _stdout ( 0x0 ), 
    _stderr ( 0x0 ),
    _close  ( closeInDestructor )
  {
    if ( removeExisting )
      Usul::File::remove ( file );
    _stdout = ::freopen ( file.c_str(), "a+", stdout );
    _stderr = ::freopen ( file.c_str(), "a+", stderr );
    std::ios::sync_with_stdio();
  }

  ~Redirect()
  {
    if ( _close )
    {
      if ( 0x0 != _stdout )
      {
        try
        {
          ::fclose ( _stdout );
        }
        catch ( ... )
        {
        }
      } 
      if ( 0x0 != _stderr )
      {
        try
        {
          ::fclose ( _stderr );
        }
        catch ( ... )
        {
        }
      }
    }
  }

  const std::string &file() const
  {
    return _file;
  }

private:

  std::string _file;
  FILE *_stdout;
  FILE *_stderr;
  bool _close;
};


} // namespace IO
} // namespace Usul


#endif // _USUL_IO_REDIRECT_OUTPUT_H_
