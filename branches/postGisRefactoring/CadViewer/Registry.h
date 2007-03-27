
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The registry class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_REGISTRY_CLASS_H_
#define _CV_REGISTRY_CLASS_H_

#include "Usul/Components/Registry.h"

#include <string>


namespace CV {


class Registry : public Usul::Components::Registry
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

  // No copying.
  Registry ( const Registry & );
  Registry &      operator = ( const Registry & );

  std::string _dir;
  std::string _name;
  std::string _file;
}; // class Registry


}; // namespace CV


#endif // _CV_REGISTRY_CLASS_H_
