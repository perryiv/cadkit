
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
//  Target for QAction.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_COMMANDS_TARGET_CLASS_H_
#define _CADKIT_HELIOS_QT_COMMANDS_TARGET_CLASS_H_

#include "Helios/Qt/Commands/Command.h"

#include "QtCore/QObject"


namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT Target : public QObject
{
public:

  // Typedefs.
  typedef QObject BaseClass;

  // Constructor.
  Target ( Command *command );

  // Destructor.
  virtual ~Target();

private:

  // No copying or assignment.
  Target ( const Target & );
  Target &operator = ( const Target & );

  void                      _destroy();

  void                      _slot();

  Command::RefPtr _command;
};


} // namespace Commands
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_COMMANDS_TARGET_CLASS_H_
