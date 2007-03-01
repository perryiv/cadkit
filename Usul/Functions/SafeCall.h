
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wraps a function call with try-catch block.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTIONS_SAFE_CALL_H_
#define _USUL_FUNCTIONS_SAFE_CALL_H_

#include <stdexcept>
#include <iostream>


namespace Usul {
namespace Functions {


template < class F > void safeCall ( F function, const char *id = 0x0 )
{
  try
  {
    function();
  }
  catch ( std::exception &e )
  {
    std::cout << "Error " << ( ( 0x0 == id ) ? "3021688577" : id ) << ": " << e.what() << std::endl;
  }
  catch ( ... )
  {
    std::cout << "Error " << id << ": exception generated when calling wrapped function" << std::endl;
  }
}


} // namespace Functions
} // namespace Usul


#endif // _USUL_FUNCTIONS_SAFE_CALL_H_
