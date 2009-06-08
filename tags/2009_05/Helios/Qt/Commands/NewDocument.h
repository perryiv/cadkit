
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command to open a document.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_COMMANDS_NEW_DOCUMENT_COMMAND_H_
#define _CADKIT_HELIOS_QT_COMMANDS_NEW_DOCUMENT_COMMAND_H_

#include "Helios/Qt/Commands/Command.h"


namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT NewDocument : public CadKit::Helios::Commands::Command
{
public:

  // Typedefs.
  typedef CadKit::Helios::Commands::Command     BaseClass;
  typedef Usul::Interfaces::IUnknown            IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( NewDocument );

  // Constructor.
  NewDocument ( IUnknown *caller, IUnknown *component, const std::string& name );

protected:

  // Use reference counting.
  virtual ~NewDocument();
  NewDocument ( const NewDocument & );
  NewDocument &operator = ( const NewDocument & );

  virtual void              _execute();

  static void               _initNewDocument ( IUnknown *document, IUnknown *caller );

private:

  IUnknown::QueryPtr _component;
};


} // namespace Commands
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_COMMANDS_NEW_DOCUMENT_COMMAND_H_
