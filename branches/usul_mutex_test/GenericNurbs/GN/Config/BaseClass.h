
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class configurations.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_BASE_CLASS_CONFIGURATIONS_H_
#define _GENERIC_NURBS_LIBRARY_BASE_CLASS_CONFIGURATIONS_H_

#include <string>


namespace GN {
namespace Config {
namespace Base {


///////////////////////////////////////////////////////////////////////////////
//
//  Base class that holds a string.
//
///////////////////////////////////////////////////////////////////////////////

struct StringData
{
  StringData() : _string()
  {
  }
  StringData ( const std::string &s ) : _string ( s )
  {
  }
  StringData ( const StringData &b ) : _string ( b._string )
  {
  }
  const std::string &getStringData() const
  {
    return _string;
  }
  void setStringData ( const std::string &s )
  {
    _string = s;
  }
  void set ( const StringData &s )
  {
    this->setStringData ( s._string );
  }
  StringData &operator = ( const StringData &b )
  {
    this->set ( b );
    return *this;
  }
private:
  std::string _string;
};


}; // namespace Base
}; // namespace Config
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_BASE_CLASS_CONFIGURATIONS_H_
