
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Pool task class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_POOL_TASK_CLASS_H_
#define _USUL_THREADS_POOL_TASK_CLASS_H_

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/Callback.h"


namespace Usul {
namespace Threads {


class Task : public Usul::Base::Referenced
{
public:

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Threads::Callback Callback;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Task );

  // Constructor
  Task ( unsigned long id, Callback *started, Callback *finished, Callback *cancelled, Callback *error );

  unsigned long   id() const { return _id; }
  Callback *      cancelledCB() { return _cancelledCB; }
  Callback *      errorCB()    { return _errorCB; }
  Callback *      finishedCB() { return _finishedCB; }
  Callback *      startedCB()  { return _startedCB; }

protected:

  // Destructor
  virtual ~Task();

private:

  unsigned long _id;
  Callback::RefPtr _cancelledCB;
  Callback::RefPtr _errorCB;
  Callback::RefPtr _finishedCB;
  Callback::RefPtr _startedCB;
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_POOL_TASK_CLASS_H_
