
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

  static Manager* instance();

  void load  ( unsigned long iid, std::list<std::string> &plugins );
  void clear ();

  //do we have any unknowns?
  bool empty () const { return _unknowns.empty(); }

  //return the unknowns
  UnknownSet& unknowns ()  { return _unknowns; }

  //Get a single IUnknown
  Usul::Interfaces::IUnknown* getInterface( unsigned long iid );

  //Get a set of IUnknowns
  UnknownSet                  getInterfaces( unsigned long iid );

private:
  typedef UnknownSet::iterator UnknownItr;

  Manager();

  UnknownSet _unknowns;

  static Manager *_instance;
};


}; // namespace Components
}; // namespace Usul


#endif // __USUL_COMPONENT_MANAGER_H__
