
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
//  Note: sensible output in multi-threaded environment requires first 
//  building the std::ostringstream and then pushing this all at once to 
//  std::cout.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTIONS_SAFE_CALL_H_
#define _USUL_FUNCTIONS_SAFE_CALL_H_

#include <stdexcept>
#include <iostream>
#include <sstream>


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
    std::ostringstream out;
    out << "Error " << ( ( 0x0 == id ) ? "3021688577" : id ) << ": " << e.what() << '\n';
    std::cout << out.str() << std::flush;
  }
  catch ( ... )
  {
    std::ostringstream out;
    out << "Error " << id << ": exception generated when calling wrapped function" << '\n';
    std::cout << out.str() << std::flush;
  }
}


template < class F, class T1 > void safeCallR1 ( F function, T1 &t1, const char *id = 0x0 )
{
  try
  {
    function ( t1 );
  }
  catch ( std::exception &e )
  {
    std::ostringstream out;
    out << "Error " << ( ( 0x0 == id ) ? "2847149640" : id ) << ": " << e.what() << '\n';
    std::cout << out.str() << std::flush;
  }
  catch ( ... )
  {
    std::ostringstream out;
    out << "Error " << id << ": exception generated when calling wrapped function" << '\n';
    std::cout << out.str() << std::flush;
  }
}


template < class F, class T1 > void safeCallV1 ( F function, T1 t1, const char *id = 0x0 )
{
  try
  {
    function ( t1 );
  }
  catch ( std::exception &e )
  {
    std::ostringstream out;
    out << "Error " << ( ( 0x0 == id ) ? "6891331220" : id ) << ": " << e.what() << '\n';
    std::cout << out.str() << std::flush;
  }
  catch ( ... )
  {
    std::ostringstream out;
    out << "Error " << id << ": exception generated when calling wrapped function" << '\n';
    std::cout << out.str() << std::flush;
  }
}


template < class F, class T1, class T2 > void safeCallR1R2 ( F function, T1 &t1, T2 &t2, const char *id = 0x0 )
{
  try
  {
    function ( t1, t2 );
  }
  catch ( std::exception &e )
  {
    std::ostringstream out;
    out << "Error " << ( ( 0x0 == id ) ? "1295065733" : id ) << ": " << e.what() << '\n';
    std::cout << out.str() << std::flush;
  }
  catch ( ... )
  {
    std::ostringstream out;
    out << "Error " << id << ": exception generated when calling wrapped function" << '\n';
    std::cout << out.str() << std::flush;
  }
}


template < class F, class T1, class T2 > void safeCallV1V2 ( F function, T1 t1, T2 t2, const char *id = 0x0 )
{
  try
  {
    function ( t1, t2 );
  }
  catch ( std::exception &e )
  {
    std::ostringstream out;
    out << "Error " << ( ( 0x0 == id ) ? "2683524620" : id ) << ": " << e.what() << '\n';
    std::cout << out.str() << std::flush;
  }
  catch ( ... )
  {
    std::ostringstream out;
    out << "Error " << id << ": exception generated when calling wrapped function" << '\n';
    std::cout << out.str() << std::flush;
  }
}


template < class F, class T1, class T2, class T3 > void safeCallR1R2R3 ( F function, T1 &t1, T2 &t2, T3 &t3, const char *id = 0x0 )
{
  try
  {
    function ( t1, t2, t3 );
  }
  catch ( std::exception &e )
  {
    std::ostringstream out;
    out << "Error " << ( ( 0x0 == id ) ? "4105045581" : id ) << ": " << e.what() << '\n';
    std::cout << out.str() << std::flush;
  }
  catch ( ... )
  {
    std::ostringstream out;
    out << "Error " << id << ": exception generated when calling wrapped function" << '\n';
    std::cout << out.str() << std::flush;
  }
}


} // namespace Functions
} // namespace Usul


#endif // _USUL_FUNCTIONS_SAFE_CALL_H_
