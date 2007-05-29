
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command to open a document.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_COMMANDS_OPEN_DOCUMENT_COMMAND_H_
#define _CADKIT_HELIOS_QT_COMMANDS_OPEN_DOCUMENT_COMMAND_H_

#include "Helios/Qt/Commands/Command.h"

#include "Usul/Interfaces/Threads/IThreadPoolAddTask.h"

#include <vector>
#include <map>

namespace Usul { namespace Threads { class Thread; } }


namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT OpenDocument : public CadKit::Helios::Commands::Command
{
public:

  // Typedefs.
  typedef CadKit::Helios::Commands::Command BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::vector<std::string> FileNames;
  typedef Usul::Interfaces::IThreadPoolAddTask IThreadPoolAddTask;
  typedef IThreadPoolAddTask::TaskHandle TaskHandle;
  typedef std::map<TaskHandle,std::string> TaskedFiles;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( OpenDocument );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( OpenDocument );

  // Constructor.
  OpenDocument ( IUnknown *caller );

protected:

  // Use reference counting.
  virtual ~OpenDocument();

  void                      _addTaskedFile ( TaskHandle, const std::string &file );

  void                      _askForFileNames ( const std::string &title, FileNames &files );

  virtual void              _execute();

  std::string               _filters() const;

  std::string               _getTaskedFile ( Usul::Threads::Thread *thread );

  std::string               _lastDirectory() const;
  void                      _lastDirectory ( const std::string & );

  void                      _open ( const std::string & );

  void                      _removeTaskedFile ( Usul::Threads::Thread *thread );

  void                      _threadStarted   ( Usul::Threads::Thread *thread );
  void                      _threadFinished  ( Usul::Threads::Thread *thread );
  void                      _threadError     ( Usul::Threads::Thread *thread );
  void                      _threadCancelled ( Usul::Threads::Thread *thread );

private:

  // No copying or assignment.
  OpenDocument ( const OpenDocument & );
  OpenDocument &operator = ( const OpenDocument & );

  TaskedFiles _taskedFiles;
};


} // namespace Commands
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_COMMANDS_OPEN_DOCUMENT_COMMAND_H_
