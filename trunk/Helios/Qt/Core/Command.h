
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all commands.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_CORE_COMMAND_CLASS_H_
#define _CADKIT_HELIOS_QT_CORE_COMMAND_CLASS_H_

#include "Helios/Qt/Core/Object.h"

#include "Usul/Interfaces/ICommand.h"

#include <string>

namespace CadKit { namespace Helios { namespace Core { class MainWindow; } } }


namespace CadKit {
namespace Helios {
namespace Core {


class HELIOS_QT_CORE_EXPORT Command : public Object,
                                      public Usul::Interfaces::ICommand
{
public:

  // Typedefs.
  typedef Object BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Command );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( Command );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

protected:

  // Constructor.
  Command ( MainWindow * );

  // Use reference counting.
  virtual ~Command();

  const MainWindow *        _getMainWindow() const;
  MainWindow *              _getMainWindow();

private:

  // No copying or assignment.
  Command ( const Command & );
  Command &operator = ( const Command & );

  MainWindow *_mainWindow;
};


} // namespace Core
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_CORE_COMMAND_CLASS_H_
