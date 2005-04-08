
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
#include <sstream>

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
  typedef std::list < std::string > StringList;
  typedef StringList::size_type size_type;
  typedef StringList::value_type value_type;
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
//  Catch and eat cancel-exceptions.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_EAT_CANCEL_EXCEPTIONS\
catch ( const Usul::Exceptions::Canceled & )\
{\
}

///////////////////////////////////////////////////////////////////////////////
//
//  Catch and re-throw cancel-exceptions.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_RETHROW_CANCEL_EXCEPTIONS\
catch ( const Usul::Exceptions::Canceled &e )\
{\
  throw e;\
}

///////////////////////////////////////////////////////////////////////////////
//
//  Catch standard exceptions.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_CATCH_STD_EXCEPTIONS(error_id)\
catch ( const std::exception &e )\
{\
  std::ostringstream message;\
  message << "Error " << error_id << ": Standard exception caught";\
  Usul::Errors::Stack::instance().push ( message.str() );\
  Usul::Errors::Stack::instance().push ( e.what() );\
}


///////////////////////////////////////////////////////////////////////////////
//
//  Catch all exceptions.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_CATCH_ALL_EXCEPTIONS(error_id)\
catch ( ... )\
{\
  std::ostringstream message;\
  message << "Error " << error_id << ": Unknown exception caught";\
  Usul::Errors::Stack::instance().push ( message.str() );\
}


}; // namespace Errors
}; // namespace Usul


#endif // _USUL_ERROR_STACK_H_
