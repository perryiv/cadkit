
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Misc functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WEBSITE_GENERATOR_FUNCTIONS_
#define _WEBSITE_GENERATOR_FUNCTIONS_

#include <string>

namespace Usul { namespace Registry { class Node; } }


struct Functions
{
  static std::string             directory ( Usul::Registry::Node & );
};


#endif // _WEBSITE_GENERATOR_FUNCTIONS_
