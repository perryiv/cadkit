
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Options given on the command line.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __USUL_COMMAND_LINE_OPTIONS_H__
#define __USUL_COMMAND_LINE_OPTIONS_H__

#include "Usul/Export/Export.h"
#include "Usul/Strings/Convert.h"

#include <map>
#include <string>
#include <iosfwd>

namespace Usul {
namespace CommandLine {

class USUL_EXPORT Options
{
 public:
  typedef std::map < std::string, std::string > Map;
  typedef Map::iterator iterator;
  typedef Map::const_iterator const_iterator;

  Options();

  void insert ( const std::string&, const std::string& );
  void insert ( const Options & );

  // Does this option exist?
  bool option ( const std::string& ) const;

  // Get the value for the option.
  std::string value ( const std::string& option, const std::string &defaultValue = "" ) const;

  /**
   * @brief Returns the given option as a string value
   * @param option A named Option
   * @return The value of the option as a string
   */
  std::string stringValue ( const std::string& option, const std::string &defaultValue = "" ) const;

  /**
   * @brief Returns the given option as a integer value
   * @param option A named Option
   * @return The value of the option as a integer
   */
  unsigned long longValue ( const std::string& option, unsigned long defaultValue = 0 ) const;
 
  /**
   * @brief Returns the given option as a float value
   * @param option A named Option
   * @return The value of the option as a float
   */
  float floatValue ( const std::string& option, float defaultValue = 0.0f ) const;

  // Iterators.
  iterator        begin()       { return _options.begin(); }
  const_iterator  begin() const { return _options.begin(); }
  iterator        end()         { return _options.end();   }
  const_iterator  end()   const { return _options.end();   }

  // Get the option.
  template < class T > void get ( const std::string &option, T &t, const T &defaultValue )
  {
    Map::const_iterator iter = _options.find ( option );
    t = ( ( iter != _options.end() ) ? Usul::Strings::fromString<T> ( iter->second ) : defaultValue );
  }

  // Get the option.
  template < class T > T get ( const std::string &option, const T &defaultValue )
  {
    T t;
    this->get<T> ( option, t, defaultValue );
    return t;
  }

  // Remove the option.
  void remove ( const std::string &name )
  {
    _options.erase ( name );
  }

private:

  Map _options;
};

USUL_EXPORT std::istream& operator>>( std::istream& in, Options& o );

}
}



#endif //__USUL_COMMAND_LINE_OPTIONS_H__

