
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command to toggle the visibility.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_QT_COMMANDS_WORK_OFFLINE_H__
#define __CADKIT_HELIOS_QT_COMMANDS_WORK_OFFLINE_H__

#include "Helios/Qt/Commands/Command.h"

class QWidget;

namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT WorkOffline : public Usul::Commands::Command
{
public:

  // Typedefs
  typedef Usul::Commands::Command               BaseClass;
  typedef Usul::Interfaces::IUnknown            IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( WorkOffline );

  // Constructor.
  WorkOffline();

protected:

  // Use reference counting.
  virtual ~WorkOffline();

  // Execute.
  virtual void              _execute();

  // Update the enabled flag.
  virtual bool              updateEnable () const;

  // Update the checked state.
  virtual bool              updateCheck () const;
};

}
}
}


#endif // __CADKIT_HELIOS_QT_COMMANDS_WORK_OFFLINE_H__
