
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The registry class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Registry.h"
#include "Exceptions.h"
#include "ErrorChecker.h"

#include "XmlTools/Reader.h"
#include "XmlTools/Policy.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include <fstream>

using namespace VRV;

typedef Usul::Threads::Guard<Usul::Threads::Mutex> Guard;


///////////////////////////////////////////////////////////////////////////////
//
//  Define the xml-reader type.
//
///////////////////////////////////////////////////////////////////////////////

namespace VRV
{
  typedef void (Registry::*Function) ( const std::string & );
  typedef Usul::Adaptors::MemberFunction < Registry *, Function > MemFun;
  typedef XML::Reader < MemFun, XML::ValueCB, VRV::ErrorChecker > XmlReader;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Get a pointer to the static instance.
//
///////////////////////////////////////////////////////////////////////////////

Registry::BaseClass &Registry::instance()
{
  if ( 0x0 == _registry )
  {
    _registry = new Registry;
    if ( 0x0 == _registry )
    {
      Usul::Exceptions::Thrower<std::runtime_error> (
        "Error: 1230927121, failed to declare static instance of registry." );
    }
  }
  return *_registry;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Registry::Registry() : BaseClass(),
  _readMutex ( Usul::Threads::Mutex::create() ),
  _dir(),
  _name(),
  _file()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Registry::~Registry()
{
  USUL_ASSERT ( _readMutex );
  delete _readMutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the registry file.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::read ( const std::string &filename )
{
  // One thread should read at a time.
  Guard guard ( *_readMutex );

  // Set to defaults.
  _dir = _name = _file = "";

#if 1
  // Look in the home directory.
  const char *home = ::getenv ( "HOME" );
  std::string registry ( "plugin_registry.xml" );
  if ( home )
    this->_read ( std::string ( home ) + "/.vrv/" + registry );
#endif
#if 1
  // Look for the environment variable.
  const char *env = ::getenv ( "VRV_REGISTRY_FILE" );
  if ( env )
    this->_read ( env );
#endif
#if 1
  // Look in the current working directory.
  this->_read ( registry );
#endif
#if 1
  // Read the given file.
  if ( !filename.empty() )
    this->_read ( filename );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and populate the registry.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::_read ( const std::string &filename )
{
  Usul::Predicates::FileExists exists;
  if ( !exists ( filename ) )
    return;

  // Make some member function adaptors.
  MemFun dir  ( this, &Registry::_directory  );
  MemFun name ( this, &Registry::_moduleName );
  MemFun file ( this, &Registry::_moduleFile );
  MemFun add  ( this, &Registry::_addModule );

  // Declare the reader and add the callbacks.
  XmlReader reader;
  reader.add ( XmlReader::NODE_START,  "vrv/plugins/directory",   dir  );
  reader.add ( XmlReader::NODE_START,  "vrv/plugins/module/name", name );
  reader.add ( XmlReader::NODE_START,  "vrv/plugins/module/file", file );
  reader.add ( XmlReader::NODE_FINISH, "vrv/plugins/module",      add  );

  // Read the file.
  try { reader.read ( filename ); }

  // Catch exceptions from the reader.
  catch ( const XML::Error &e )
  {
    // Throw a better exception.
    Usul::Exceptions::Thrower<VRV::Exceptions::CorruptRegistry> (
      "Error: 2346022239, corrupt registry file: ", filename,
      "\n\tLine:     ", e.line(),
      "\n\tPosition: ", e.position(),
      "\n\tMessage:  ", e.message() );
  }
}


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
//  Set the directory.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::_directory ( const std::string &s )
{
  // Set the directory.
  _dir = s;

  // If we have a valid string for the directory...
  if ( false == _dir.empty() )
  {
    // Append a slash if needed.
    std::string::size_type last ( _dir.size() - 1 );
    if ( DELIMITER_SLASH != _dir[last] )
      _dir += DELIMITER_SLASH;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the file.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::_moduleFile ( const std::string &s )
{
  // Set the file name.
  _file = s;

  // On windows, the extension for the debug-built plugin 
  // will have a 'd' appended to it.
#ifdef _WIN32
#ifdef _DEBUG
  _file = s + 'd';
#endif
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::_moduleName ( const std::string &s )
{
  _name = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the module.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::_addModule ( const std::string & )
{
  // The adding is protected with its own mutex.
  this->add ( _name, _dir + _file );
}
