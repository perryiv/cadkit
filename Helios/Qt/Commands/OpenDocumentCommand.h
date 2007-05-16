
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

#include "Usul/Commands/Command.h"

#include <vector>


namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT OpenDocumentCommand : public Usul::Commands::Command
{
public:

  // Typedefs.
  typedef CadKit::Helios::Core::Command BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::vector<std::string> FileNames;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( OpenDocumentCommand );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( OpenDocumentCommand );

  // Constructor.
  OpenDocumentCommand ( IUnknown *caller );

  // Execute the command.
  virtual void              execute ( IUnknown *caller );

protected:

  // Use reference counting.
  virtual ~OpenDocumentCommand();

  void                      _askForFileNames ( const std::string &title, FileNames &files );

  std::string               _filters() const;

  std::string               _lastDirectory() const;
  void                      _lastDirectory ( const std::string & );

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
