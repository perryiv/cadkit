
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for managing IUnknowns
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_COMPONENT_MANAGER_H__
#define __USUL_COMPONENT_MANAGER_H__

#include "Usul/Export/Export.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IClassFactory.h"

#include <string>
#include <set>
#include <list>

namespace Usul {
namespace Components {


class USUL_EXPORT Manager
{
public:
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::set < IUnknown::ValidRefPtr >  UnknownSet;
  typedef std::list < std::string > Strings;

  static Manager* instance();

  void load  ( unsigned long iid, const std::list<std::string> &plugins, bool keepGoingIfException = true );
  void clear ();

  //do we have any unknowns?
  bool empty () const { return _unknowns.empty(); }

  // Return list of plugin names. This queries each unknown pointer for IPlugin.
  Strings names() const;

  //return the unknowns
  UnknownSet& unknowns ()  { return _unknowns; }

  //Get a single IUnknown
  Usul::Interfaces::IUnknown* getInterface( unsigned long iid );

  //Get a set of IUnknowns
  UnknownSet                  getInterfaces( unsigned long iid );
  UnknownSet                  getInterfaces( unsigned long iid1, unsigned long iid2 );

private:
  typedef UnknownSet::iterator UnknownItr;
  typedef Usul::Interfaces::IClassFactory Factory;

  Manager();

  Factory*  _factory ( const std::string &filename );

  UnknownSet _unknowns;

  static Manager *_instance;
};


}; // namespace Components
}; // namespace Usul


#endif // __USUL_COMPONENT_MANAGER_H__
