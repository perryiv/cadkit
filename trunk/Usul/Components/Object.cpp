
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for loading a component object.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Components/Object.h"
#include "Usul/Components/Registry.h"
#include "Usul/Components/Exceptions.h"

#include "Usul/DLL/Library.h"
#include "Usul/DLL/Loader.h"
#include "Usul/DLL/Exceptions.h"

#include "Usul/Interfaces/IClassFactory.h"

#include "Usul/Exceptions/Thrower.h"

#include "Usul/System/LastError.h"

#include "Usul/Errors/Assert.h"

#include "Usul/Predicates/FileExists.h"

#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include <map>

using namespace Usul;
using namespace Usul::Components;

typedef Usul::Threads::Mutex Mutex;
typedef Usul::Threads::Guard<Mutex> Guard;
typedef std::list<Registry::StringMap::value_type> RegistryValues;


///////////////////////////////////////////////////////////////////////////////
//
//  Define the slash.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# define DELIMITER_SLASH '\\'
#else
# define DELIMITER_SLASH '/'
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  The details...
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul {
namespace Components {
namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  A pool of libraries. Since it is a global, the pool goes out of scope 
//  when the program ends. That is when the libraries are released.
//  Note: the pool may have null libraries, which indicate failed loading.
//  Note: I do not want this in the header.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::DLL::Library::RefPtr LibPtr;
typedef std::map < std::string, LibPtr > LibraryPool;
LibraryPool _pool;


///////////////////////////////////////////////////////////////////////////////
//
//  We need one mutex for the pool. However, we cannot have the mutex be 
//  declared before main(), because that is where the client sets the 
//  mutex-creation function.
//
///////////////////////////////////////////////////////////////////////////////

Mutex &poolMutex()
{
  static Mutex *mutex = 0x0;
  if ( !mutex )
    mutex = Usul::Threads::Mutex::create();
  USUL_ASSERT ( mutex );
  return *mutex;
}


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
        "Name: ", name, "Library: ", lib->filename() );
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
//  Thread-safe copy of container.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class SourceContainer,
  class TargetContainer,
  class MutexType
>
inline void copy ( const SourceContainer &source, TargetContainer &target, MutexType &mutex )
{
  Guard guard ( mutex );
  std::copy ( source.begin(), source.end(), std::back_inserter ( target ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  End of the details.
//
///////////////////////////////////////////////////////////////////////////////

}; // namespace Detail
}; // namespace Components
}; // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Load the library and create the component.
//
///////////////////////////////////////////////////////////////////////////////

Interfaces::IUnknown *Object::create ( unsigned long iid, const StringList &path, const std::string &filename )
{
  // Loop through the path.
  for ( StringList::const_iterator i = path.begin(); i != path.end(); ++i )
  {
    // Get the directory.
    const std::string &dir = *i;

    // Safely...
    try
    {
      // Call the other one.
      Interfaces::IUnknown::ValidRefPtr unknown ( Object::create ( iid, dir, filename ) );
      return unknown.release();
    }

    // Catch (and eat) expected exceptions.
    catch ( const Usul::DLL::Exceptions::FailedToLoad & ){}
    catch ( const Usul::Components::Exceptions::FailedToFindFunction & ){}
  }

  // If we get to here then we failed. Turn the path into a readable string.
  std::string libPath;
  for ( StringList::const_iterator i = path.begin(); i != path.end(); ++i )
  {
    libPath += "\n\t";
    libPath += *i;
  }

  // Throw the exception.
  Usul::Exceptions::Thrower<Usul::Components::Exceptions::FailedToCreate> 
    ( "Error: 1325961825, failed to create component from file: ", filename,
      "\n\tPath used:            ", libPath,
      "\n\tSystem error number:  ", System::LastError::number(),
      "\n\tSystem error message: ", System::LastError::message() );

  // The compiler does not know that the above "Thrower" constructor throws 
  // an exception, and thus, we never get here.
  USUL_ASSERT ( 0 );
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the library and create the component.
//
///////////////////////////////////////////////////////////////////////////////

Interfaces::IUnknown *Object::create ( unsigned long iid, const std::string &dir, const std::string &filename )
{
  // Make a copy because we may change it.
  std::string directory ( dir );

  // If we have a valid string for the directory...
  if ( false == directory.empty() )
  {
    // Append a slash if needed.
    std::string::size_type last ( directory.size() - 1 );
    if ( DELIMITER_SLASH != directory[last] )
      directory += DELIMITER_SLASH;
  }

  // Call the other one.
  return Object::create ( iid, directory + filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the library and create the component.
//
///////////////////////////////////////////////////////////////////////////////

Object::CreateResult Object::_create ( unsigned long iid, const std::string &name )
{
  // Initialize the filename.
  std::string filename ( name );

  // If the string is not a file then use it as a key in the registry.
  Usul::Predicates::FileExists exists;
  if ( false == exists ( filename ) )
  {
    // It throws if it fails to find an entry.
    filename = Registry::instance().find ( name );
  }

  // Load the library. It throws if it fails to load.
  Usul::DLL::Library::ValidRefPtr lib ( Object::_load ( filename ) );

  // Call the other one.
  return Object::_create ( iid, lib.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the library and create the component.
//
///////////////////////////////////////////////////////////////////////////////

Interfaces::IUnknown *Object::create ( unsigned long iid, const std::string &name )
{
  // Call the other one.
  Object::CreateResult result ( Object::_create ( iid, name ) );

  // Return the component.
  return result.second.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the class factory.
//
///////////////////////////////////////////////////////////////////////////////

Interfaces::IClassFactory *Object::factory ( const std::string &name )
{
  // Call the other one.
  Object::CreateResult result ( Object::_create ( Factory::IID, name ) );

  // Return the factory.
  return result.first.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the component from the given library.
//
///////////////////////////////////////////////////////////////////////////////

Object::CreateResult Object::_create ( unsigned long iid, Library *ptr )
{
  // The constructor will throw if the pointer is null.
  Usul::DLL::Library::ValidRefPtr lib ( ptr );

  // Get the debug function. Note: g++ does not allow a reinterpret_cast.
  typedef bool (*DebugFunction)();
  DebugFunction df = (DebugFunction) Detail::_getFunction ( "usul_is_debug_build", lib );

  // Make sure it matches.
  Detail::_checkBuildModes ( Detail::_isDebug(), df(), ptr );

  // Get the factory function.
  typedef Usul::Interfaces::IClassFactory IClassFactory;
  typedef IClassFactory * (*FactoryFunction)();
  FactoryFunction ff = (FactoryFunction) Detail::_getFunction ( "usul_get_class_factory", lib );

  // Get the class factory.
  IClassFactory::ValidRefPtr factory ( ff() );

  // Return the factory and the component.
  return Object::CreateResult ( factory.get(), factory->createInstance ( iid ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Look for the library in the pool.
//
///////////////////////////////////////////////////////////////////////////////

Usul::DLL::Library *Object::_find ( const std::string &filename )
{
  Guard guard ( Detail::poolMutex() );
  Detail::LibraryPool::const_iterator i = Detail::_pool.find ( filename );
  return ( Detail::_pool.end() == i ) ? 0x0 : i->second.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the library in the pool.
//
///////////////////////////////////////////////////////////////////////////////

void Object::_setPool ( const std::string &key, Library *lib )
{
  Guard guard ( Detail::poolMutex() );
  Detail::_pool[key] = lib;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a valid library, either from the pool or a newly loaded one.
//
///////////////////////////////////////////////////////////////////////////////

Usul::DLL::Library *Object::_load ( const std::string &filename )
{
  // See if it is in the pool.
  Library *fromPool = Object::_find ( filename );
  if ( fromPool )
    return fromPool;

  // Safely...
  try
  {
    // Load the library. It throws if it fails to load.
    Usul::DLL::Library::ValidRefPtr lib ( Usul::DLL::Loader::load ( filename ) );

    // Add the library to our map. You have to keep the libraries loaded 
    // because you are handing a pointer from the library over to the client.
    // Note: I am still searching for an elegant way to have the libraries 
    // automatically free themselves, without putting too much of a burden on 
    // the component authors.
    Object::_setPool ( lib->filename(), lib );

    // Return the library.
    return lib.release();
  }

  // Catch all exceptions.
  catch ( ... )
  {
    // Add this filename with a null library to the pool. That way we don't 
    // waste time trying to load it again. It will already be in the pool 
    // and returned (as null) above.
    Object::_setPool ( filename, 0x0 );

    // Rethrow the exception.
    throw;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load all components that support the interface.
//
///////////////////////////////////////////////////////////////////////////////

void Object::create ( unsigned long iid, Object::UnknownList &u )
{
  // Copy the registry.
  RegistryValues reg;
  Detail::copy ( Registry::instance(), reg, Registry::instance().mutex() );

  // Used below.
  Usul::Predicates::FileExists exists;

  // Loop through the registry's data strings...
  for ( RegistryValues::const_iterator i = reg.begin(); i != reg.end(); ++i )
  {
    // Get the string.
    const std::string &s = i->second;

    // If it is a file then try to load it.
    if ( exists ( s ) )
    {
      // Safely...
      try { u.push_back ( Object::create ( iid, s ) ); }

      // Catch and eat exceptions that we know of.
      catch ( const Usul::Components::Exceptions::Exception & ){}
      catch ( const Usul::DLL::Exceptions::Exception & ){}
      catch ( const Usul::Pointers::Exceptions::NullPointer & ){}

      #ifdef _DEBUG

      // Catch all other exceptions for now (until you are confident 
      // that above ones get them all.)
      catch ( const std::exception & ) { USUL_ASSERT ( 0 ); throw; }
      catch ( ... )                    { USUL_ASSERT ( 0 ); throw; }

      #endif
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the object with control over exception handeling.
//
///////////////////////////////////////////////////////////////////////////////

Interfaces::IUnknown *Object::create ( unsigned long iid, const std::string &name, bool throwExpected, bool throwUnexpected )
{
  // Create the object safely.
  try { return Usul::Components::Object::create ( iid, name ); }

  // Catch exceptions that we know of.
  catch ( const Usul::Components::Exceptions::Exception & )
  {
    if ( throwExpected )
      throw;
  }
  catch ( const Usul::DLL::Exceptions::Exception & )
  {
    if ( throwExpected )
      throw;
  }
  catch ( const Usul::Pointers::Exceptions::NullPointer & )
  {
    if ( throwExpected )
      throw;
  }

  // Catch all other exceptions.
  catch ( const std::exception &e )
  {
    USUL_ASSERT ( 0 );
    if ( throwUnexpected )
      throw e;
  }
  catch ( ... )
  {
    USUL_ASSERT ( 0 );
    if ( throwUnexpected )
      throw;
  }

  // If we get to here then just return null.
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Release all libraries. Note: this is the hammer. It does not check 
//  to see if there are any existing components from the libraries.
//
///////////////////////////////////////////////////////////////////////////////

void Object::releaseLibraries()
{
  Guard guard ( Detail::poolMutex() );
  Detail::_pool.clear();
}
