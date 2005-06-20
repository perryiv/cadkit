
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

  // Get the value.
  bool                  getBool   ( const std::string &key );
  const std::string &   getString ( const std::string &key );

  // Return the single instance.
  static Preferences &  instance();

  // Set the value.
  void                  setBool   ( const std::string &key, bool value );
  void                  setString ( const std::string &key, const std::string &value );

private:

  // Constructor and destructor.
  Preferences();
  ~Preferences();

  // No copying or assignment.
  Preferences &operator = ( const Preferences & );
  Preferences ( const Preferences & );

  std::map < std::string, bool >        _bools;
  std::map < std::string, std::string > _strings;
};


}; // namespace Shared
}; // namespace Usul


#endif // _USUL_PREFERENCES_H_
