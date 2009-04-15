
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
#include "Usul/Interfaces/GUI/IGUIDelegateNotify.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Strings/Format.h"
#include "Usul/Strings/Qt.h"
#include "Usul/System/Sleep.h"
#include "Usul/Threads/Named.h"
#include "Usul/Trace/Trace.h"
#include "Usul/File/Path.h"
#include "Usul/System/Clock.h"

using namespace CadKit::Helios::Commands;

USUL_IMPLEMENT_COMMAND ( OpenDocument );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenDocument::OpenDocument ( IUnknown *caller ) : BaseClass ( caller ),
_filename ()
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
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenDocument::OpenDocument ( const OpenDocument& rhs ) : BaseClass ( rhs ),
_filename ( rhs._filename )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

OpenDocument& OpenDocument::operator = ( const OpenDocument& rhs ) 
{
  USUL_TRACE_SCOPE;
  BaseClass::operator = ( rhs );
  _filename = rhs._filename;
  return *this;
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

OpenDocument::Job::Job ( Document::RefPtr doc, const std::string &name, IUnknown::RefPtr caller ) : 
  OpenDocument::Job::BaseClass( caller ),
  _document ( doc ),
  _name     ( name ),
  _caller   ( caller )
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
  _document = 0x0;
  _name.clear();
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

  // Get the filename we have.  May be empty.
  std::string file ( this->filename () );

  // If we already have one, open it.
  if ( false == file.empty () )
    this->_startJob ( file );

  // No filename.  Ask the user for filenames.
  else
  {
    // Get file names.
    FileNames files ( this->_askForFileNames ( "Open Document" ) );

    // Loop through the files.
    for ( FileNames::const_iterator i = files.begin(); i != files.end(); ++i )
    {
      const std::string file ( *i );
      try
      {
        this->_startJob ( file );
      }
      catch ( const std::exception& e )
      {
        std::cout << "Error 7531978100: Standard exception caught while trying to create job for " << file << std::endl;
        std::cout << "Message: " << e.what() << std::endl;
      }
      catch ( ... )
      {
        std::cout << "Error 2160956073: Unknown exception caught while trying to create job for " << file << std::endl;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start a job.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocument::_startJob ( const std::string& file )
{
  USUL_TRACE_SCOPE;

  // Typedefs.
  typedef Usul::Documents::Manager Manager;
  typedef Manager::DocumentInfo    DocumentInfo;

  // Get needed information for the filename.
  DocumentInfo info ( Manager::instance().find ( file, this->caller() ) );

  // Make sure a document was found.
  if ( false == info.document.valid() )
    Usul::Exceptions::Thrower < std::runtime_error > ( "Error 2771743457: Could not find document for file: ", file );

  // Make sure a delegate was found.
  if ( false == info.delegate.valid() )
    Usul::Exceptions::Thrower < std::runtime_error > ( "Error 4522012370: Could not find delegate for file: ", file );

  // If the document is loaded, pop the windows of the document forward.
  if ( info.loaded )
  {
    info.document->windowsForward();
    return;
  }

  // Set the delegate.
  info.document->delegate ( info.delegate );

  // Create and add the job to the job manager.
  Usul::Jobs::Manager::instance().addJob ( Usul::Jobs::Job::RefPtr ( new OpenDocument::Job ( info.document, file, this->caller() ) ) );
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
  ILoadFileDialog::QueryPtr dialog ( this->caller().get() );
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

  // If we have a valid document...
  if ( _document.valid() )
  {
    // Feedback.
    std::cout << Usul::Strings::format ( "Opening file: ", _name );
    #ifdef _DEBUG
    //std::cout << Usul::Strings::format ( ", thread = ", this->thread()->id() );
    #endif
    std::cout << Usul::Resources::TextWindow::endl;
    
    // Initialize start time.
    Usul::Types::Uint64 start ( Usul::System::Clock::milliseconds() );

    // Lets open the document.
    _document->open ( _name, _caller, this->progress() );

    // Feedback.
    const double seconds ( static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 );
    std::cout << Usul::Strings::format ( seconds, " seconds ... Time to open ", _name ) << Usul::Resources::TextWindow::endl;

    // See if the caller wants to be notified when the document finishes loading.
    Usul::Interfaces::IGUIDelegateNotify::QueryPtr notify ( _caller );

    // Notify.
    if ( notify.valid() )
      notify->notifyDocumentFinishedLoading ( _document );

    // Add the document to the manager.
    Usul::Documents::Manager::instance().add ( _document );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the filename to open.
//
///////////////////////////////////////////////////////////////////////////////

void OpenDocument::filename ( const std::string& filename )
{
  Guard guard ( this->mutex () );
  _filename = filename;

  this->text ( _filename );
  this->shortcut ( "" );
  this->statusTip ( "" );
  this->toolTip ( "" );
  this->iconPath ( "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filename to open.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& OpenDocument::filename () const
{
  Guard guard ( this->mutex () );
  return _filename;
}
