
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for a registry, which is a mapping between arbitrary strings.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_COMPONENT_REGISTRY_CLASS_H_
#define _USUL_COMPONENT_REGISTRY_CLASS_H_

#include "Usul/Export/Export.h"

#include <string>
#include <map>

namespace Usul { namespace Threads { class Mutex; }; };


namespace Usul {
namespace Components {


class USUL_EXPORT Registry
{
public:

  // Useful typedefs.
  typedef std::map < std::string, std::string > StringMap;
  typedef StringMap::size_type size_type;
  typedef StringMap::iterator iterator;
  typedef StringMap::const_iterator const_iterator;
  typedef Usul::Threads::Mutex Mutex;

  // Access to the one instance.
  static Registry &   instance();

  // Add the key and the data associated with it.
  void                add ( const std::string &key, const std::string &data );

  // Iterators.
  const_iterator      begin() const { return _strings.begin(); }
  iterator            begin()       { return _strings.begin(); }
  const_iterator      end()   const { return _strings.end(); }
  iterator            end()         { return _strings.end(); }

  // Is the registry empty?
  bool                empty() const;

  // Get the number of entries.
  size_type           size() const;

  // Look for the data given the key. Throws if not found.
  std::string         find ( const std::string &key ) const;

  // Read the registry and populate the internal map.
  virtual void        read ( const std::string &filename = "" ) = 0;

  // Get the mutex.
  const Mutex &       mutex() const;
  Mutex &             mutex();

protected:

  // Construct/destruct.
  Registry();
  virtual ~Registry();

  static Registry *_registry;

private:

  // No copying.
  Registry ( const Registry & );
  Registry &operator = ( const Registry & );

  Mutex *_mutex;
  StringMap _strings;
};


}; // namespace Components
}; // namespace Usul


#endif // _USUL_COMPONENT_REGISTRY_CLASS_H_
