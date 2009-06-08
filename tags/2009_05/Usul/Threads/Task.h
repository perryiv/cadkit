
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
#include "Usul/Threads/Callback.h"


namespace Usul {
namespace Threads {


class USUL_EXPORT Task : public Usul::Base::Object
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Threads::Callback Callback;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Task );

  // Constructor
  Task ( unsigned long id, Callback *started, Callback *finished, Callback *cancelled, Callback *error );

  unsigned long             id() const;
  Callback *                cancelledCB();
  Callback *                errorCB();
  Callback *                finishedCB();
  Callback *                startedCB();

  virtual std::string       name() const;
  void                      name ( const std::string & );

protected:

  // Destructor
  virtual ~Task();

	unsigned long _id;
  Callback::RefPtr _cancelledCB;
  Callback::RefPtr _errorCB;
  Callback::RefPtr _finishedCB;
  Callback::RefPtr _startedCB;

private:

  // No copying or assignment.
  Task ( const Task & );
  Task &operator = ( const Task & );

  void                      _destroy();

};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_POOL_TASK_CLASS_H_
