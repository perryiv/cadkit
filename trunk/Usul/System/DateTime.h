
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class the returns time nicely formatted dates and times.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SYSTEM_CLOCK_CLASS_H_
#define _USUL_SYSTEM_CLOCK_CLASS_H_

#include "Usul/Export/Export.h"
#include "Usul/Types/Types.h"

#include <string>
#include <time.h>


namespace Usul {
namespace System {


class USUL_EXPORT DateTime
{
public:

  DateTime();
  ~DateTime();

  static std::string        format ( time_t, const std::string &f = std::string() ); // time_t
  static std::string        format ( const std::string &f = std::string() );

  static ::tm               local ( time_t );
  static ::tm               local();

  static std::string        now(); // Depreciated. Use format() instead.

  // Pass __DATE__ macro to get YYYY.MM.DD formatted string.
  static std::string        version ( const std::string &date, const std::string &delim = "." );
};


} // namespace System
} // namespace Usul


#endif // _USUL_SYSTEM_CLOCK_CLASS_H_
