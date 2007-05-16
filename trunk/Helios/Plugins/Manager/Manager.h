
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_PLUGINS_MANAGER_H__
#define __CADKIT_HELIOS_PLUGINS_MANAGER_H__

#include "Export.h"

#include <string>
#include <map>

namespace Usul { namespace Interfaces { struct IUnknown; } }

namespace XmlTree { class Node; }

namespace CadKit {
namespace Helios {
namespace Plugins {
namespace Manager {

class HELIOS_PLUGINS_EXPORT Manager
{
public:

	/// Return the instance.
	static Manager& instance();
	
	/// Get/Set the filename to parse.
	void                      filename ( const std::string& filename );
	const std::string&        filename() const;

	/// Parse the file.
	void                      parse();
	
	/// Load all the plugins.
	void                      load( Usul::Interfaces::IUnknown *caller = 0x0 );
	
protected:
	void                      _addPlugins ( XmlTree::Node &node );
	void                      _addPlugin  ( XmlTree::Node &node );
	
private:
	typedef std::map < std::string, bool > Names;
	
	Manager();
	~Manager();
	
	std::string     _filename;
	Names           _names;
	
	static Manager *_instance;
};


}
}
}
}


#endif // __CADKIT_HELIOS_PLUGINS_MANAGER_H__
