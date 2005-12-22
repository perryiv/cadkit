
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The main window class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "MainWindow.h"
#include "Application.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/System/Screen.h"

IMPLEMENT_CLASS ( MainWindow, MainWindow::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Event table.
//
///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE ( MainWindow, MainWindow::BaseClass )
    EVT_CLOSE ( MainWindow::_onCloseMainWindow )
END_EVENT_TABLE()


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow() : BaseClass ( 0x0, wxID_ANY, "Code Maker",
  wxPoint ( Usul::System::Screen::width() * 0.1, Usul::System::Screen::height() * 0.1 ),
  wxSize  ( Usul::System::Screen::width() * 0.8, Usul::System::Screen::height() * 0.8 ) ),
  _leftPanel ( 0x0 )
{
  // One big split window.
  wxSplitterWindow *splitter ( new wxSplitterWindow ( this, wxID_ANY ) );
  splitter->SetSashGravity ( 0.5 );

  // Load left panel.
  const std::string leftPanelFile ( Usul::CommandLine::Arguments::instance().directory() + "/LeftPanel.xrc" );
  wxXmlResource::Get()->Load ( leftPanelFile.c_str() );
  _leftPanel = wxXmlResource::Get()->LoadPanel ( splitter, "ID_LEFT_PANEL" );
  if ( 0x0 == _leftPanel )
    throw std::runtime_error ( "Error: left panel not loaded from file: " + leftPanelFile );

  // Find browse button and attach event handler.
  wxButton *browseButton ( wxDynamicCast ( _leftPanel->FindWindow ( "ID_DIRECTORY_BROWSE_BUTTON" ), wxButton ) );
  if ( browseButton )
    this->Connect ( wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler ( MainWindow::_onBrowseForDirectory ) );

  // Make right panel.
  wxPanel *rightPanel ( new wxPanel ( splitter, wxID_ANY ) );

  // Tell splitter what to split.
  splitter->SplitVertically ( _leftPanel, rightPanel, 0 );

  // Right half.
  wxTextCtrl *rightText ( new wxTextCtrl ( rightPanel, wxID_ANY, "Right Text" ) );
  wxSizer *rightSizer = new wxBoxSizer ( wxHORIZONTAL );
  rightSizer->Add ( rightText, 1, wxGROW | wxALL, 0 );
  rightPanel->SetSizer ( rightSizer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the browse button is pressed.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_onBrowseForDirectory ( wxCommandEvent &event )
{
  // Get text control.
  wxTextCtrl *textControl ( wxDynamicCast ( _leftPanel->FindWindow ( "ID_DIRECTORY_TEXT_CONTROL" ), wxTextCtrl ) );
  if ( 0x0 == textControl )
    return;

  // Ask for a directory name.
  wxString dir ( wxDirSelector ( "Choose a Directory", textControl->GetValue() ) );
  if ( dir.IsEmpty() )
    return;

  // Set text.
  textControl->SetValue ( dir );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the frame is about to close.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_onCloseMainWindow ( wxCloseEvent &event )
{
  // Pass the event along.
  event.Skip();
}
