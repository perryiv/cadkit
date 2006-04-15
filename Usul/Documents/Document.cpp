
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document class. Helios windows have reference-counted smart-pointers to 
//  this class. Since fox deletes its own windows its own way, this class 
//  simply has raw pointers to fox windows. Care must be taken to ensure the 
//  set of windows does not have any dangling pointers.
//
///////////////////////////////////////////////////////////////////////////////

#include "Document.h"
#include "Manager.h"

#include "Usul/Exceptions/Canceled.h"
#include "Usul/Interfaces/IProgressBar.h"
#include "Usul/Interfaces/IStatusBar.h"
#include "Usul/Interfaces/IFlushEvents.h"
#include "Usul/Interfaces/ICancelButton.h"
#include "Usul/Interfaces/ILoadFileDialog.h"
#include "Usul/Interfaces/ISaveFileDialog.h"
#include "Usul/Interfaces/IQuestion.h"
#include "Usul/Interfaces/IHandleActivatingDocument.h"
#include "Usul/Interfaces/IUpdateGUI.h"

#include "Usul/Resources/ProgressBar.h"
#include "Usul/Resources/EventQueue.h"
#include "Usul/Resources/StatusBar.h"
#include "Usul/Resources/TextWindow.h"

#include <sstream>
#include <algorithm>
#include <fstream>
#include <iostream>

