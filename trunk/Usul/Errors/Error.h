
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A single error.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ERRORS_ERROR_CLASS_H_
#define _USUL_ERRORS_ERROR_CLASS_H_

#include "Usul/Export/Export.h"
#include "Usul/Types/Types.h"

#include <string>


namespace Usul {
namespace Errors {


class USUL_EXPORT Error
{
public:

  // Constructor/destructor.
  Error ( const std::string &message );
  Error ( const Error & );
  ~Error();

  // Format the error into a single string.
  void                format ( std::string &s ) const;
  std::string         format() const;

  // Get the message.
  const std::string   message() const { return _message; }

private:

  // No assignment.
  Error &operator = ( const Error & );

  std::string _message;
  unsigned long _thread;
  Usul::Types::Uint64 _milliseconds;
};


} // namespace Errors
} // namespace Usul


#endif // _USUL_ERRORS_ERROR_CLASS_H_
