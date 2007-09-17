
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
//  Command to insert into a document.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_QT_COMMANDS_INSERT_DOCUMENT_H__
#define __CADKIT_HELIOS_QT_COMMANDS_INSERT_DOCUMENT_H__

#include "Helios/Qt/Commands/Command.h"

#include "Usul/Documents/Document.h"


namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT InsertDocument : public Usul::Commands::Command
{
public:

  // Typedefs
  typedef Usul::Commands::Command               BaseClass;
  typedef Usul::Interfaces::IUnknown            IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( InsertDocument );

  // Constructor.
  InsertDocument ( IUnknown *caller );

protected:
  // Use reference counting.
  virtual ~InsertDocument();

  // Execute.
  virtual void              _execute();

  // Update the enabled flag.
  virtual bool              updateEnable () const;
};

}
}
}


#endif // __CADKIT_HELIOS_QT_COMMANDS_INSERT_DOCUMENT_H__
