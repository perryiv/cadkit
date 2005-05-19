
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

#include <stdio.h>
#include <string>


namespace Usul {
namespace IO {


struct Redirect
{
  Redirect ( const std::string &file ) : 
    _file ( file ), 
    _stdout ( ::freopen ( file.c_str(), "a+", stdout ) ), 
    _stderr ( ::freopen ( file.c_str(), "a+", stderr ) )
  {
    std::ios::sync_with_stdio();
  }

  ~Redirect()
  {
    ::fclose ( _stdout );
    ::fclose ( _stderr );
  }

  const std::string &file() const
  {
    return _file;
  }

private:

  std::string _file;
  FILE *_stdout;
  FILE *_stderr;
};


}; // namespace IO
}; // namespace Usul


#endif // _USUL_IO_REDIRECT_OUTPUT_H_
