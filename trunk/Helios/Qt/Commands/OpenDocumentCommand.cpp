
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
#include "Helios/Qt/Core/MainWindow.h"
#include "Helios/Qt/Core/SettingsGroupScope.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

#include "QtGui/QFileDialog"

using namespace CadKit::Helios::Commands;

USUL_IMPLEMENT_TYPE_ID ( OpenDocumentCommand );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenDocumentCommand::OpenDocumentCommand ( CadKit::Helios::Core::MainWindow *mw ) : BaseClass ( mw )
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

  // Check thread.
  CadKit::Helios::Core::MainWindow *mw ( this->_getMainWindow() );
  if ( 0x0 == mw || false == mw->isGuiThread() )
    return;

  // Get the last directory.
  QString dir;
  {
    Guard mwGuard ( mw->mutex() );
    CadKit::Helios::Core::SettingsGroupScope group ( CadKit::Helios::Core::Registry::Sections::FILE_DIALOG, mw->settings() );
    dir = mw->settings().value ( CadKit::Helios::Core::Registry::Keys::GEOMETRY.c_str(), CadKit::Helios::Core::Registry::Defaults::GEOMETRY ).value<QString>();
  }

  // Get the filter string.
  QString filter;

  // Get the file names.
  QStringList answer ( QFileDialog::getOpenFileNames ( mw, title.c_str(), dir, filter ) );

  // If there are no file names...
  if ( true == answer.empty() )
    return;

  // Set the directory.
  {
    Guard mwGuard ( mw->mutex() );
    CadKit::Helios::Core::SettingsGroupScope group ( CadKit::Helios::Core::Registry::Sections::FILE_DIALOG, mw->settings() );
    mw->settings().setValue ( CadKit::Helios::Core::Registry::Keys::GEOMETRY.c_str(), dir );
  }

  // Set the files.
  files.reserve ( answer.size() );
  for ( QStringList::const_iterator i = answer.begin(); i != answer.end(); ++i )
  {
    const QString &file ( *i );
    QByteArray bytes ( file.toAscii() );
    files.push_back ( std::string ( bytes.begin(), bytes.end() ) );
  }
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

  HERE
  // De-couple from MainWindow with interfaces.
  // Launch a thread here to do the work.
  // Probably want each command to contain an action.
  // Make different commands for menubar and toolbar, but have them point to the same functions.
}
