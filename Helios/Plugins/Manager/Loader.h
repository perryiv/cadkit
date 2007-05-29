
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Loader for plugin files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_PLUGINS_MANAGER_H__
#define __CADKIT_HELIOS_PLUGINS_MANAGER_H__

#include "Helios/Plugins/Manager/Export.h"

#include "Usul/Threads/Guard.h"
#include "Usul/Threads/RecursiveMutex.h"

#include <string>
#include <map>

namespace Usul { namespace Interfaces { struct IUnknown; } }

namespace XmlTree { class Node; }


namespace CadKit {
namespace Helios {
namespace Plugins {
namespace Manager {


class HELIOS_PLUGINS_EXPORT Loader
{
public:

  /// Useful typedefs.
  typedef Usul::Threads::RecursiveMutex     Mutex;
  typedef Usul::Threads::Guard<Mutex>       Guard;

  // Constructor and destructor.
	Loader();
	~Loader();

  // Add a single plugin file name.
	void                      addPlugin ( const std::string &file );

	/// Get/Set the filename to parse.
	void                      filename ( const std::string& filename );
	std::string               filename() const;

  // Get the mutex.
  Mutex &                   mutex() const { return _mutex; }

	/// Parse the file.
	void                      parse();
	
	/// Load all the plugins.
	void                      load ( Usul::Interfaces::IUnknown *caller = 0x0 );
	
protected:

  void                      _addPlugins ( XmlTree::Node &node );
	void                      _addPlugin  ( XmlTree::Node &node );
	
private:

	typedef std::map < std::string, bool > Names;
	
	std::string     _filename;
	Names           _names;
  mutable Mutex   _mutex;
};


} // namespace Manager
} // namespace Plugins
} // namespace Helios
} // namespace CadKit


#endif // __CADKIT_HELIOS_PLUGINS_MANAGER_H__
