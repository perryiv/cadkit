
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_FUNCTIONS_TO_STRING_H__
#define __USUL_FUNCTIONS_TO_STRING_H__

#include <string>
#include <sstream>

namespace Usul
{
  namespace Functions
  {
    template < class T > std::string toString( const T& t )
    {
      std::ostringstream os;
      os << t;
      return os.str();
    }
  }
}

#endif //__USUL_FUNCTIONS_TO_STRING_H__