using namespace Usul;
using namespace Usul::Documents;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Document, Document::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Document::Document ( const std::string &type ) : BaseClass(), 
  _file      (),
  _windows   (),
  _views     (),
  _active    (),
  _typeName  ( type ),
  _delegate  (),
  _options   ()
{
  // Assign default filename.
  static unsigned int count ( 0 );
  std::ostringstream name;
  name << "Untitled" << ++count;
  this->fileName  ( name.str() );
  this->fileValid ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Document::~Document()
{
  // Note: the windows should already be destroyed, or will be by fox.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a window to the referenced set.
//
///////////////////////////////////////////////////////////////////////////////

void Document::addWindow ( Window *window )
{
  // Changed set to list to get proper order of addition
  if ( std::find_if ( _windows.begin(), _windows.end(), WindowPtr::IsEqual ( window ) ) == _windows.end() )
    _windows.push_back ( window );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a view to the view set.
//
///////////////////////////////////////////////////////////////////////////////

void Document::addView ( View *view )
{
  if ( std::find_if ( _views.begin(), _views.end(), ViewPtr::IsEqual ( view ) ) == _views.end() )
    _views.push_back ( view );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a window from the referenced set.
//
///////////////////////////////////////////////////////////////////////////////

void Document::removeWindow ( Window *window )
{
  _windows.remove ( window );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a view from the view set.
//
///////////////////////////////////////////////////////////////////////////////

void Document::removeView ( View *view )
{
  _views.remove ( view );
  //_options.erase ( view );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Document::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IDocument::IID:
    return static_cast< Usul::Interfaces::IDocument*> ( this );
  case Usul::Interfaces::ISendMessage::IID:
    return static_cast< Usul::Interfaces::ISendMessage* > ( this );
  case Usul::Interfaces::IRead::IID:
    return static_cast< Usul::Interfaces::IRead* > ( this );
  case Usul::Interfaces::IGetTitle::IID:
    return static_cast < Usul::Interfaces::IGetTitle* > ( this );
  case Usul::Interfaces::ICanInsert::IID:
    return static_cast < Usul::Interfaces::ICanInsert* > ( this );
  case Usul::Interfaces::ICanClose::IID:
    return static_cast < Usul::Interfaces::ICanClose* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the file. Clears any data this document already has.
//
///////////////////////////////////////////////////////////////////////////////

void Document::open ( const std::string &file, Usul::Interfaces::IUnknown *caller )
{
  this->clear();
  this->read ( file, caller );
  this->fileName ( file );
  this->fileValid ( true );
  this->modified ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save the document to existing file name.
//
///////////////////////////////////////////////////////////////////////////////

void Document::save ( Usul::Interfaces::IUnknown *caller )
{
  // If the filename is valid...
  if ( this->fileValid() )
    this->_save ( this->fileName(), caller, this->options() );

  // Otherwise...
  else
    this->saveAs ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Always prompts for new file name.
//
///////////////////////////////////////////////////////////////////////////////

void Document::saveAs ( Usul::Interfaces::IUnknown *caller )
{
  // Ask for file name.
  const std::string name ( this->_getSaveAsFileName( this->options(), caller ) );

  // Should have valid name unless user pressed cancel. This behavior is 
  // needed in canClose() and it's consistant with how we cancel elsewhere.
  if ( name.empty() )
    throw Usul::Exceptions::Canceled ( "User cancelled the save-as operation" );

  // Save the document.
  this->_save ( name, caller, this->options() );

  //Since the filename changed, update titles
  this->sendMessage( Document::ID_UPDATE_TITLES );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prompts for the save-as filename. Empty string indicates a cancel.
//
///////////////////////////////////////////////////////////////////////////////

std::string Document::_getSaveAsFileName ( Options &options, Unknown *caller )
{
  // For convenience.
  typedef Usul::Interfaces::ISaveFileDialog FileDialog;
  typedef FileDialog::FileResult Result;

  // Get the file dialog interface
  FileDialog::QueryPtr fileDialog ( caller );

  // Return now if there is no interface
  if( !fileDialog.valid() )
    return "";

  // Ask for file name.
  const std::string title ( ( this->typeName().empty() ) ? "Save" : "Save " + this->typeName() );
  const Result result ( fileDialog->getSaveFileName ( title, this->filtersSave() ) );

  // Get the format user specified.
  std::string filter ( result.second.first );
  std::transform ( filter.begin(), filter.end(), filter.begin(), ::tolower );
  std::string format ( ( std::string::npos != filter.find ( "binary" ) ) ? "binary" : "ascii" );
  
  // Set the format option.
  options["format"] = format;

  // Set the binary flag
  this->binary( "binary" == format );

  // Return the result.
  return result.first;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void Document::_save ( const std::string &name, Unknown *caller, const Options &options )
{
  // Write the document.
  std::cout << "Saving file: " << name << " ... " << Usul::Resources::TextWindow::flush;
  this->write ( name, caller );
  std::cout << "done" << Usul::Resources::TextWindow::endl;

  // Set the file name.
  this->fileName ( name );
  this->fileValid ( true );

  // We are no longer modified.
  this->modified ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prompt user for documents to insert into this one.
//
///////////////////////////////////////////////////////////////////////////////

void Document::insert ( Unknown *caller )
{
  // For convenience.
  typedef Usul::Interfaces::ILoadFileDialog FileDialog;
  typedef FileDialog::FilesResult FilesResult;
  typedef FileDialog::Filenames Filenames;

  // Get the file dialog interface
  FileDialog::QueryPtr fileDialog ( caller );

  // Return now if there is no interface
  if ( !fileDialog.valid() )
    return;

  // Ask for file names.
  const FilesResult result ( fileDialog->getLoadFileNames ( "Insert", this->filtersInsert() ) );
  const Filenames files ( result.first );
  if ( files.empty() )
    return;

  // Set this up here in case we throw below.
  this->modified ( true );

  // Show/hide the progress bar and cancel button.
  Usul::Interfaces::IProgressBar::ShowHide shp  ( caller );
  Usul::Interfaces::ICancelButton::ShowHide shc ( caller );

  // Progress.
  Usul::Interfaces::IProgressBar::UpdateProgressBar progress ( 0, 1, Usul::Resources::progressBar() );

  // Loop through the files.
  for ( Filenames::const_iterator i = files.begin(); i != files.end(); ++i )
  {
    std::cout << "Inserting document: " << *i << Usul::Resources::TextWindow::endl;
    this->read ( *i, caller );

    // Show overall progress.
    progress ( static_cast < unsigned int > ( std::distance ( files.begin(), i ) ), files.size() );

    // Flush events. This lets the user cancel.
    this->flushEvents();
  }

  // Tell everyone to rebuild scene and render.  Scene needs to be rebuilt
  this->sendMessage ( Document::ID_BUILD_SCENE );
  this->sendMessage ( Document::ID_RENDER_SCENE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prompt user for documents to export.
//
///////////////////////////////////////////////////////////////////////////////

void Document::exportDocument ( Unknown *caller )
{
  // For convenience.
  typedef Usul::Interfaces::ISaveFileDialog FileDialog;
  typedef FileDialog::FilesResult Result;
  typedef FileDialog::Filenames Filenames;

  // Get the file dialog interface
  FileDialog::QueryPtr fileDialog ( caller );

  // Return now if there is no interface
  if( !fileDialog.valid() )
    return;

  // Ask for file names.
  const Result result ( fileDialog->getSaveFileNames ( "Export", this->filtersExport() ) );
  const Filenames &files = result.first;

  // Loop through the files.
  for ( Filenames::const_iterator i = files.begin(); i != files.end(); ++i )
    this->write ( *i, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Ask the document if the window can close.
//
///////////////////////////////////////////////////////////////////////////////

bool Document::canClose ( Window *window, Usul::Interfaces::IUnknown *caller, bool checkNumWindows )
{
  // If the given window is not in our set...
  if ( _windows.end() == std::find_if ( _windows.begin(), _windows.end(), WindowPtr::IsEqual( window ) ) )
    return true;

  // If there are no reference windows...
  if ( _windows.empty() )
    return true;

  // If there are more than one window referencing this document...
  if ( checkNumWindows && _windows.size() > 1 )
    return true;

  return this->canClose( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Ask the document if the window can close.
//
///////////////////////////////////////////////////////////////////////////////

bool Document::canClose ( Unknown *caller )
{
  // If the document isn't modified...
  if ( !this->modified() )
    return true;

  // When we get to here, the user has to decide if the document should be saved.
  std::ostringstream message;
  message << "Save document '" << this->fileName() << "' before closing?";
  Usul::Interfaces::IQuestion::QueryPtr question  ( caller );

  // Return true if there is no interface
  if( !question.valid() )
    return true;

  const std::string result ( question->question ( "Yes|No|Cancel", "Save?", message.str() ) );

  // Ok to close window
  if ( "No" == result )
    return true;

  // Not ok to close window
  if ( "Cancel" == result )
    return false;

  // Save document.
  this->save ( caller );

  // If we get down here then the window can close.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close all referenced windows
//
///////////////////////////////////////////////////////////////////////////////

bool Document::closeWindows ( Usul::Interfaces::IUnknown *caller, const Window* skip )
{
  // See if we can close.  Don't check the number of referenced windows
  if ( false == this->canClose ( *_windows.begin(), caller, false ) )
    return false;

  // Make a copy because closing the window invalidates the iterator
  Windows copy ( _windows );

  // Tell the windows to close
  this->_sendMessage ( copy, Document::ID_CLOSE, skip );

  // Let anyone else who may be open that the document is closing
  this->sendMessage( Document::ID_CLOSE );

  // If we get here the windows have closed
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send the message to all listeners except for one specified.
//
///////////////////////////////////////////////////////////////////////////////

void Document::sendMessage ( unsigned short message, const Unknown *skip )
{
  // Send message to both windows and views
  this->_sendMessage ( _windows, message, skip );
  this->_sendMessage ( _views,   message, skip );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to flush events.
//
///////////////////////////////////////////////////////////////////////////////

void Document::flushEvents()
{
  // Give user opportunity to cancel.
  Usul::Interfaces::IFlushEvents::QueryPtr flush ( Usul::Resources::flushEvents() );
  if ( flush.valid() )
    flush->flushEventQueue();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to show progress and flush events.
//
///////////////////////////////////////////////////////////////////////////////

void Document::setProgressBar ( bool state, unsigned int numerator, unsigned int denominator )
{
  // If we should...
  if ( state )
  {
    // Report progress.
    Usul::Interfaces::IProgressBar::QueryPtr progress ( Usul::Resources::progressBar() );
    if ( progress.valid() )
    {
      const float n ( static_cast < float > ( numerator ) );
      const float d ( static_cast < float > ( denominator ) );
      const float fraction ( n / d );
      progress->updateProgressBar ( static_cast < unsigned int > ( fraction * 100 ) );
    }

    // Give user opportunity to cancel.
    this->flushEvents();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to show progress and flush events.
//
///////////////////////////////////////////////////////////////////////////////

void Document::setProgressBar ( bool state, std::istream &in, unsigned int fileSize )
{
  if ( state )
    this->setProgressBar ( state, in.tellg(), fileSize );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to set status bar and flush events.
//
///////////////////////////////////////////////////////////////////////////////

void Document::setStatusBar ( const std::string &text )
{
  Usul::Interfaces::IStatusBar::QueryPtr status ( Usul::Resources::statusBar() );
  if ( status.valid() )
    status->setStatusBarText ( text, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the binary state.
//
///////////////////////////////////////////////////////////////////////////////

bool Document::binary() const
{
  return IDocument::FILE_FORMAT_BINARY == this->fileFormat();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the binary state.
//
///////////////////////////////////////////////////////////////////////////////

void Document::binary ( bool b )
{
  this->fileFormat ( ( b ) ? IDocument::FILE_FORMAT_BINARY : IDocument::FILE_FORMAT_ASCII );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bring this document's windows to the front
//
///////////////////////////////////////////////////////////////////////////////

void Document::windowsForward()
{
  // Loop through the documents in reverse order.  The first window will be on top.
  for ( Windows::reverse_iterator i = _windows.rbegin(); i != _windows.rend(); ++i )
  {
    // Set the focus (brings window to the front).
    (*i)->setFocus();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Build the title for the given Window
//
///////////////////////////////////////////////////////////////////////////////

std::string Document::getTitle ( Window *window )
{
  // String for title
  std::string title ( this->fileName() );

  // See if there is more than one window...
  if ( this->numWindows() > 1 )
  {
    // Find our window.
    typedef Document::WindowConstItr Itr;
    Document::WindowPtr::IsEqual pred ( window );
    Itr where ( std::find_if ( this->beginWindows(), this->endWindows(), pred ) );

    // Build the proper title
    std::ostringstream os;
    os << title << ":" << std::distance < Itr > ( this->beginWindows(), where ) + 1;
    title = os.str();
  }

  // Set the title
  return title;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Notify the document that it is no longer active.
//
///////////////////////////////////////////////////////////////////////////////

void Document::noLongerActive( const std::string &activeType )
{
  Usul::Interfaces::IHandleActivatingDocument::QueryPtr handle ( this->delegate() );

  if( handle.valid() )
    handle->noLongerActive( this, activeType );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the document that it is active
//
///////////////////////////////////////////////////////////////////////////////

void Document::nowActive( const std::string &oldType )
{
  Usul::Interfaces::IHandleActivatingDocument::QueryPtr handle ( this->delegate() );

  if( handle.valid() )
    handle->nowActive( this, oldType );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Given window is closing
//
///////////////////////////////////////////////////////////////////////////////

void Document::closing ( Window *window )
{
  if ( _windows.empty() )
  {
    // Tell the manager we are closing.
    Manager::instance().close ( this );

    // Clear the data.
    std::cout << "Closing document: " << this->fileName() << " ... " << Usul::Resources::TextWindow::flush;
    this->clear ( Usul::Resources::progressBar() );
    std::cout << "done" << Usul::Resources::TextWindow::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify this document of the message.
//
///////////////////////////////////////////////////////////////////////////////

void Document::notify ( unsigned short message )
{
  // Default implementation passes the message along to the windows.
  this->sendMessage ( message );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Delegate.  May be null.
//
///////////////////////////////////////////////////////////////////////////////

void Document::delegate ( Delegate *delegate )
{
  _delegate = delegate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Delegate.  May be null.
//
///////////////////////////////////////////////////////////////////////////////

Document::Delegate* Document::delegate()
{
  return _delegate.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the default GUI.
//
///////////////////////////////////////////////////////////////////////////////

void Document::createDefaultGUI ( Unknown *caller )
{
  if( this->delegate() )
    this->delegate()->createDefaultGUI( this, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Refresh the view
//
///////////////////////////////////////////////////////////////////////////////

void Document::refreshView ( Usul::Interfaces::IViewer *viewer )
{
  if ( this->delegate() )
    this->delegate()->refreshView ( this, viewer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the active view.
//
///////////////////////////////////////////////////////////////////////////////

void  Document::activeView  ( View *view )
{
  _active = view;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the active view.
//
///////////////////////////////////////////////////////////////////////////////

Document::View* Document::activeView  (  ) const
{
  return _active.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the GUI
//
///////////////////////////////////////////////////////////////////////////////

void Document::updateGUI  (  ) 
{
  Usul::Interfaces::IUpdateGUI::QueryPtr update  ( this->delegate( ) );

  if( update.valid() )
    update->updateGUI();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the options.
//
///////////////////////////////////////////////////////////////////////////////

Document::Options& Document::options()
{
  //return _options[this->activeView()];
  return _options;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the options.
//
///////////////////////////////////////////////////////////////////////////////

const Document::Options& Document::options() const
{
  //return _options[this->activeView()];
  return _options;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the options.
//
///////////////////////////////////////////////////////////////////////////////

Document::Options& Document::options( View* view )
{
  //return _options[view];
  return _options;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the options.
//
///////////////////////////////////////////////////////////////////////////////

const Document::Options& Document::options( View* view ) const
{
  //return _options[view];
  return _options;
}

