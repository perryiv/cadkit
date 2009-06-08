
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command history.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_COMMANDS_HISTORY_CLASS_H_
#define _USUL_COMMANDS_HISTORY_CLASS_H_

#include "Usul/Export/Export.h"
#include "Usul/Base/Object.h"

#include "Usul/Interfaces/ICommand.h"

#include <stack>


namespace Usul {
namespace Commands {


class USUL_EXPORT History : public Usul::Base::Object
{
public:

  // Typedefs.
  typedef Object BaseClass;
  typedef Usul::Interfaces::ICommand ICommand;
  typedef std::stack<ICommand::RefPtr> CommandStack;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( History );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( History );

  // Constructor.
  History();

  // Add the recently executed command.
  void                        add ( ICommand * );
  
  // Can the command be undone or redone?
  bool                        canRedo() const;
  bool                        canUndo() const;
  
  // Clear the history.
  void                        clear();
  
  // Redo the command that was last undone.
  void                        redo();
  
  // Undo the last command.
  void                        undo();

protected:

  // Use reference counting.
  virtual ~History();

private:

  // No copying or assignment.
  History ( const History & );
  History &operator = ( const History & );

  void                        _destroy();

  CommandStack _done;
  CommandStack _undone;
};


} // namespace Commands
} // namespace Usul


#endif //_USUL_COMMANDS_HISTORY_CLASS_H_
