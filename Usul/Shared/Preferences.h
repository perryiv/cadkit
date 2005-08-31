
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

namespace Usul { namespace Threads { class Mutex; } }


namespace Usul {
namespace Shared {


class USUL_EXPORT Preferences
{
public:

  // Typedefs.
  typedef std::map < std::string, bool >          Bools;
  typedef std::map < std::string, int >           Ints;
  typedef std::map < std::string, unsigned int >  Uints;
  typedef std::map < std::string, float >         Floats;
  typedef std::map < std::string, double >        Doubles;
  typedef std::map < std::string, std::string >   Strings;

  // Get the value.
  bool                  getBool   ( const std::string &key );
  std::string           getString ( const std::string &key );
  void                  getString ( const std::string &key, std::string &value );
  int                   getInt    ( const std::string &key );
  unsigned int          getUint   ( const std::string &key );
  float                 getFloat  ( const std::string &key );
  double                getDouble ( const std::string &key );

  // Return the single instance.
  static Preferences &  instance();

  // Set the value.
  void                  setBool   ( const std::string &key, bool               value );
  void                  setString ( const std::string &key, const std::string &value );
  void                  setInt    ( const std::string &key, int                value );
  void                  setUint   ( const std::string &key, unsigned int       value );
  void                  setFloat  ( const std::string &key, float              value );
  void                  setDouble ( const std::string &key, double             value );

  // Get copies of the containers.
  const Bools           getBools()   const;
  const Strings         getStrings() const;
  const Ints            getInts()    const;
  const Uints           getUints()   const;
  const Floats          getFloats()  const;
  const Doubles         getDoubles() const;

private:

  // Constructor and destructor.
  Preferences();
  ~Preferences();

  // No copying or assignment.
  Preferences &operator = ( const Preferences & );
  Preferences ( const Preferences & );

  Bools    _bools;
  Ints     _ints;
  Uints    _uints;
  Floats   _floats;
  Doubles  _doubles;
  Strings  _strings;
  Usul::Threads::Mutex *_mutex;
};


}; // namespace Shared
}; // namespace Usul


#endif // _USUL_PREFERENCES_H_
