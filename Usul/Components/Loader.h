
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

#include "Usul/Components/Manager.h"
#include "Usul/Config/Config.h"
#include "Usul/DLL/Loader.h"
#include "Usul/DLL/Library.h"
#include "Usul/DLL/LibraryPool.h"
#include "Usul/File/Path.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Trace/Trace.h"

#include <string>
#include <vector>

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

  // Add a library to load.
  void                      addLibrary ( const std::string &file );
  
  // Set the directory.
  void                      directory ( const std::string& dir );

  // Get the mutex.
  Mutex &                   mutex() const { return _mutex; }

  /// Parse the file.
  void                      parse ( const std::string& filename );
	
  /// Load all the plugins.
  void                      load ( Usul::Interfaces::IUnknown *caller = 0x0 );
	
protected:

  struct PluginInfo
  {
    PluginInfo() :
      name ( "" ),
      load ( true )
    {
    }
    
    std::string name;
    bool load;
  };

  void                      _addPlugins ( const Node &node );
  void                      _addPlugin  ( const Node &node );
  void                      _addLibrary ( const Node &node );
  void                      _addPlugin  ( PluginInfo &plugin );
	
private:

  typedef std::vector<PluginInfo> Plugins;
  typedef std::vector<std::string> Libraries;
	
  std::string     _directory;
  Plugins         _plugins;
  Libraries       _libraries;
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
  _libraries(),
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
inline void Loader< Document >::parse ( const std::string& filename )
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
inline void Loader< Document >::_addPlugins ( const Node &parent )
{
  USUL_TRACE_SCOPE;

  typedef typename Document::Children Children;
  const Children& children ( parent.children() );
	
  for ( typename Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    typename Node::RefPtr node ( (*iter).get() );
    if ( "plugin" == node->name() )
    {
      this->_addPlugin ( *node );
    }
    if ( "library" == node->name() )
      this->_addLibrary ( *node );
    else if ( "directory" == node->name() )
    {
      Guard guard ( this->mutex() );
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
inline void Loader< Document >::_addPlugin ( const Node &node )
{
  USUL_TRACE_SCOPE;

  typedef typename Document::Attributes Attributes;
  const Attributes& attributes ( node.attributes() );
	
  PluginInfo plugin;

  for ( typename Attributes::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "file" == iter->first )
    {
      plugin.name = iter->second;
    }
    else if ( "load" == iter->first )
    {
      std::string load ( iter->second );

      // Should we load.  Default is yes.
      if ( "false" == load )
        plugin.load = false;
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
inline void Loader< Document >::_addLibrary ( const Node &node )
{
  USUL_TRACE_SCOPE;

  typedef typename Document::Attributes Attributes;
  const Attributes& attributes ( node.attributes() );

  for ( typename Attributes::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "file" == iter->first )
    {
      this->addLibrary ( iter->second );
    }
  }
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

  // Strip any extension provided in the xml file.  This is to support legacy plugin files.
  const std::string name ( Usul::File::base ( plugin.name ) );

#if _DEBUG
  plugin.name = name + USUL_PLUGIN_POSTFIX_DEBUG + "." + USUL_PLUGIN_EXTENSION_DEBUG;
#else
  plugin.name = name + "." + USUL_PLUGIN_EXTENSION_RELEASE;
#endif
  
  Guard guard ( this->mutex() );
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

  PluginInfo plugin;
  plugin.name = file;
  plugin.load = true;

  this->_addPlugin ( plugin );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a plugin.
//
///////////////////////////////////////////////////////////////////////////////

template < class Document >
inline void Loader< Document >::addLibrary ( const std::string &file )
{
  USUL_TRACE_SCOPE;

  // Filename for the current platform.
  std::string name ( file );

  // Prepend 'lib' if not on windows.
#ifndef _MSC_VER
  name = "lib" + name;
#endif
  
  // Add a d if building debug.
#if _DEBUG
  name += 'd';
#endif

  // Add the correct extension.
#ifdef _MSC_VER
  name += ".dll";
#elif __APPLE__
  name += ".dylib";
#else
  name += ".so";
#endif

  Guard guard ( this->mutex() );
  _libraries.push_back ( name );
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

      // Load the plugin.
      Usul::Components::Manager::instance().load ( Usul::Interfaces::IPlugin::IID, name );
    }
 
    // Update progress.
    if ( progress.valid() )
      progress->updateProgressBar ( ++number );
  }

  // Load the libraries.
  for ( typename Libraries::const_iterator iter = _libraries.begin(); iter != _libraries.end(); ++iter )
  {
    // Get the name.
    std::string name ( *iter );

    // Let the user know what library we are loading.
    status ( "Loading " + name + "...", true );

    // Load.
    try
    {
      // Prepend the directory if we have one...
      if( false == _directory.empty() )
	      name = _directory + name;
      
      Usul::DLL::Library::RefPtr library ( Usul::DLL::Loader::load ( name ) );
      
      // Add to our cache.
      Usul::DLL::LibraryPool::instance().add ( library );
    }
    catch ( const std::exception &e )
    {
      std::cout << "Error 2012373085: Standard exception caught while trying to load library " << name << ": " << e.what() << std::endl;
    }
    catch ( ... )
    {
      std::cout << "Error 1001727732: Unknown exception caught while trying to load library " << name << "." << std::endl;
    }
  }
}
  
  
///////////////////////////////////////////////////////////////////////////////
//
//  Set the directory.
//
///////////////////////////////////////////////////////////////////////////////

template < class Document >
inline void Loader< Document >::directory ( const std::string& dir )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _directory = dir;
}


} // namespace Components
} // namespace Usul


#endif // __CADKIT_HELIOS_PLUGINS_MANAGER_H__
