
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

#ifndef _VIRTUAL_REALITY_VIEWER_REGISTRY_CLASS_H_
#define _VIRTUAL_REALITY_VIEWER_REGISTRY_CLASS_H_

#include "VRV/Core/Export.h"

#include "Usul/Components/Registry.h"

#include <string>


namespace VRV {


class VRV_EXPORT Registry : public Usul::Components::Registry
{
public:

  // Useful typedef(s).
  typedef Usul::Components::Registry BaseClass;

  // Access to the one instance.
  static BaseClass &  instance();

  // Read the registry and populate the internal map.
  virtual void        read ( const std::string &filename = "" );

protected:

  // Construct/destruct.
  Registry();
  ~Registry();

  void                _addModule ( const std::string & );

  void                _directory  ( const std::string & );

  void                _moduleFile ( const std::string & );
  void                _moduleName ( const std::string & );

  void                _read ( const std::string &filename );

private:

  typedef Usul::Threads::Mutex Mutex;

  // No copying.
  Registry ( const Registry & );
  Registry &operator = ( const Registry & );

  Mutex *_readMutex;
  std::string _dir;
  std::string _name;
  std::string _file;
};


}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_REGISTRY_CLASS_H_
