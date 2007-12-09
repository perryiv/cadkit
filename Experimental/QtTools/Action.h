
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
//  Action class that executes a command.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_COMMANDS_ACTION_CLASS_H_
#define _CADKIT_HELIOS_QT_COMMANDS_ACTION_CLASS_H_

#include "QtTools/Export.h"
#include "QtTools/Image.h"

#include "Usul/Commands/Command.h"
#include "Usul/Commands/History.h"
#include "Usul/Threads/Named.h"
#include "Usul/Trace/Trace.h"

#include "QtGui/QAction"

#include "boost/shared_ptr.hpp"

#include <stdexcept>


namespace QtTools {


class QT_TOOLS_EXPORT Action : public QAction
{
  Q_OBJECT;
public:

  // Typedefs.
  typedef QAction BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef boost::shared_ptr<Action> RefPtr;

  // Constructor.
  Action ( Usul::Commands::Command *command );

  // Destructor.
  virtual ~Action();

private slots:

  void                _slot();

private:

  // No copying or assignment.
  Action ( const Action & );
  Action &operator = ( const Action & );

  virtual void _execute();

  Usul::Commands::Command::RefPtr _command;
};


} // namespace QtTools


#endif //_CADKIT_HELIOS_QT_COMMANDS_ACTION_CLASS_H_
