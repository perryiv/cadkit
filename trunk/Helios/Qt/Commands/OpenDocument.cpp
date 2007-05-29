
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

#include "Helios/Qt/Commands/OpenDocument.h"
#include "Helios/Qt/Core/Constants.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"
#include "Usul/Strings/Qt.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Threads/Named.h"
#include "Usul/Trace/Trace.h"

#include "QtGui/QFileDialog"

using namespace CadKit::Helios::Commands;

USUL_IMPLEMENT_TYPE_ID ( OpenDocument );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenDocument::OpenDocument ( IUnknown *caller ) : BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "&Open" );
  this->shortcut ( "Ctrl+O" );
  this->statusTip ( "Open existing document" );
  this->toolTip ( "Open existing document" );
  this->iconPath ( "openDocument.png" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenDocument::~OpenDocument()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command. This function is re-entrant.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocument::_execute()
{
  USUL_TRACE_SCOPE;
  // Do not lock the mutex. This function is re-entrant.

  // Get file names.
  FileNames files;
  this->_askForFileNames ( "Open Document", files );

  // Loop through the files.
  for ( FileNames::const_iterator i = files.begin(); i != files.end(); ++i )
  {
    const std::string file ( *i );
    Usul::Functions::safeCallR1 ( Usul::Adaptors::memberFunction ( this, &OpenDocument::_open ), file, "9974193900" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocument::_askForFileNames ( const std::string &title, FileNames &files )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Initialize.
  files.clear();

  // Only GUI thread.
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  // Get the last directory.
  const std::string dir ( this->_lastDirectory() );

  // Get the filter string.
  const std::string filters ( this->_filters() );

  // Get the file names.
  const QStringList answer ( QFileDialog::getOpenFileNames ( 0x0, QString ( title.c_str() ), QString ( dir.c_str() ), QString ( filters.c_str() ) ) );

  // If there are no file names...
  if ( true == answer.empty() )
    return;

  // Set the directory.
  this->_lastDirectory ( dir );

  // Set the files.
  Usul::Strings::convert ( answer, files );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string OpenDocument::_lastDirectory() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  std::string dir ( Usul::CommandLine::Arguments::instance().directory() );
  return dir;
  //CadKit::Helios::Core::SettingsGroupScope group ( CadKit::Helios::Core::Registry::Sections::FILE_DIALOG, mw->settings() );
  //dir = mw->settings().value ( CadKit::Helios::Core::Registry::Keys::GEOMETRY.c_str(), CadKit::Helios::Core::Registry::Defaults::GEOMETRY ).value<QString>();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the last directory.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocument::_lastDirectory ( const std::string &dir )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  //CadKit::Helios::Core::SettingsGroupScope group ( CadKit::Helios::Core::Registry::Sections::FILE_DIALOG, mw->settings() );
  //mw->settings().setValue ( CadKit::Helios::Core::Registry::Keys::GEOMETRY.c_str(), dir );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters.
//
///////////////////////////////////////////////////////////////////////////////

std::string OpenDocument::_filters() const
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this->mutex() ); Do you need to guard?
  std::string filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the file. This function is re-entrant.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocument::_open ( const std::string &file )
{
  USUL_TRACE_SCOPE;
  // Do not lock the mutex. This function is re-entrant.

  Usul::Interfaces::IThreadPoolAddTask::QueryPtr pool ( this->caller() );
  if ( true == pool.valid() )
  {
    Usul::Threads::Callback::RefPtr started   ( Usul::Threads::newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &OpenDocument::_threadStarted   ) ) );
    Usul::Threads::Callback::RefPtr finished  ( Usul::Threads::newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &OpenDocument::_threadFinished  ) ) );
    Usul::Threads::Callback::RefPtr error     ( Usul::Threads::newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &OpenDocument::_threadError     ) ) );
    Usul::Threads::Callback::RefPtr cancelled ( Usul::Threads::newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &OpenDocument::_threadCancelled ) ) );
    OpenDocument::TaskHandle task ( pool->addTask ( started.get(), finished.get(), cancelled.get(), error.get(), 0x0 ) );

    //HERE
    // Need to add user-data to the task. 
    // The user-data is an instance of a "task" class that opens the files.
    // Make a new task for each file.
    // When the pool's task finishes it deletes its user-data... the task class.

    // Save the file with the task.
    this->_addTaskedFile ( task, file );

    // Reference this instance for each task.
    this->ref();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the thread starts.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocument::_threadStarted ( Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE;
  const std::string file ( this->_getTaskedFile ( thread ) );

  std::cout << Usul::Strings::format ( "Opening file: ", file ) << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the thread finished.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocument::_threadFinished ( Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE;
  this->_removeTaskedFile ( thread );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called if the thread fails.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocument::_threadError ( Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE;
  this->_removeTaskedFile ( thread );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called if the thread fails.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocument::_threadCancelled ( Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE;
  this->_removeTaskedFile ( thread );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the file to the map.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocument::_addTaskedFile ( TaskHandle task, const std::string &file )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _taskedFiles[task] = file;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the file from the map.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocument::_removeTaskedFile ( Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE;

  bool found ( false );
  if ( 0x0 != thread && true == thread->isTask() )
  {
    Guard guard ( this->mutex() );
    TaskedFiles::iterator i ( _taskedFiles.find ( thread->task() ) );
    if ( _taskedFiles.end() != i )
    {
      _taskedFiles.erase ( i );
      found = true;
    }
  }

  // Have to do this after the guard goes out of scope.
  if ( true == found )
  {
    this->unref();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file from the map.
//
///////////////////////////////////////////////////////////////////////////////

std::string OpenDocument::_getTaskedFile ( Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != thread && true == thread->isTask() )
  {
    Guard guard ( this->mutex() );
    TaskedFiles::const_iterator i ( _taskedFiles.find ( thread->task() ) );
    return ( ( _taskedFiles.end() == i ) ? std::string ( "" ) : i->second );
  }

  return std::string ( "" );
}
