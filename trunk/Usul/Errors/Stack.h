
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Error stack.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ERROR_STACK_H_
#define _USUL_ERROR_STACK_H_

#include "Usul/Export/Export.h"
#include "Usul/Exceptions/Canceled.h"

#include <list>
#include <string>
#include <exception>

namespace Usul { namespace Threads { class Mutex; }; };


namespace Usul {
namespace Errors {


#if 0


///////////////////////////////////////////////////////////////////////////////
//
//  A single error.
//
///////////////////////////////////////////////////////////////////////////////

struct USUL_EXPORT Element : protected std::pair < unsigned int, std::string >
{
  // Useful typedefs.
  typedef std::pair < unsigned int, std::string > BaseClass;

  // Constructors/destructor.
  Element();
  Element ( const Element & );
  Element ( unsigned int, const std::string & );
  ~Element();

  // Return the id.
  unsigned int          id() const;

  // Return the message.
  const std::string &   message() const;
};


class USUL_EXPORT StackAdapter : public std::stack < Usul::Errors::Element >
{
 public:
  typedef std::stack < Usul::Errors::Element > BaseClass;
  
  StackAdapter() : BaseClass() { }
  container_type::const_iterator begin() const { return c.begin(); }
  container_type::const_iterator end() const { return c.end(); }
};


#endif


///////////////////////////////////////////////////////////////////////////////
//
//  The error stack.
//
///////////////////////////////////////////////////////////////////////////////

class USUL_EXPORT Stack
{
public:

  // Useful typedefs.
  typedef std::list < std::string > StringList;
  typedef StringList::size_type size_type;
  typedef StringList::value_type value_type;
  typedef Usul::Threads::Mutex Mutex;

  // Clear the stack.
  void                clear();

  // Is it empty?
  bool                empty() const;

  // Format the errors into single string.
  void                format ( std::string &s ) const;
  std::string         format() const;

  // It's a singleton.
  static Stack &      instance();

  // Push an error.
  void                push ( unsigned int id, const std::string &message );
  void                push ( const std::string &message );

  // Pop the top error.
  void                pop();

  // Get the size.
  size_type           size() const;

  // Get the top error.
  value_type          top() const;

private:

  // Constructor/destructor.
  Stack();
  ~Stack();

  // Cannot copy.
  Stack ( const Stack & );
  Stack &operator = ( const Stack & );

  StringList _s;
  Mutex *_m;
  static Stack *_instance;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for starting an exception-safe block of code.
//
///////////////////////////////////////////////////////////////////////////////

#if 0
#define USUL_EXCEPTION_SAFE_START try {
#else
#define USUL_EXCEPTION_SAFE_START
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for ending an exception-safe block of code.
//
///////////////////////////////////////////////////////////////////////////////

#if 0
#define USUL_EXCEPTION_SAFE_END(error_id)\
}\
catch ( const Usul::Exceptions::Canceled & )\
{\
}\
catch ( const std::exception &e )\
{\
  std::string message ( e.what() ? e.what() : "Standard exception caught" );\
  Usul::Errors::Stack::instance().push ( error_id, message.c_str() );\
}\
catch ( ... )\
{\
  Usul::Errors::Stack::instance().push ( error_id, "Unknown exception caught" );\
}
#else
#define USUL_EXCEPTION_SAFE_END(error_id)
#endif

}; // namespace Errors
}; // namespace Usul


#endif // _USUL_ERROR_STACK_H_
