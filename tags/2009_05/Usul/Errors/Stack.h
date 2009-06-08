
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
#include "Usul/Errors/Error.h"

#include <list>
#include <string>
#include <sstream>
#include <stdexcept>

namespace Usul { namespace Threads { class Mutex; }; };


namespace Usul {
namespace Errors {


///////////////////////////////////////////////////////////////////////////////
//
//  The error stack.
//
///////////////////////////////////////////////////////////////////////////////

class USUL_EXPORT Stack
{
public:

  // Useful typedefs.
  typedef std::list < Usul::Errors::Error > ErrorStack;
  typedef ErrorStack::size_type size_type;
  typedef ErrorStack::value_type value_type;
  typedef Usul::Threads::Mutex Mutex;

  // Clear the stack.
  void                clear();

  // Is it empty?
  bool                empty() const;

  // Format the errors into single string.
  void                format ( const std::string &prefix, std::string &s ) const;
  std::string         format ( const std::string &prefix = std::string() ) const;

  // It's a singleton.
  static Stack &      instance();

  // Push an error.
  void                push ( const std::string &message );
  void                push ( const char *message );

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

  ErrorStack _s;
  Mutex *_m;
  static Stack *_instance;
};


} // namespace Errors
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Macros to wrap exceptions and push an error onto the stack.
//
//  Use:
//
//    USUL_ERROR_STACK_CATCH_EXCEPTIONS_BEGIN;
//    
//    Put code here.
//
//    USUL_ERROR_STACK_CATCH_EXCEPTIONS_END ( "some id" );
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_ERROR_STACK_CATCH_EXCEPTIONS_BEGIN try {

#define USUL_ERROR_STACK_CATCH_EXCEPTIONS_END(the_location_id) }\
  catch ( const std::exception &the_exception )\
  {\
    std::ostringstream out;\
    out << "Error " << the_location_id << ": Standard exception caught: " << the_exception.what();\
    Usul::Errors::Stack::instance().push ( out.str() );\
  }\
  catch ( ... )\
  {\
    std::ostringstream out;\
    out << "Error " << the_location_id << ": Unknown exception caught";\
    Usul::Errors::Stack::instance().push ( out.str() );\
  }


#endif // _USUL_ERROR_STACK_H_
