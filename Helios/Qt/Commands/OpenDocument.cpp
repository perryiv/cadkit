
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
#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/GUI/ILoadFileDialog.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Strings/Format.h"
#include "Usul/Strings/Qt.h"
#include "Usul/System/Sleep.h"
#include "Usul/Threads/Named.h"
#include "Usul/Trace/Trace.h"
#include "Usul/File/Path.h"
#include "Usul/App/Controller.h"

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
  FileNames files ( this->_askForFileNames ( "Open Document" ) );

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

OpenDocument::FileNames OpenDocument::_askForFileNames ( const std::string &title )
{
  USUL_TRACE_SCOPE;
  // Do not lock the mutex. This function is re-entrant.

  // Get the needed interface.
  ILoadFileDialog::QueryPtr dialog ( this->caller() );
  if ( false == dialog.valid() )
    return FileNames();

  // Get appropriate filters.
  Filters filters ( Usul::Documents::Manager::instance().filtersOpen() );

  // Get the file names.
  ILoadFileDialog::FilesResult result ( dialog->getLoadFileNames ( title, filters ) );
  return result.first;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the job starts.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocument::Job::_started()
{
  USUL_TRACE_SCOPE;

  // Open the document.
  Usul::App::Controller::instance().documentOpen ( _name, _caller );

  std::cout << Usul::Strings::format ( "Opening file: ", _name, ", thread = ", this->thread()->id() ) << Usul::Resources::TextWindow::endl;
  Usul::System::Sleep::seconds ( 1 );
}
