
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base callback class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_BASE_CALLBACK_CLASS_H_
#define _USUL_THREADS_BASE_CALLBACK_CLASS_H_

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

namespace Usul { namespace Threads { class Thread; } }


namespace Usul {
namespace Threads {


///////////////////////////////////////////////////////////////////////////////
//
//  Base callback class.
//
///////////////////////////////////////////////////////////////////////////////

class USUL_EXPORT Callback : public Usul::Base::Referenced
{
public:

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Callback );

  // Overload this function.
  virtual void operator() ( Usul::Threads::Thread * ) = 0;

protected:

  // Constructor.
  Callback();

  // Use reference counting.
  virtual ~Callback();
};


///////////////////////////////////////////////////////////////////////////////
//
//  Callback class.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function > class FunctionCallback : public Callback
{
public:

  // Useful typedefs.
  typedef Callback BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( FunctionCallback );

  // Constructor.
  FunctionCallback ( Function f ) : BaseClass(),
    _f ( f )
  {
  }

  // Overload this function.
  virtual void operator() ( Usul::Threads::Thread *thread )
  {
    _f ( thread );
  }

protected:

  // Use reference counting.
  virtual ~FunctionCallback()
  {
  }

private:

  Function _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to make a FunctionCallback.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function > Callback *newFunctionCallback ( Function f )
{
  return new FunctionCallback<Function> ( f );
}


}; // namespace Threads
}; // namespace Usul


#endif // _USUL_THREADS_BASE_CALLBACK_CLASS_H_
