
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows log window class.
//
///////////////////////////////////////////////////////////////////////////////

#include "WxPrecompiled.h"
#include "WxLogWindow.h"
#include "WxObjectMap.h"
#include "WxMainWindow.h"

#include "Usul/Errors/Assert.h"

AFW_IMPLEMENT_OBJECT ( WxLogWindow );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WxLogWindow::WxLogWindow() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WxLogWindow::~WxLogWindow()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the existing status bar.
//
///////////////////////////////////////////////////////////////////////////////

wxPanel *WxLogWindow::get()
{
  Guard guard ( this->mutex() );
  return ( WxObjectMap::find<wxPanel> ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the existing status bar.
//
///////////////////////////////////////////////////////////////////////////////

const wxPanel *WxLogWindow::get() const
{
  Guard guard ( this->mutex() );
  return ( WxObjectMap::find<wxPanel> ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Detach from gui object.
//
///////////////////////////////////////////////////////////////////////////////

void WxLogWindow::detach()
{
  WxObjectMap::remove ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the text window.
//
///////////////////////////////////////////////////////////////////////////////

bool WxLogWindow::create ( AFW::Core::Window *w )
{
  Guard guard ( this->mutex() );

  // Get the window.
  wxWindow *window ( WxObjectMap::find<wxFrame> ( w ) );
  if ( 0x0 == window )
    return false;

  // Don't create twice.
  if ( 0x0 != this->get() )
    return false;

  // Make a panel.
  std::auto_ptr<wxPanel> panel ( new wxPanel ( window, wxID_ANY ) );

  // Set our window in the map.
  WxObjectMap::set ( this, panel.release() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text.
//
///////////////////////////////////////////////////////////////////////////////

void WxLogWindow::textSet ( const std::string &t )
{
  Guard guard ( this->mutex() );
  BaseClass::textSet ( t );
  this->_textSet ( t.c_str(), t.length() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text.
//
///////////////////////////////////////////////////////////////////////////////

void WxLogWindow::textSet ( const char *t, unsigned int length )
{
  Guard guard ( this->mutex() );
  BaseClass::textSet ( t, length );
  this->_textSet ( t, length );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the text.
//
///////////////////////////////////////////////////////////////////////////////

void WxLogWindow::textAppend ( const std::string &t )
{
  Guard guard ( this->mutex() );
  BaseClass::textAppend ( t );
  this->_textAppend ( t.c_str(), t.length() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the text.
//
///////////////////////////////////////////////////////////////////////////////

void WxLogWindow::textAppend ( const char *t, unsigned int length )
{
  Guard guard ( this->mutex() );
  BaseClass::textAppend ( t );
  this->_textAppend ( t, length );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append to the existing text.
//
///////////////////////////////////////////////////////////////////////////////

void WxLogWindow::_textAppend ( const char *text, unsigned int length )
{
  Guard guard ( this->mutex() );
  if ( 0x0 != text && length > 0 )
  {
    // Get the panel.
    wxPanel *panel ( WxObjectMap::find<wxPanel> ( this ) );
    if ( panel )
    {
      // If there is no sizer then make one.
      if ( 0x0 == panel->GetSizer() )
        panel->SetSizer ( new wxBoxSizer ( wxVERTICAL ) );

      // Get the sizer.
      wxSizer *sizer ( panel->GetSizer() );

      // For each line...
      const char *start ( text );
      const char *end ( start + length );
      while ( start < end )
      {
        // Find the end of the line.
        const char *newline ( std::find ( start, end, '\n' ) );

        // Make a sizer.
        std::auto_ptr<wxBoxSizer> row ( new wxBoxSizer ( wxHORIZONTAL ) );

        // Add an icon.
        wxBitmap bitmap ( wxArtProvider::GetBitmap ( wxART_INFORMATION, wxART_MESSAGE_BOX ) );
        std::auto_ptr<wxStaticBitmap> sb ( new wxStaticBitmap ( panel, wxID_ANY, bitmap ) );
        sb->SetSizer ( row.get() );

        // The panel will delete.
        sb.release();

        // Add it to the parent sizer.
        sizer->Add ( row.release() );

        // Update the starting position.
        start = newline + 1;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text.
//
///////////////////////////////////////////////////////////////////////////////

void WxLogWindow::_textSet ( const char *text, unsigned int length )
{
  Guard guard ( this->mutex() );
  if ( 0x0 != text && length > 0 )
  {
  }
}
