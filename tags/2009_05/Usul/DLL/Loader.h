
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that loads a library.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DYNAMIC_LOAD_LIBRARY_LOADER_CLASS_H_
#define _USUL_DYNAMIC_LOAD_LIBRARY_LOADER_CLASS_H_

#include "Usul/Export/Export.h"

#include <string>
#include <list>


namespace Usul {
namespace DLL {


class Library;


struct USUL_EXPORT Loader
{
  typedef std::list<std::string> Path;

  // Load the library.
  static Library *    load ( const std::string &filename );
  static Library *    load ( const std::string &dir, const std::string &filename );
  static Library *    load ( const Path &path,       const std::string &filename );
};


} // namespace DLL
} // namespace Usul


#endif // _USUL_DYNAMIC_LOAD_LIBRARY_LOADER_CLASS_H_
