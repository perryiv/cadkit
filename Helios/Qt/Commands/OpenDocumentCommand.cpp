
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

#include "Helios/Qt/Commands/OpenDocumentCommand.h"
#include "Helios/Qt/Core/Constants.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Qt.h"
#include "Usul/Threads/Named.h"
#include "Usul/Trace/Trace.h"

#include "QtGui/QFileDialog"

using namespace CadKit::Helios::Commands;

USUL_IMPLEMENT_TYPE_ID ( OpenDocumentCommand );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenDocumentCommand::OpenDocumentCommand ( IUnknown *caller ) : BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenDocumentCommand::~OpenDocumentCommand()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command. This function is re-entrant.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocumentCommand::execute ( Usul::Interfaces::IUnknown *caller )
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
    Usul::Functions::safeCallR1 ( Usul::Adaptors::memberFunction ( this, &OpenDocumentCommand::_open ), file, "9974193900" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocumentCommand::_askForFileNames ( const std::string &title, FileNames &files )
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

std::string OpenDocumentCommand::_lastDirectory() const
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

void OpenDocumentCommand::_lastDirectory ( const std::string &dir )
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

std::string OpenDocumentCommand::_filters() const
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

void OpenDocumentCommand::_open ( const std::string &file )
{
  USUL_TRACE_SCOPE;
  // Do not lock the mutex. This function is re-entrant.

  // Launch a thread here to do the work.
  // Probably want each command to contain an action.
  // Make different commands for menubar and toolbar, but have them point to the same functions.
}
