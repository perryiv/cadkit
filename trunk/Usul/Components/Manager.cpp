
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Components/Manager.h"
#include "Usul/Components/Object.h"
#include "Usul/Components/Exceptions.h"
#include "Usul/DLL/Library.h"
#include "Usul/DLL/Loader.h"
#include "Usul/DLL/Exceptions.h"
#include "Usul/Errors/Stack.h"
#include "Usul/File/Find.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IClassesFactory.h"

#include <iostream>

using namespace Usul;
using namespace Usul::Components;


///////////////////////////////////////////////////////////////////////////////
//
//  Start of details.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail {


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
  return ( debug ) ? "Debug" : "Release";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the function or throw if it fails.
//
///////////////////////////////////////////////////////////////////////////////

Usul::DLL::Library::Function _getFunction ( const std::string &name, Usul::DLL::Library *ptr )
{
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
  // The constructor will throw if the pointer is null.
  Usul::DLL::Library::ValidRefPtr lib ( ptr );

  // Compare the modes.
  if ( loader != justLoaded )
  {
    // Throw the exception.
    Usul::Exceptions::Thrower<Usul::Components::Exceptions::MismatchedBuildModes>
      ( "Error: 4210150186, mismatched build modes.",
        "\n\tBuild mode for loading module is ", Detail::_buildMode ( loader ),
        "\n\tBuild mode for '", lib->filename(), "' is ", Detail::_buildMode ( justLoaded ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  End of details.
//
///////////////////////////////////////////////////////////////////////////////

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
_unknowns()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load plugins in given directory with given extension.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::load ( unsigned long iid, const std::string &dir, const std::string &ext, bool keepGoingIfException )
{
  Strings plugins;
  Usul::File::find ( dir, ext, plugins );
  this->load ( iid, plugins, keepGoingIfException );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Load all plugins in the list of file names.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::load ( unsigned long iid, const Strings &plugins, bool keepGoingIfException )
{
  typedef std::list<std::string>::const_iterator Iterator;

  for ( Iterator i = plugins.begin(); i != plugins.end(); ++i )
  {
    const std::string name ( *i );
    
    try
    {
      // Find the factory
      Usul::Interfaces::IClassFactory::ValidQueryPtr factory ( this->_factory ( name ) );

      // Interface for adding mutliple IUnknowns
      Usul::Interfaces::IClassesFactory::QueryPtr classes ( factory );

      // Do we need to add more than one?
      if ( classes.valid() )
      {
        typedef Usul::Interfaces::IClassesFactory::Unknowns Unknowns;

        // Get the list of IUnknowns
        Unknowns unknowns ( classes->createInstances ( iid ) );

        // Go through each list and add to our set
        for( Unknowns::iterator i = unknowns.begin(); i != unknowns.end(); ++i )
        {
          Usul::Interfaces::IUnknown::QueryPtr unknown ( *i );

          // Insert into set of plugins
          if ( unknown.valid() )
            _unknowns.insert ( unknown.get() );
        }
      }
      else
      {
        // Get the IUnknown
        Usul::Interfaces::IUnknown::QueryPtr unknown ( factory->createInstance ( iid ) );

        // Insert into set of plugins.
        if ( unknown.valid() )
          _unknowns.insert ( unknown.get() );
      }
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
//  Clear plugins and release libraries
//
///////////////////////////////////////////////////////////////////////////////

void Manager::clear() 
{ 
  _unknowns.clear(); 
  Detail::_pool.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find first IUnknown with given iid
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Manager::getInterface( unsigned long iid )
{
  for( UnknownItr i = _unknowns.begin(); i != _unknowns.end(); ++i )
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

Manager::UnknownSet Manager::getInterfaces( unsigned long iid )
{
  UnknownSet set;

  for( UnknownItr i = _unknowns.begin(); i != _unknowns.end(); ++i )
  {
    IUnknown *u ( (*i).get() );
    if( u->queryInterface( iid ) )
      set.insert( *i );
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
  UnknownSet set;

  for ( UnknownItr i = _unknowns.begin(); i != _unknowns.end(); ++i )
  {
    IUnknown *u ( i->get() );
    if ( u->queryInterface ( iid1 ) && u->queryInterface ( iid2 )  )
      set.insert (  *i );
  }

  return set;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return list of plugin names.
//
///////////////////////////////////////////////////////////////////////////////

Manager::Strings Manager::names() const
{
  Manager::Strings names;
  UnknownSet unknowns ( _unknowns );
  for ( UnknownSet::iterator i = unknowns.begin(); i != unknowns.end(); ++i )
  {
    Usul::Interfaces::IPlugin::QueryPtr plugin ( i->get() );
    if ( plugin.valid() )
      names.push_back ( plugin->getPluginName() );
  }
  names.sort();
  return names;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the factory for the given file name
//
///////////////////////////////////////////////////////////////////////////////

Manager::Factory* Manager::_factory ( const std::string &filename )
{
  // Load the library. It throws if it fails to load.
  Usul::DLL::Library::ValidRefPtr lib ( Usul::DLL::Loader::load ( filename ) );

  Detail::_pool.insert( lib.get() );

  // Get the debug function. Note: g++ does not allow a reinterpret_cast.
  typedef bool (*DebugFunction)();
  DebugFunction df = (DebugFunction) Detail::_getFunction ( "usul_is_debug_build", lib );

  // Make sure it matches.
  Detail::_checkBuildModes ( Detail::_isDebug(), df(), lib.get() );

  // Get the factory function.
  typedef Usul::Interfaces::IClassFactory IClassFactory;
  typedef IClassFactory * (*FactoryFunction)();
  FactoryFunction ff = (FactoryFunction) Detail::_getFunction ( "usul_get_class_factory", lib );

  // Get the class factory.
  IClassFactory::ValidRefPtr factory ( ff() );

  return factory.release();
  //return Usul::Components::Object::factory( filename );
}
