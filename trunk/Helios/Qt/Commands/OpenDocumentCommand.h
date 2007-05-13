
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command to open a document.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_CORE_OPEN_DOCUMENT_COMMAND_H_
#define _CADKIT_HELIOS_QT_CORE_OPEN_DOCUMENT_COMMAND_H_

#include "Helios/Qt/Commands/Export.h"

#include "Helios/Qt/Core/Command.h"

#include <vector>


namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT OpenDocumentCommand : public CadKit::Helios::Core::Command
{
public:

  // Typedefs.
  typedef CadKit::Helios::Core::Command BaseClass;
  typedef std::vector<std::string> FileNames;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( OpenDocumentCommand );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( OpenDocumentCommand );

  // Constructor.
  OpenDocumentCommand ( CadKit::Helios::Core::MainWindow * );

  // Execute the command.
  virtual void              execute ( Usul::Interfaces::IUnknown *caller );

protected:

  // Use reference counting.
  virtual ~OpenDocumentCommand();

  void                      _askForFileNames ( const std::string &title, FileNames &files );

  void                      _open ( const std::string & );

private:

  // No copying or assignment.
  OpenDocumentCommand ( const OpenDocumentCommand & );
  OpenDocumentCommand &operator = ( const OpenDocumentCommand & );
};


} // namespace Commands
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_CORE_OPEN_DOCUMENT_COMMAND_H_
