
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Components/Manager.h"
#include "Usul/Components/Exceptions.h"
#include "Usul/DLL/Library.h"
#include "Usul/DLL/Loader.h"
#include "Usul/DLL/Exceptions.h"
#include "Usul/Errors/Stack.h"
#include "Usul/File/Find.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Trace/Trace.h"

#include <iostream>
#include <iterator>

using namespace Usul;
using namespace Usul::Components;


///////////////////////////////////////////////////////////////////////////////
//
//  Start of details.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul {
namespace Components {
namespace Helper {


///////////////////////////////////////////////////////////////////////////////
//
//  The library pool.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::DLL::Library::ValidRefPtr LibPtr;
typedef std::set < LibPtr > LibraryPool;
LibraryPool _pool;


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this is a debug build.
//
///////////////////////////////////////////////////////////////////////////////

bool _isDebug()
{
  USUL_TRACE_SCOPE_STATIC;
  #ifdef _DEBUG
    return true;
  #else
    return false;
  #endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a string describing the build mode.
//
///////////////////////////////////////////////////////////////////////////////

std::string _buildMode ( bool debug )
{
  USUL_TRACE_SCOPE_STATIC;
  return ( debug ) ? "Debug" : "Release";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the function or throw if it fails.
//
///////////////////////////////////////////////////////////////////////////////

Usul::DLL::Library::Function _getFunction ( const std::string &name, Usul::DLL::Library *ptr )
{
  USUL_TRACE_SCOPE_STATIC;

  // The constructor will throw if the pointer is null.
  Usul::DLL::Library::ValidRefPtr lib ( ptr );

  // Get the function. Note: g++ does not allow a reinterpret_cast.
  Usul::DLL::Library::Function fun = lib->function ( name );

  // See if we got the function.
  if ( 0x0 == fun )
  {
    Usul::Exceptions::Thrower<Usul::Components::Exceptions::FailedToFindFunction>
      ( "Error: 1028502101, failed to find function.",
        " Name: ", name, " in Library: ", lib->filename() );
  }

  // Return the function pointer.
  return fun;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the build modes.
//
///////////////////////////////////////////////////////////////////////////////

void _checkBuildModes ( bool loader, bool justLoaded, Usul::DLL::Library *ptr )
{
  USUL_TRACE_SCOPE_STATIC;

  // The constructor will throw if the pointer is null.
  Usul::DLL::Library::ValidRefPtr lib ( ptr );

  // Compare the modes.
  if ( loader != justLoaded )
  {
    // Throw the exception.
    Usul::Exceptions::Thrower<Usul::Components::Exceptions::MismatchedBuildModes>
      ( "Error: 4210150186, mismatched build modes.",
        "\n\tBuild mode for loading module is ", Helper::_buildMode ( loader ),
        "\n\tBuild mode for '", lib->filename(), "' is ", Helper::_buildMode ( justLoaded ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  End of details.
//
///////////////////////////////////////////////////////////////////////////////

}
}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member
//
///////////////////////////////////////////////////////////////////////////////

Manager* Manager::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Get the instance
//
///////////////////////////////////////////////////////////////////////////////

Manager& Manager::instance()
{
  USUL_TRACE_SCOPE_STATIC;
  if ( !_instance )
    _instance = new Manager();
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Manager::Manager() :
  _unknowns(),
  _plugExts(),
  _directories()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load plugins in using internal directories and plugin extentions.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::load ( unsigned long iid, bool keepGoingIfException )
{
  USUL_TRACE_SCOPE;

  // Get a list of all the potential plugins.
  std::list<std::string> plugins;
  for ( Directories::const_iterator dir = _directories.begin(); dir != _directories.end(); ++dir )
  {
    for ( PluginExtensions::const_iterator j = _plugExts.begin(); j != _plugExts.end(); ++j )
      Usul::File::find ( *dir, *j, plugins );
  }

  this->load ( iid, plugins, keepGoingIfException );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load all plugins in the list of file names.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::load ( unsigned long iid, const Strings &plugins, bool keepGoingIfException )
{
  USUL_TRACE_SCOPE;

  typedef std::list<std::string>::const_iterator Iterator;

  for ( Iterator i = plugins.begin(); i != plugins.end(); ++i )
  {
    const std::string name ( *i );
    
    try
    {
      // Find the factory
      Usul::Interfaces::IClassFactory::ValidQueryPtr factory ( this->_factory ( name ) );

      // Get the IUnknown
      Usul::Interfaces::IUnknown::QueryPtr unknown ( factory->createInstance ( iid ) );

      // Insert into set of plugins.
      this->addPlugin ( unknown.get() );
    }

    catch ( const std::exception &e )
    {
      std::ostringstream out;
      out << "Error 3422339904: failed to load plugin file: " << name;
      if ( e.what() )
        out << '\n' << e.what();
      std::cout << out.str() << std::endl;
      Usul::Errors::Stack::instance().push ( out.str() );
      if ( !keepGoingIfException )
        throw;
    }

    catch ( ... )
    {
      std::ostringstream out;
      out << "Error 2297148630: failed to load plugin file: " << name;
      std::cout << out.str() << std::endl;
      Usul::Errors::Stack::instance().push ( out.str() );
      if ( !keepGoingIfException )
        throw;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load a plugin with the given filename.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::load ( unsigned long iid, const std::string& file )
{
  USUL_TRACE_SCOPE;

  try
	{
		// Find the factory
  	Usul::Interfaces::IClassFactory::ValidQueryPtr factory ( this->_factory ( file ) );

    // Get the IUnknown
    Usul::Interfaces::IUnknown::QueryPtr unknown ( factory->createInstance ( iid ) );

    // Insert into set of plugins.
    this->addPlugin ( unknown.get() );
  }
	catch ( const std::exception& e )
	{
		std::cout << "Error 4241786283: Exception caught while trying to load " << file << std::endl;
		std::cout << "Message: " << e.what() << std::endl;
	}
	catch ( ... )
	{
		std::cout << "Error 7167132960: Unknown exception caught while trying to load. " << std::endl;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the plugin.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::addPlugin ( Usul::Interfaces::IUnknown::RefPtr unknown )
{
  USUL_TRACE_SCOPE;

  if ( true == unknown.valid() )
  {
    _unknowns.insert ( unknown.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear plugins and release libraries
//
///////////////////////////////////////////////////////////////////////////////

void Manager::clear ( std::ostream *out ) 
{
  USUL_TRACE_SCOPE;

  if ( 0x0 == out )
  {
    _unknowns.clear(); 
    Helper::_pool.clear();
    return;
  }

  while ( false == _unknowns.empty() )
  {
    UnknownSet::iterator i ( _unknowns.begin() );
    Usul::Interfaces::IUnknown::RefPtr unknown ( i->get() );
    Usul::Interfaces::IPlugin::QueryPtr plugin ( unknown.get() );
    if ( plugin.valid() )
      (*out) << "Releasing component: " << plugin->getPluginName() << std::endl;
    _unknowns.erase ( i );
  }

  while ( false == Helper::_pool.empty() )
  {
    Helper::LibraryPool::iterator i ( Helper::_pool.begin() );
    (*out) << "Releasing file: " << (*i)->filename() << std::endl;
    Helper::_pool.erase ( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find first IUnknown with given iid.,
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Manager::getInterface( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  for ( UnknownItr i = _unknowns.begin(); i != _unknowns.end(); ++i )
  {
    IUnknown *u ( (*i).get() );
    if( u->queryInterface( iid ) )
      return (*i).get();
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all IUnknowns with given iid
//
///////////////////////////////////////////////////////////////////////////////

Manager::UnknownSet Manager::getInterfaces ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  UnknownSet set;
  for ( UnknownItr i = _unknowns.begin(); i != _unknowns.end(); ++i )
  {
    IUnknown *u ( (*i).get() );
    if( u->queryInterface( iid ) )
      set.insert ( *i );
  }

  return set;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all IUnknowns with given iids
//
///////////////////////////////////////////////////////////////////////////////

Manager::UnknownSet Manager::getInterfaces ( unsigned long iid1, unsigned long iid2 )
{
  USUL_TRACE_SCOPE;

  UnknownSet set;
  for ( UnknownItr i = _unknowns.begin(); i != _unknowns.end(); ++i )
  {
    IUnknown *u ( i->get() );
    if ( u->queryInterface ( iid1 ) && u->queryInterface ( iid2 )  )
      set.insert ( *i );
  }

  return set;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return list of plugin names.
//
///////////////////////////////////////////////////////////////////////////////

Manager::Strings Manager::names ( bool sort ) const
{
  USUL_TRACE_SCOPE;

  Manager::Strings names;
  UnknownSet unknowns ( _unknowns );
  for ( UnknownSet::iterator i = unknowns.begin(); i != unknowns.end(); ++i )
  {
    Usul::Interfaces::IPlugin::QueryPtr plugin ( i->get() );
    if ( plugin.valid() )
      names.push_back ( plugin->getPluginName() );
  }

  if ( true == sort )
    names.sort();

  return names;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print message about loaded plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::print ( std::ostream &out ) const
{
  USUL_TRACE_SCOPE;

  const Manager::Strings names ( this->names() );
  out << names.size() << ( ( 1 == names.size() ) ? ( " plugin" ) : ( " plugins" ) );
  if ( false == names.empty() )
  {
    out << ": ";
    std::copy ( names.begin(), names.end(), std::ostream_iterator<std::string> ( out, "; " ) );
  }
  out << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the factory for the given file name
//
///////////////////////////////////////////////////////////////////////////////

Manager::Factory* Manager::_factory ( const std::string &filename )
{
  USUL_TRACE_SCOPE;

  // Load the library. It throws if it fails to load.
  Usul::DLL::Library::ValidRefPtr lib ( Usul::DLL::Loader::load ( filename ) );

  // Cache in our pool.
  Helper::_pool.insert ( lib.get() );

  // Get the debug function. Note: g++ does not allow a reinterpret_cast.
  typedef bool (*DebugFunction)();
  DebugFunction df = (DebugFunction) Helper::_getFunction ( "usul_is_debug_build", lib );

  // Make sure it matches.
  Helper::_checkBuildModes ( Helper::_isDebug(), df(), lib.get() );

  // Get the factory function.
  typedef Usul::Interfaces::IClassFactory IClassFactory;
  typedef IClassFactory * (*FactoryFunction)();
  FactoryFunction ff = (FactoryFunction) Helper::_getFunction ( "usul_get_class_factory", lib );

  // Get the class factory.
  IClassFactory::ValidRefPtr factory ( ff() );
  return factory.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add extensions for plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::addPluginExtension ( const std::string &ext )
{
  USUL_TRACE_SCOPE;
  _plugExts.insert ( ext );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove extensions for plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::removePluginExtension ( const std::string &ext )
{
  USUL_TRACE_SCOPE;
  _plugExts.erase ( ext );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear extensions for plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::clearPluginExtensions()
{
  USUL_TRACE_SCOPE;
  _plugExts.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add directories for plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::addDirectory ( const std::string &ext )
{
  USUL_TRACE_SCOPE;
  _directories.insert ( ext );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove directories for plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::removeDirectory ( const std::string &ext )
{
  USUL_TRACE_SCOPE;
  _directories.erase ( ext );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear directories for plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::clearDirectory()
{
  USUL_TRACE_SCOPE;
  _directories.clear();
}
