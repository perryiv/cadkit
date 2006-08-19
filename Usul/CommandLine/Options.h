
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


#include <map>
#include <string>

namespace Usul {
namespace CommandLine {

class Options
{
 public:
  typedef std::map < std::string, std::string > Map;
  typedef Map::iterator iterator;
  typedef Map::const_iterator const_iterator;

  Options();

  void insert ( const std::string&, const std::string& );

  // Does this option exist?
  bool option ( const std::string& ) const;

  // Get the value for the option.
  std::string value ( const std::string& ) const;

/**
 * @brief Returns the given option as a string value
 * @param option A named Option
 * @return The value of the option as a string
 */
 std::string Options::stringValue ( const std::string& option ) const;

/**
 * @brief Returns the given option as a integer value
 * @param option A named Option
 * @return The value of the option as a integer
 */
unsigned long int Options::intValue (const std::string& option) const;
 
 /**
 * @brief Returns the given option as a float value
 * @param option A named Option
 * @return The value of the option as a float
 */
float Options::floatValue(const std::string& option) const;


  iterator begin() { return _options.begin(); }
  iterator end() { return _options.end(); }

  const_iterator begin() const { return _options.begin(); }
  const_iterator end()   const { return _options.end();   }

 private:
  Map _options;
};

}
}



#endif //__USUL_COMMAND_LINE_OPTIONS_H__

