
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

#include "Usul/Commands/Command.h"

#include "QtGui/QAction"

#include "boost/shared_ptr.hpp"
#include "boost/function.hpp"

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
  typedef boost::function<void ()> ExecuteCallback;

  // Constructor.
  Action ( Usul::Commands::Command *command );
  Action ( const std::string& text, ExecuteCallback callback );

  // Destructor.
  virtual ~Action();

private slots:

  void                _slot();

private:

  // No copying or assignment.
  Action ( const Action & );
  Action &operator = ( const Action & );

  virtual void _execute();

  ExecuteCallback _executeCallback;
};


} // namespace QtTools


#endif //_CADKIT_HELIOS_QT_COMMANDS_ACTION_CLASS_H_
