
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Remove the file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_REMOVE_H_
#define _USUL_FILE_REMOVE_H_

#include "Usul/Predicates/FileExists.h"

#include <cstdio>
#include <stdexcept>
#include <string>


namespace Usul {
namespace File {


#if 0
void Usul::File::remove ( const std::string &file, bool allowThrow = false )
#else
inline void remove ( const std::string &file, bool allowThrow = false )
{
  try
  {
    if ( false == file.empty() )
    {
      if ( Usul::Predicates::FileExists::test ( file ) )
      {
        const int result ( ::remove ( file.c_str() ) );
        if ( result != 0 && true == allowThrow )
        {
          throw std::runtime_error ( "Error 2210803015: Failed to remove file: " + file );
        }
      }
    }
  }
  catch ( const std::exception &e )
  {
    if ( true == allowThrow )
      throw e;
    else
      std::cout << "Error 1331448352: Failed to remove file: " << file << std::endl;
  }
  catch ( ... )
  {
    if ( true == allowThrow )
      throw;
    else
      std::cout << "Error 5249566150: Failed to remove file: " << file << std::endl;
  }
}
#endif

}; // namespace File
}; // namespace Usul


#endif // _USUL_FILE_REMOVE_H_
