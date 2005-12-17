
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
    EVT_CLOSE ( MainWindow::_onClose )
END_EVENT_TABLE()


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow() : BaseClass ( 0x0, wxID_ANY, "Code Maker",
  wxPoint ( Usul::System::Screen::width() * 0.1, Usul::System::Screen::height() * 0.1 ),
  wxSize  ( Usul::System::Screen::width() * 0.8, Usul::System::Screen::height() * 0.8 ) )
{
  // One big split window.
  wxSplitterWindow *splitter ( new wxSplitterWindow ( this, wxID_ANY ) );
  splitter->SetSashGravity ( 0.5 );

  // Load left panel.
  const std::string leftPanelFile ( Usul::CommandLine::Arguments::instance().directory() + "/LeftPanel.xrc" );
  wxXmlResource::Get()->Load ( leftPanelFile.c_str() );
  wxPanel *leftPanel ( wxXmlResource::Get()->LoadPanel ( splitter, "ID_LEFT_PANEL" ) );

  // Make right panel.
  wxPanel *rightPanel ( new wxPanel ( splitter, wxID_ANY ) );

  // Tell splitter what to split.
  splitter->SplitVertically ( leftPanel, rightPanel, 0 );

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
//  Called when the frame is about to close.
//
///////////////////////////////////////////////////////////////////////////////

void MainWindow::_onClose ( wxCloseEvent &event )
{
  // Pass the event along.
  event.Skip();
}
