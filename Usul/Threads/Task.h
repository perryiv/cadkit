
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

#include "Usul/Base/Object.h"

#include "boost/function.hpp"

namespace Usul {
namespace Threads {


class USUL_EXPORT Task : public Usul::Base::Object
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef boost::function<void ()> Callback;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Task );

  // Constructor
  Task ( unsigned long id, Callback started, Callback finished, Callback cancelled, Callback error );

  unsigned long id() const;
  void cancelled();
  void error();
  void finished();
  void started();

  virtual std::string       name() const;
  void                      name ( const std::string & );

protected:

  // Destructor
  virtual ~Task();

	unsigned long _id;
  Callback _cancelledCB;
  Callback _errorCB;
  Callback _finishedCB;
  Callback _startedCB;

private:

  // No copying or assignment.
  Task ( const Task & );
  Task &operator = ( const Task & );

};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_POOL_TASK_CLASS_H_
