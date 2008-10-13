
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Special exception that contains additional diagnostic information.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_EXCEPTIONS_EXCEPTION_CLASS_H_
#define _USUL_EXCEPTIONS_EXCEPTION_CLASS_H_

#include "Usul/Diagnostics/StackTrace.h"

#include <exception>
#include <list>
#include <string>


namespace Usul {
namespace Exceptions {


class Exception : public std::exception
{
public:

  typedef std::exception BaseClass;

  Exception ( const std::string &message ) : BaseClass(), 
    _message ( message ),
    _stackTrace(),
    _nested()
  {
  }

  Exception ( const Exception &e ) : BaseClass ( e ),
    _message ( e._message ),
    _stackTrace ( e._stackTrace ),
    _nested ( e._nested )
  {
  }

  Exception &operator = ( const Exception &e )
  {
    _message = e._message;
    _stackTrace = e._stackTrace;
    _nested = e._nested;
    return *this;
  }

  ~Exception() throw()
  {
    _message.clear();
    _nested.clear();
  }

  virtual const char* what() const throw()
  {
    return _message.c_str();
  }

  std::string message() const
  {
    return _message;
  }

  std::string stackTrace() const
  {
    return _stackTrace.toString();
  }

  bool hasNested() const
  {
    return ( false == _nested.empty() );
  }

  Exception nested() const
  {
    return ( ( true == this->hasNested() ) ? _nested.front() : Exception ( "No nested exception" ) );
  }

  void nested ( const Exception &e )
  {
    _nested.clear();
    _nested.push_back ( e );
  }

private:

  std::string _message;
  Usul::Diagnostics::StackTrace _stackTrace;
  std::list<Exception> _nested;
};


} // namespace Exceptions
} // namespace Usul


#endif // _USUL_EXCEPTIONS_EXCEPTION_CLASS_H_
