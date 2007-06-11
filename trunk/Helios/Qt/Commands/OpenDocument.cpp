
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

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Strings/Format.h"
#include "Usul/Strings/Qt.h"
#include "Usul/System/Sleep.h"
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
//  Constructor for job class.
//
///////////////////////////////////////////////////////////////////////////////

OpenDocument::Job::Job ( const std::string &name, Usul::Interfaces::IUnknown *caller ) : 
  OpenDocument::Job::BaseClass(),
  _name   ( name ),
  _caller ( caller )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor for job class.
//
///////////////////////////////////////////////////////////////////////////////

OpenDocument::Job::~Job()
{
  USUL_TRACE_SCOPE;
  _caller = 0x0;
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
    Usul::Jobs::Manager::instance().add ( new OpenDocument::Job ( file, this->caller() ) );
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters.
//
///////////////////////////////////////////////////////////////////////////////

std::string OpenDocument::_filters() const
{
  USUL_TRACE_SCOPE;
  std::string filters;
  return filters;
}
    

///////////////////////////////////////////////////////////////////////////////
//
//  Called when the job starts.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocument::Job::_started()
{
  USUL_TRACE_SCOPE;
  std::cout << Usul::Strings::format ( "Opening file: ", _name, ", thread = ", this->thread()->id() ) << Usul::Resources::TextWindow::endl;
  Usul::System::Sleep::seconds ( 1 );
}
