
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for the action.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_COMMANDS_BASE_ACTION_CLASS_H_
#define _CADKIT_HELIOS_QT_COMMANDS_BASE_ACTION_CLASS_H_

#include "Helios/Qt/Commands/Export.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include "QtGui/QAction"

#include "boost/shared_ptr.hpp"


namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT BaseAction : public QAction
{
  Q_OBJECT

public:

  // Typedefs.
  typedef QAction BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef boost::shared_ptr<BaseAction> RefPtr;

  // Destructor.
  virtual ~BaseAction();

  // Return the caller.
  const IUnknown *    caller() const;
  IUnknown *          caller();
  void                caller ( IUnknown * );

  // Get the mutex.
  Mutex &             mutex() const;

protected:

  // Constructor.
  BaseAction ( IUnknown *caller );

private slots:

  void                _slot();

private:

  // No copying or assignment.
  BaseAction ( const BaseAction & );
  BaseAction &operator = ( const BaseAction & );

  void                _destroy();

  virtual void        _execute() = 0;

  IUnknown::RefPtr _caller;
  mutable Mutex *_mutex;
};


} // namespace Commands
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_COMMANDS_BASE_ACTION_CLASS_H_
