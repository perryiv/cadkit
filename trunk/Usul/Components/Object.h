
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

#ifndef _USUL_COMPONENT_OBJECT_CLASS_H_
#define _USUL_COMPONENT_OBJECT_CLASS_H_

#include "Usul/Export/Export.h"

#include "Usul/Interfaces/IClassFactory.h"

#include <string>
#include <list>

namespace Usul { namespace DLL { class Library; }; };


namespace Usul {
namespace Components {


class USUL_EXPORT Object
{
public:

  // Typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Interfaces::IClassFactory Factory;
  typedef Usul::DLL::Library Library;
  typedef std::list<std::string> StringList;
  typedef Usul::Interfaces::IUnknown::ValidRefPtr UnknownPtr;
  typedef std::list<UnknownPtr> UnknownList;

  // Load the library and create the component.
  static Unknown *      create ( unsigned long iid, 
                                 const std::string &name );
  static Unknown *      create ( unsigned long iid, 
                                 const std::string &dir, 
                                 const std::string &filename );
  static Unknown *      create ( unsigned long iid, 
                                 const StringList &path,
                                 const std::string &filename );

  // Create all components that support the given interface.
  static void           create ( unsigned long iid, UnknownList &u );

  // Create with control over exceptions. There are several expected failures, 
  // and some not anticipated. Pass true or false for either case.
  static Unknown *      create ( unsigned long iid, 
                                 const std::string &name, 
                                 bool throwExpected, 
                                 bool throwUnexpected );

  // Return the class factory.
  static Factory *      factory ( const std::string &filename );

  // Release all libraries. Note: this is the hammer. It does not check 
  // to see if there are any existing components from the libraries.
  static void           releaseLibraries();

protected:

  typedef std::pair < Factory::RefPtr, Unknown::RefPtr > CreateResult;
  static CreateResult   _create  ( unsigned long iid, const std::string &name );
  static CreateResult   _create  ( unsigned long iid, Library *lib );
  static Library *      _find    ( const std::string &filename );
  static Library *      _load    ( const std::string &filename );
  static void           _setPool ( const std::string &key, Library *lib );
};


}; // namespace Components
}; // namespace Usul


#endif // _USUL_COMPONENT_OBJECT_CLASS_H_
