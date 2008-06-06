
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Exceptions for when things take too long.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_EXCEPTIONS_TIMED_OUT_CLASS_H_
#define _USUL_EXCEPTIONS_TIMED_OUT_CLASS_H_

#include <stdexcept>
#include <string>


namespace Usul {
namespace Exceptions {
namespace TimedOut {


///////////////////////////////////////////////////////////////////////////////
//
//  Exception class timed-out network downloads.
//
///////////////////////////////////////////////////////////////////////////////

struct NetworkDownload : public std::runtime_error
{
  typedef std::runtime_error BaseClass;
  NetworkDownload ( const std::string &message ) : BaseClass ( message )
  {
  }
};


} // namespace TimedOut
} // namespace Exceptions
} // namespace Usul


#endif // _USUL_EXCEPTIONS_TIMED_OUT_CLASS_H_