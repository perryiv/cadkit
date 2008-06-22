
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Special exception signaling that the user canceled an operation.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_EXCEPTIONS_CANCELED_CLASS_H_
#define _USUL_EXCEPTIONS_CANCELED_CLASS_H_

#include <exception>
#include <string>

#define CANCELED_MESSAGE_STRING "Operation canceled by user"


namespace Usul {
namespace Exceptions {


class Canceled : public std::exception
{
public:

  // Useful typedef.
  typedef std::exception BaseClass;

  // Constructors.
  Canceled() : BaseClass(), _message ( CANCELED_MESSAGE_STRING )
  {
  }
  Canceled ( const std::string &message ) : BaseClass(), _message ( message.empty() ? CANCELED_MESSAGE_STRING : message.c_str() )
  {
  }
  Canceled ( const Canceled &e ) : BaseClass ( e )
  {
  }

  // Destructor.
  ~Canceled() throw()
  {
  }

  // Assignment.
  Canceled &operator = ( const Canceled &e )
  {
    BaseClass::operator = ( e );
    return *this;
  }

  virtual const char* what() const throw() { return _message.c_str(); }
  
private:

  std::string _message;
};


typedef Canceled Cancelled;


} // namespace Exceptions
} // namespace Usul


#undef CANCELED_MESSAGE_STRING


#endif // _USUL_EXCEPTIONS_CANCELED_CLASS_H_
