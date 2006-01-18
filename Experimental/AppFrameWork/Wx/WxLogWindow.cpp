
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
#include "WxEventHandler.h"

#include "Usul/Errors/Assert.h"
#include "Usul/Strings/Case.h"

AFW_IMPLEMENT_OBJECT ( WxLogWindow );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WxLogWindow::WxLogWindow() : BaseClass(),
  _error   ( "afw_error_16x16"   ),
  _info    ( "afw_info_16x16"    ),
  _warning ( "afw_warning_16x16" )
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

bool WxLogWindow::create ( AFW::Windows::Window *w )
{
  Guard guard ( this->mutex() );

  // Get the window.
  wxWindow *window ( WxObjectMap::find<wxFrame> ( w ) );
  if ( 0x0 == window )
    return false;

  // Don't create twice.
  if ( 0x0 != this->get() )
    return false;

  // Make an html window.
  std::auto_ptr<wxHtmlWindow> html ( new wxHtmlWindow ( window, wxID_ANY ) );

  // Add another event handler.
  html->PushEventHandler ( new WxEventHandler );

  // Set our window in the map.
  WxObjectMap::set ( this, html.release() );

  // Make menu button if applicable.
  this->_makeRequestedControls();

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
  this->_update();
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
  this->_update();
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
  this->_update();
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
  this->_update();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the text.
//
///////////////////////////////////////////////////////////////////////////////

void WxLogWindow::_update()
{
  Guard guard ( this->mutex() );

  // Get our window.
  wxHtmlWindow *window ( WxObjectMap::find<wxHtmlWindow> ( this ) );
  if ( 0x0 == window )
    return;

  // Get internal string.
  std::string internal;
  this->textGet ( internal );

  // The prefix and suffix for the page.
  const std::string prefix ( "<html><head></head><body><table width=\"100%\"border=\"0\" cellpadding=\"5\" cellspacing=\"0\">" );
  const std::string suffix ( "</table></body></html>" );

  // The two colors.
  typedef std::pair < std::string, std::string > Colors;
  const Colors colors ( "E0E0E0", "FFFFFF" );

  // Build the html string.
  std::string html;
  html.reserve ( prefix.size() + 2 * internal.size() + suffix.size() );

  // Start the page.
  html += prefix;

  // Initialize the counter.
  unsigned int count ( 0 );

  // For each line...
  std::string::const_iterator start ( internal.begin() );
  std::string::const_iterator end   ( internal.end()   );
  while ( start < end )
  {
    // Find the end of the line.
    std::string::const_iterator newline ( std::find ( start, end, '\n' ) );

    // The line to add.
    const std::string line ( start, newline );

    // Determine the color.
    const std::string color ( ( 0 == count % 2 ) ? colors.first : colors.second );
    ++count;

    // String to begin a column.
    const std::string beginCol ( "<td valign=\"top\" bgcolor=\"#" + color + "\">" );

    {
      // Start of the row.
      html += "<tr>";

      // Add the first column.
      html += ( beginCol + "<img src=\"" + this->_getImage ( line ) + "\"/>" + "</td>" );

      // Add the second column.
      html += ( beginCol + line + "</td>" );

      // End of the row.
      html += "</tr>";

      // Add a horizontal line.
      //html += "<tr><td><hr/></td><td><hr/></td></tr>";
    }

    // Update the starting position.
    start = newline + 1;
  }

  // Finish the page.
  html += suffix;

  // Set the page.
  window->SetPage ( wxString ( html.c_str() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the appropriate image.
//
///////////////////////////////////////////////////////////////////////////////

std::string WxLogWindow::_getImage ( const std::string &line ) const
{
  // Make it all lower-case.
  std::string copy ( Usul::Strings::lowerCase ( line ) );

  // What we are looking for.
  const std::string warning     ( "warning" );
  const std::string error       ( "error" );
  const std::string systemError ( "system error" );

  // If the string is not long enough.
  if ( line.empty() )
    return _info.file();

  // Is it a system error?
  const bool isSystemError ( 0 == copy.compare ( 0, systemError.size(), systemError ) );
  if ( isSystemError )
    return _error.file();

  // Is it an error?
  const bool isError ( 0 == copy.compare ( 0, error.size(), error ) );
  if ( isError )
    return _error.file();

  // Is it a warning?
  const bool isWarning ( 0 == copy.compare ( 0, warning.size(), warning ) );
  if ( isWarning )
    return _warning.file();

  // Default to information.
  return _info.file();
}
