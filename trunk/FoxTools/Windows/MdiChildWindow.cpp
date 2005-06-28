
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Child window.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Windows/MdiChildWindow.h"
#include "FoxTools/App/Application.h"
#include "FoxTools/Adaptors/Wait.h"
#include "FoxTools/Headers/MenuPane.h"
#include "FoxTools/Headers/MainWindow.h"
#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Dialogs/Message.h"

#include "Usul/Interfaces/IReportErrors.h"
#include "Usul/Interfaces/IStatusBar.h"
#include "Usul/Interfaces/IProgressBar.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IRead.h"
#include "Usul/Interfaces/ICanClose.h"
#include "Usul/Interfaces/ICanInsert.h"
#include "Usul/Interfaces/IGetTitle.h"

#include "Usul/Errors/Checker.h"
#include "Usul/File/Path.h"
#include "Usul/Cast/Cast.h"

#include "Usul/Resources/ReportErrors.h"
#include "Usul/Resources/StatusBar.h"
#include "Usul/Resources/ProgressBar.h"

#ifdef _MSC_VER                     // VC++
# define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX                   // Do not define min and max as macros.
# include <windows.h>               // Base Windows header file.
# include <shellapi.h>              // For DragAcceptFiles().
#endif // _MSC_VER

using namespace FoxTools::Windows;


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

// This class, base class, array of message handlers, size of the array.
FXDEFMAP ( MdiChildWindow ) WindowMap[] = 
{
  // Message Type              ID                                                         Message Handler.
  FXMAPFUNC ( FX::SEL_FOCUSIN, 0,                                                         MdiChildWindow::onFocusIn               ),
  FXMAPFUNC ( FX::SEL_MOTION,  0,                                                         MdiChildWindow::onMotion                ),
  FXMAPFUNC ( FX::SEL_CLOSE,   0,                                                         MdiChildWindow::onCommandClose          ),
  FXMAPFUNC ( FX::SEL_COMMAND, FXMDIChild::ID_MDI_CLOSE,                                  MdiChildWindow::onCommandClose          ),
  FXMAPFUNC ( FX::SEL_COMMAND, FXMDIChild::ID_MDI_MENUCLOSE,                              MdiChildWindow::onCommandClose          ),
  FXMAPFUNC ( FX::SEL_COMMAND, FoxTools::App::Application::ID_DROP_FILE,                  MdiChildWindow::onCommandDropFile       ),
  FXMAPFUNC ( FX::SEL_COMMAND, Usul::Interfaces::ISendMessage::ID_CLOSE,                  MdiChildWindow::onCommandDocumentClose  ),
  FXMAPFUNC ( FX::SEL_COMMAND, Usul::Interfaces::ISendMessage::ID_UPDATE_TITLES,          MdiChildWindow::onCommandUpdateTitle    ),
  FXMAPFUNC ( FX::SEL_COMMAND, Usul::Interfaces::ISendMessage::ID_CLEAR_SCENE,            MdiChildWindow::onClearScene            ),
  FXMAPFUNC ( FX::SEL_COMMAND, Usul::Interfaces::ISendMessage::ID_BUILD_SCENE,            MdiChildWindow::onBuildScene            ),
  FXMAPFUNC ( FX::SEL_COMMAND, Usul::Interfaces::ISendMessage::ID_DISPLAY_LISTS_UPDATE,   MdiChildWindow::onDisplayListUpdate     ),
};

