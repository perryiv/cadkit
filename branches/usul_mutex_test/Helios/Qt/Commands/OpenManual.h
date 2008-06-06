
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command to export the viewer's model.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_QT_COMMANDS_OPEN_MANUAL_H__
#define __CADKIT_HELIOS_QT_COMMANDS_OPEN_MANUAL_H__

#include "Helios/Qt/Commands/Command.h"

#include "Usul/Documents/Document.h"


namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT OpenManual : public Usul::Commands::Command
{
public:

  // Typedefs
  typedef Usul::Commands::Command               BaseClass;
  typedef Usul::Interfaces::IUnknown            IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( OpenManual );

  // Constructor.
  OpenManual ( IUnknown *caller, const std::string &manual );

protected:

  // Use reference counting.
  virtual ~OpenManual();

  // Get the name of the manual file.
  std::string               _manualFile() const;

  // Execute.
  virtual void              _execute();

  // Update the enabled flag.
  virtual bool              updateEnable() const;

private:

  std::string _manual;
};

}
}
}


#endif // __CADKIT_HELIOS_QT_COMMANDS_OPEN_MANUAL_H__
