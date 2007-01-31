
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Error-checking class that prints to standard error.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ERROR_POLICY_PRINT_H_
#define _USUL_ERROR_POLICY_PRINT_H_

#include <iostream>


namespace Usul {
namespace Errors {


struct PrintingPolicy
{
  PrintingPolicy ( const char *filename, unsigned int line, bool state )
  {
    if ( !state )
      std::cerr << "Error!\n\tLine: " << line << "\n\tFile: " << filename << std::endl;
  }
  PrintingPolicy ( unsigned int id, bool state )
  {
    if ( !state )
      std::cerr << "Error = " << id << std::endl;
  }
  template < class Arg_ > PrintingPolicy ( unsigned int id, bool state, const Arg_ &arg )
  {
    if ( !state )
      std::cerr << "Error = " << id << "\n\t" << arg << std::endl;
  }
  template < class Arg_ > PrintingPolicy ( const char *filename, unsigned int line, bool state, const Arg_ &arg )
  {
    if ( !state )
      std::cerr << "Error!\n\tLine: " << line << "\n\tFile: " << filename << "\n\t" << arg << std::endl;
  }
  template < class Int_ > static void bounds ( unsigned int id, Int_ size, Int_ index )
  {
    if ( index < 0 || index >= size )
      std::cerr << "Index bounds error = " << id << std::endl;
  }
};


}; // namespace Errors
}; // namespace Usul


#endif // _USUL_ERROR_POLICY_PRINT_H_
