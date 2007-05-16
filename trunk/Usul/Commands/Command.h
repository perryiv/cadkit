
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

#ifndef _USUL_COMMANDS_COMMAND_CLASS_H_
#define _USUL_COMMANDS_COMMAND_CLASS_H_

#include "Usul/Export/Export.h"
#include "Usul/Base/Object.h"

#include "Usul/Interfaces/ICommand.h"

#include <string>


namespace Usul {
namespace Commands {


class USUL_EXPORT Command : public Usul::Base::Object,
                            public Usul::Interfaces::ICommand
{
public:

  // Typedefs.
  typedef Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Command );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( Command );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Get/Set the text.
  void                         text( const std::string& value );
  const std::string&           text() const;
  
  /// Get/Set the tooltip.
  void                         tooltip ( const std::string& value );
  const std::string&           tooltip() const;
  
  /// Get/Set the shortcut.
  void                         shortcut ( const std::string& value );
  const std::string&           shortcut() const;
  
  /// Get/Set the icon path.
  void                         iconPath ( const std::string& value );
  const std::string&           iconPath() const;

protected:

  // Constructor.
  Command ( IUnknown *caller );

  // Use reference counting.
  virtual ~Command();

  const IUnknown *          _getCaller() const;
  IUnknown *                _getCaller();

private:

  // No copying or assignment.
  Command ( const Command & );
  Command &operator = ( const Command & );

  void                      _destroy();

  IUnknown::RefPtr _caller;
  std::string      _text;
  std::string      _toolTipText;
  std::string      _shortcut;
  std::string      _iconPath;
  
};


} // namespace Commands
} // namespace Usul


#endif //_USUL_COMMANDS_COMMAND_CLASS_H_
