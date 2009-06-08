
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that represents a dynamically loaded library.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/DLL/Library.h"
#include "Usul/DLL/Listener.h"
#include "Usul/DLL/Exceptions.h"
#include "Usul/Errors/Stack.h"
#include "Usul/System/LastError.h"
#include "Usul/Threads/Variable.h"
#include "Usul/Threads/Guard.h"

#ifdef _WIN32
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#else
# include <dlfcn.h>
#endif

#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>

using namespace Usul;
using namespace Usul::DLL;


///////////////////////////////////////////////////////////////////////////////
//
//  The details...
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul {
namespace DLL {
namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  The listeners.
//
///////////////////////////////////////////////////////////////////////////////

typedef std::vector < Listener::RefPtr > ListenerVector;
typedef Usul::Threads::Variable < ListenerVector > Listeners;
typedef Usul::Threads::Guard < Listeners::MutexType > Guard;
Listeners _listeners;


///////////////////////////////////////////////////////////////////////////////
//
//  Load the library.
//
///////////////////////////////////////////////////////////////////////////////

Library::ModuleHandle _load ( const std::string &filename )
{
  // Handle bad string.
  if ( filename.empty() )
    return 0x0;

#ifdef _WIN32
  return ::LoadLibrary ( filename.c_str() );
#else
  return ::dlopen ( filename.c_str(), RTLD_LAZY | RTLD_GLOBAL );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Free the library.
//
///////////////////////////////////////////////////////////////////////////////

bool _free ( Library::ModuleHandle module )
{
  // Null modules are already free. Do not change this without 
  // also changing the code that calls this.
  if ( 0x0 == module )
    return true;

#ifdef _WIN32
  return ( 0 != ::FreeLibrary ( reinterpret_cast<HMODULE> ( module ) ) );
#else
  return ( 0 == ::dlclose ( module ) );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the address of the function.
//
///////////////////////////////////////////////////////////////////////////////

Library::Function _function ( const std::string &name, Library::ModuleHandle module )
{
  // Handle bad module.
  if ( 0x0 == module )
    return 0x0;

  // Handle bad string.
  if ( name.empty() )
    return 0x0;

#ifdef _WIN32
  return ::GetProcAddress ( reinterpret_cast<HMODULE> ( module ), name.c_str() );
#else
  return ::dlsym ( module, name.c_str() );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  End of the details.
//
///////////////////////////////////////////////////////////////////////////////

} // namespace Detail
} // namespace DLL
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Library::Library ( const std::string &filename ) : BaseClass(),
  _module   ( 0x0 ),
  _filename ( filename )
{
  // Initialize the last error.
  System::LastError::init();

  // Load the library.
  _module = Usul::DLL::Detail::_load ( filename );

  // Notify listeners.
  this->callListeners ( ( ( _module ) ? ( "Loaded: " ) : ( "Failed to load: " ) ) +  _filename );

  // See if it worked.
  if ( 0x0 == _module )
  {
    std::ostringstream out;
    out << "Error: 3942529130, failed to load library: " << _filename 
        << "\n\tSystem error number: "  << System::LastError::number()
        << "\n\tSystem error message: " << System::LastError::message();
    throw Usul::DLL::Exceptions::FailedToLoad ( out.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Library::~Library()
{
  try
  {
    // Initialize the last error.
    System::LastError::init();

    // Free the library.
    bool success = Usul::DLL::Detail::_free ( _module );

    // Make sure it worked.
    if ( false == success )
    {
      std::ostringstream out;
      out << "Error: 4286324342, failed to free library: " << _filename 
          << "\n\tSystem error number: "  << System::LastError::number()
          << "\n\tSystem error message: " << System::LastError::message();
      Usul::Errors::Stack::instance().push ( out.str() );
    }

    // Notify listeners.
    this->callListeners ( ( ( success ) ? ( "Released: " ) : ( "Failed to release: " ) ) +  _filename );
  }

  catch ( const std::exception &e )
  {
    std::cout << "Error 3130313481: Standard exception caught while releasing: " << _filename << std::endl;
    if ( e.what() )
      std::cout << ". " << e.what() << std::endl;
  }
  catch ( ... )
  {
    std::cout << "Error 3947493394: Unknown exception caught while releasing: " << _filename << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the address of the function.
//
///////////////////////////////////////////////////////////////////////////////

Library::Function Library::function ( const std::string &name ) const
{
  Function fun ( Usul::DLL::Detail::_function ( name, _module ) );
  if ( 0x0 == fun )
    this->callListeners ( "No function '" + name + "' in: " + _filename );
  return fun;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append a listener.
//
///////////////////////////////////////////////////////////////////////////////

void Library::append ( Usul::DLL::Listener *l )
{
  if ( l )
  {
    Detail::Guard guard ( Detail::_listeners.mutex() );
    Detail::_listeners.value().push_back ( l );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a listener.
//
///////////////////////////////////////////////////////////////////////////////

bool Library::remove ( Usul::DLL::Listener *l )
{
  Detail::Guard guard ( Detail::_listeners.mutex() );
  Detail::ListenerVector::iterator begin ( Detail::_listeners.value().begin() );
  Detail::ListenerVector::iterator end   ( Detail::_listeners.value().end()   );
  Detail::ListenerVector::iterator i = std::find ( begin, end, Usul::DLL::Listener::RefPtr ( l ) );
  if ( end == i )
    return false;
  Detail::_listeners.value().erase ( i );
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call the listener.
//
///////////////////////////////////////////////////////////////////////////////

void Library::callListeners ( const std::string &message )
{
  Detail::Guard guard ( Detail::_listeners.mutex() );
  Detail::ListenerVector::iterator begin ( Detail::_listeners.value().begin() );
  Detail::ListenerVector::iterator end   ( Detail::_listeners.value().end()   );
  for ( Detail::ListenerVector::iterator i = begin; i != end; ++i )
  {
    Listener::RefPtr l ( *i );
    if ( l.valid() )
    {
      (*l) ( message );
    }
  }
}
