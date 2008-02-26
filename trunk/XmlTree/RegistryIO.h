
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that reads and writes Usul's registry.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_TREE_REGISTRY_IO_H_
#define _XML_TREE_REGISTRY_IO_H_

#include "XmlTree/Export.h"

#include <string>

namespace Usul { namespace Registry { class Database; } }

namespace XmlTree {


struct XML_TREE_EXPORT RegistryIO
{
  // Read/write the registry.
  static void    read  ( const std::string &file, Usul::Registry::Database& );
  static void    write ( const std::string &file, Usul::Registry::Database& );
};


} // namespace XmlTree


#endif // _XML_TREE_REGISTRY_IO_H_