FXIMPLEMENT ( MdiChildWindow, MdiChildWindow::BaseClass, WindowMap, ARRAYNUMBER ( WindowMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MdiChildWindow::MdiChildWindow() : BaseClass(),
  _document(), // This valid-ref-pointer should throw.
  _view(),
  _refCount( 0 )
{
  // If this static-assertion fails then make Document::ID_FIRST bigger.
  USUL_STATIC_ASSERT ( Usul::Interfaces::ISendMessage::ID_FIRST > MdiChildWindow::ID_LAST );

  // Do not use this constructor.
  USUL_ERROR_CHECKER ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MdiChildWindow::MdiChildWindow ( Document *doc,
                                 FX::FXMDIClient *parent,
                                 const std::string &name,
                                 FX::FXIcon *icon,
                                 FX::FXMenuPane *menu,
                                 FX::FXuint options ) : 
  BaseClass ( parent, name.c_str(), icon, menu, options, 0, 0, 300, 200 ),
  _document ( doc ),
  _view(),
  _refCount( 0 )
{
  // Should be true.
  USUL_ERROR_CHECKER ( 0x0 != this->getApp() );

  // Initialize the placement.
  this->_initPlacement();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MdiChildWindow::~MdiChildWindow()
{
  // Remove this window from the document's sets.
  this->document()->removeWindow   ( this );

  // Update all child window's titles
  Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( this->document() );
  sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_UPDATE_TITLES );

  // Tell the document this is closing.  
  // Make sure function is called after removeWindow is called.
  this->document()->closing( this );

  // Better be zero
  USUL_ASSERT ( 0 == _refCount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create.
//
///////////////////////////////////////////////////////////////////////////////

void MdiChildWindow::create()
{
  // Should be true.
  USUL_ERROR_CHECKER ( 0x0 != this->getApp() );
  USUL_ERROR_CHECKER ( 0x0 != this->getParent()->id() );

  // Call the base class's function.
  BaseClass::create();
  
  // Add this to the document
  this->document()->addWindow   ( this );

  // Update titles.
  Usul::Interfaces::ISendMessage::ValidQueryPtr message ( this->document() );
  message->sendMessage ( Usul::Interfaces::ISendMessage::ID_UPDATE_TITLES );

  // Enable windows drag-and-drop.
  #ifdef _WIN32
  ::DragAcceptFiles ( reinterpret_cast < HWND > ( this->id() ), TRUE );
  #endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is it created?
//
///////////////////////////////////////////////////////////////////////////////

bool MdiChildWindow::isCreated() const
{
  return ( 0x0 != this->id() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the placement. This gives the nice cascading behavior.
//
///////////////////////////////////////////////////////////////////////////////

void MdiChildWindow::_initPlacement()
{
  // Shortcuts.
  double percent ( 0.6 );
  int width  ( this->getParent()->getWidth() );
  int height ( this->getParent()->getHeight() );
  int w ( static_cast <int> ( percent * width ) );
  int h ( static_cast <int> ( percent * height ) );

  // Declare the placement static.
  static int x ( 0 );
  static int y ( 0 );

  // Set the placement.
  this->setX      ( x );
  this->setY      ( y );
  this->setWidth  ( w );
  this->setHeight ( h );

  // Increment.
  x += 20;
  y += 20;

  // If we are too far then start over.
  if ( ( x + w ) > width || ( y + h ) > height )
  {
    x = 0;
    y = 0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Report any errors.
//
///////////////////////////////////////////////////////////////////////////////

void MdiChildWindow::_reportErrors ( unsigned int options, bool clear )
{ 
  Usul::Interfaces::IReportErrors::QueryPtr report ( Usul::Resources::reportErrors() );
  if( report.valid() )
    report->reportErrors ( options, clear );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Window is being closed.
//
///////////////////////////////////////////////////////////////////////////////

long MdiChildWindow::onCommandClose ( FXObject *object, FXSelector id, void *data )
{
  // Get Interface
  Usul::Interfaces::ICanClose::ValidQueryPtr close ( this->document() );
  
  // Ask the document if we can close.
  if ( false == close->canClose ( this, Usul::Interfaces::IUnknown::ValidQueryPtr ( this ) ) )
    return 0;

  // Show/hide the progress bar.
  Usul::Interfaces::IProgressBar::ShowHide progress ( Usul::Resources::progressBar() );

  // Call the base class's function.
  return BaseClass::onCmdClose ( object, id, data );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar text.
//
///////////////////////////////////////////////////////////////////////////////

void MdiChildWindow::setStatusBarText ( const std::string &text, bool force )
{
  Usul::Interfaces::IStatusBar::QueryPtr status ( Usul::Resources::statusBar() );
  if( status.valid() )
    status->setStatusBarText ( text, force );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

const MdiChildWindow::Document *MdiChildWindow::document() const
{
  return _document.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

MdiChildWindow::Document *MdiChildWindow::document()
{
  return _document.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* MdiChildWindow::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IView::IID:
    return static_cast < Usul::Interfaces::IView* > ( this );
  case Usul::Interfaces::IWindow::IID:
    return static_cast < Usul::Interfaces::IWindow* > ( this );
  case Usul::Interfaces::IQuestion::IID:
    return static_cast < Usul::Interfaces::IQuestion* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference this instance. Needed because we inherits from IUnknown.
//
///////////////////////////////////////////////////////////////////////////////

void MdiChildWindow::ref()
{
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference this instance. Needed because we inherits from IUnknown.
//
///////////////////////////////////////////////////////////////////////////////

void MdiChildWindow::unref ( bool allowDeletion )
{
  --_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open dropped file.
//
///////////////////////////////////////////////////////////////////////////////

long MdiChildWindow::onCommandDropFile ( FX::FXObject *, FX::FXSelector, void *ptr )
{
  // The wait-cursor.
  FoxTools::Adaptors::Wait wait;

  // Have the document read this file if it can.
  std::string filename ( ( ptr ) ? USUL_UNSAFE_CAST ( FX::FXchar *, ptr ) : "" );

  Usul::Interfaces::ICanInsert::QueryPtr insert ( this->document() );

  if ( insert.valid() && insert->canInsert ( filename ) )
  {
    Usul::Interfaces::IRead::ValidQueryPtr read ( this->document() );
    read->read ( filename, Usul::Interfaces::IUnknown::ValidQueryPtr ( this ) );

    Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( this->document() );
    sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
    sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
  }

  // Otherwise, some user feedback...
  else
  {
    // Perhaps ideally, the cursor would change if this window does not 
    // contain a document that can read the file(s) that are about to be 
    // dropped. However, there may be more than one type of file, some of them
    // readable and some not. Popping up a message dialog for each file that 
    // cannot be read would (I think) be rather annoying.
    std::ostringstream message;
    message << "Document type '" 
            << this->document()->typeName() 
            << "' does not read files with extension '" 
            << Usul::File::extension ( filename );
    this->setStatusBarText ( message.str(), true );
  }

  // Handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Document is closing, close self
//
///////////////////////////////////////////////////////////////////////////////

long MdiChildWindow::onCommandDocumentClose ( FX::FXObject *object, FX::FXSelector sel, void *data )
{
  //Redirect to base class' close
  return BaseClass::onCmdClose( object, sel, data );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the title based on the number of windows in the document
//
///////////////////////////////////////////////////////////////////////////////

long MdiChildWindow::onCommandUpdateTitle ( FX::FXObject *, FX::FXSelector, void * )
{
  // Get the interface
  Usul::Interfaces::IGetTitle::QueryPtr getTitle ( this->document() );

  // String for title
  std::string title ( getTitle.valid() ? getTitle->getTitle( this ) : "" );

  // Set the title
  this->setTitle ( title.c_str() );

  // Message handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Focus is now on this window
//
///////////////////////////////////////////////////////////////////////////////

long MdiChildWindow::onFocusIn ( FX::FXObject *object, FX::FXSelector sel, void *data )
{
  // Call the base class first
  BaseClass::onFocusIn ( object, sel, data );

  // Tell the document that it is active
  // Might need a way to get to the document manager through an interface 
  // to tell it that our document is now active
  this->document()->active();

  //Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the view
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IViewer* MdiChildWindow::view()
{
  return _view.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the view
//
///////////////////////////////////////////////////////////////////////////////

void MdiChildWindow::view ( Usul::Interfaces::IViewer *viewer )
{
  _view = viewer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the mouse moves over the window.
//
///////////////////////////////////////////////////////////////////////////////

long MdiChildWindow::onMotion ( FX::FXObject *object, FX::FXSelector selector, void *callData )
{
  // Have to call this first to get the proper cursor.
  long result ( BaseClass::onMotion ( object, selector, callData ) );

  // If we are grabbed then pipe the size of the active view to the status bar.
  if ( this->grabbed() )
  {
    int x ( this->newx + _view->x() );
    int y ( this->newy + _view->y() );
    int w ( this->neww - ( this->getWidth()  - _view->width()  ) );
    int h ( this->newh - ( this->getHeight() - _view->height() ) );
    std::ostringstream out;
    out << "  x = " << x << ", y = " << y << ", w = " << w << ", h = " << h;
    this->setStatusBarText ( out.str(), false );
  }

  // Return the result.
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called to rebuild the scene.
//
///////////////////////////////////////////////////////////////////////////////

long MdiChildWindow::onBuildScene ( FX::FXObject *, FX::FXSelector, void * )
{
  this->document()->refreshView ( _view );

  // Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called to clear the scene.
//
///////////////////////////////////////////////////////////////////////////////

long MdiChildWindow::onClearScene ( FX::FXObject *, FX::FXSelector, void * )
{
  _view->clear();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Ignore this message.
//
///////////////////////////////////////////////////////////////////////////////

long MdiChildWindow::onDisplayListUpdate ( FX::FXObject *, FX::FXSelector, void * )
{
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the focus to this window.
//
///////////////////////////////////////////////////////////////////////////////

void MdiChildWindow::setFocus()
{
  BaseClass::setFocus();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle the message.
//
///////////////////////////////////////////////////////////////////////////////

void MdiChildWindow::handleMessage ( unsigned short message )
{
  this->handle ( 0x0, FXSEL ( FX::SEL_COMMAND, message ), 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prompt a question dialog to the user
//
///////////////////////////////////////////////////////////////////////////////

std::string MdiChildWindow::question ( const std::string &buttons,  const std::string &title, const std::string &text )
{
  return FoxTools::Dialogs::Message::question( buttons, title, text );
}

