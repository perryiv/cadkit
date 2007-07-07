
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
//  Command to export an image.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_QT_COMMANDS_EXPORT_IMAGE_H__
#define __CADKIT_HELIOS_QT_COMMANDS_EXPORT_IMAGE_H__

#include "Helios/Qt/Commands/Command.h"

#include "Usul/Documents/Document.h"


namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT ExportImage : public Usul::Commands::Command
{
public:

  // Typedefs
  typedef Usul::Commands::Command               BaseClass;
  typedef Usul::Interfaces::IUnknown            IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ExportImage );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( ExportImage );

  // Constructor.
  ExportImage ( IUnknown *caller );

protected:
  // Use reference counting.
  virtual ~ExportImage();

  // Execute.
  virtual void              _execute();

private:

  // No copying or assignment.
  ExportImage ( const ExportImage & );
  ExportImage &operator = ( const ExportImage & );
};

}
}
}


#endif // __CADKIT_HELIOS_QT_COMMANDS_EXPORT_IMAGE_H__
