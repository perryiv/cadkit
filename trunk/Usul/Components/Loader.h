
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Adam Kubach and Perry Miller.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Loader for plugin files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_COMPONENTS_LOADER_H__
#define __USUL_COMPONENTS_LOADER_H__

#include "Usul/Threads/Guard.h"
#include "Usul/Threads/RecursiveMutex.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Trace/Trace.h"

#include <string>
#include <map>

namespace Usul {
namespace Components {


template < class Document >
class Loader
{
public:

  /// Useful typedefs.
  typedef Usul::Threads::RecursiveMutex     Mutex;
  typedef Usul::Threads::Guard<Mutex>       Guard;
  typedef typename Document::NodeType       Node;

  // Constructor and destructor.
	Loader();
	~Loader();

  // Add a single plugin file name.
	void                      addPlugin ( const std::string &file );

  // Get the mutex.
  Mutex &                   mutex() const { return _mutex; }

	/// Parse the file.
	void                      parse( const std::string& filename );
	
	/// Load all the plugins.
	void                      load ( Usul::Interfaces::IUnknown *caller = 0x0 );
	
protected:

  struct PluginInfo
  {
    PluginInfo() :
      name ( "" ),
      alias ( "" ),
      load ( false )
    {
    }
    
    std::string name;
    std::string alias;
    bool load;
  };

  void                      _addPlugins ( Node &node );
	void                      _addPlugin  ( Node &node );
  void                      _addPlugin  ( PluginInfo &plugin );
	
private:

	typedef std::vector < PluginInfo > Plugins;
	
	std::string     _directory;
	Plugins         _plugins;
  mutable Mutex   _mutex;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template < class Document >
inline Loader< Document >::Loader() : 
	_directory(),
	_plugins(),
  _mutex()
{
  USUL_TRACE_SCOPE;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

template < class Document >
inline Loader< Document >::~Loader()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the file.
//
///////////////////////////////////////////////////////////////////////////////

template < class Document >
inline void Loader< Document >::parse( const std::string& filename )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  typename Document::RefPtr doc = new Document;
	doc->load ( filename );

  if ( "plugins" == doc->name() )
  {
    this->_addPlugins ( *doc );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a node.
//
///////////////////////////////////////////////////////////////////////////////

template < class Document >
inline void Loader< Document >::_addPlugins ( Node &parent )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  typedef typename Document::Children Children;
	Children& children ( parent.children() );
	
	for ( typename Children::iterator iter = children.begin(); iter != children.end(); ++iter )
	{
		typename Node::RefPtr node ( (*iter) );
		if ( "plugin" == node->name() )
		{
			this->_addPlugin ( *node );
		}
		else if ( "directory" == node->name() )
		{
			_directory = node->value();
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a plugin.
//
///////////////////////////////////////////////////////////////////////////////

template < class Document >
inline void Loader< Document >::_addPlugin ( Node &node )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  typedef typename Document::Attributes Attributes;
	Attributes& attributes ( node.attributes() );
	
	PluginInfo plugin;

  for ( typename Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
  	if ( "file" == iter->first )
    {
      plugin.name = iter->second;
    }
    else if ( "alias" == iter->first )
    {
      plugin.name = iter->second;
    }
    else if ( "load" == iter->first )
    {
      std::string load ( iter->second );

      // Should we load.  Default is no.
      if ( "true" == load )
        plugin.load = true;
   	}
  }
  
  this->_addPlugin ( plugin );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a plugin.
//
///////////////////////////////////////////////////////////////////////////////

template < class Document >
inline void Loader< Document >::_addPlugin ( PluginInfo &plugin )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

#if _DEBUG
  plugin.name += 'd';
#endif

  _plugins.push_back ( plugin );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a plugin.
//
///////////////////////////////////////////////////////////////////////////////

template < class Document >
inline void Loader< Document >::addPlugin ( const std::string &file )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  PluginInfo plugin;
  plugin.name = file;
  plugin.load = true;

  this->_addPlugin ( plugin );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load all the plugins.
//
///////////////////////////////////////////////////////////////////////////////

template < class Document >
inline void Loader< Document >::load ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Query for interfaces.
	Usul::Interfaces::IStatusBar::UpdateStatusBar status ( caller );
	Usul::Interfaces::IProgressBar::QueryPtr progress ( caller );

	// Set the size for the progress bar.
	if ( progress.valid() )
		progress->setTotalProgressBar ( _plugins.size() );

	// Number we are on.
	unsigned int number ( 0 );

	// Load the plugins.
	for ( typename Plugins::iterator iter = _plugins.begin(); iter != _plugins.end(); ++iter )
	{
		// Load the plugin if we are suppose to.
		if( iter->load )
		{
			// The name.
			std::string name ( iter->name );
			
			// Prepend the directory if we have one...
      if( false == _directory.empty() )
      	name = _directory + name;

			// Let the user know what plugin we are loading.
			status ( "Loading " + name + "...", true );

      // Get the alias.
      std::string alias ( iter->alias );

			// Load the plugin.
			Usul::Components::Manager::instance().load ( Usul::Interfaces::IPlugin::IID, name, alias );
		}

		// Update progress.
		if ( progress.valid() )
			progress->updateProgressBar ( ++number );
	}
}


} // namespace Components
} // namespace Usul


#endif // __CADKIT_HELIOS_PLUGINS_MANAGER_H__
