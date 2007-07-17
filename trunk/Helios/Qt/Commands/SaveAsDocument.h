
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command to save a document as another document.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_QT_COMMANDS_SAVE_AS_DOCUMENT_H__
#define __CADKIT_HELIOS_QT_COMMANDS_SAVE_AS_DOCUMENT_H__

#include "Helios/Qt/Commands/Command.h"

#include "Usul/Documents/Document.h"


namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT SaveAsDocument : public Usul::Commands::Command
{
public:

  // Typedefs
  typedef Usul::Commands::Command               BaseClass;
  typedef Usul::Interfaces::IUnknown            IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SaveAsDocument );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( SaveAsDocument );

  // Constructor.
  SaveAsDocument ( IUnknown *caller );

protected:
  // Use reference counting.
  virtual ~SaveAsDocument();

  // Execute.
  virtual void              _execute();

private:

  // No copying or assignment.
  SaveAsDocument ( const SaveAsDocument & );
  SaveAsDocument &operator = ( const SaveAsDocument & );
};

}
}
}


#endif // __CADKIT_HELIOS_QT_COMMANDS_SAVE_AS_DOCUMENT_H__
