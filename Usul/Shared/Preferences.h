
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Preference class to hold application-defined state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREFERENCES_H_
#define _USUL_PREFERENCES_H_

#include "Usul/Export/Export.h"

#include <string>
#include <map>


namespace Usul {
namespace Shared {


class USUL_EXPORT Preferences
{
public:

  // Return the single instance.
  static Preferences &instance();

  // Get the boolean.
  bool &        getBool ( const std::string &key );

  // Set the boolean.
  void          setBool ( const std::string &key, bool t );

private:

  // Constructor and destructor.
  Preferences();
  ~Preferences();

  // No copying or assignment.
  Preferences &operator = ( const Preferences & );
  Preferences ( const Preferences & );

  std::map < std::string, bool > _bools;
};


}; // namespace Shared
}; // namespace Usul


#endif // _USUL_PREFERENCES_H_
