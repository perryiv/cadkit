
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Icon class.
//
///////////////////////////////////////////////////////////////////////////////

#include "WxPrecompiled.h"
#include "WxIcon.h"
#include "WxBitmap.h"

#include "AppFrameWork/Windows/Window.h"
#include "AppFrameWork/Core/Icon.h"

#include "Usul/Errors/Stack.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Load the icon.
//
///////////////////////////////////////////////////////////////////////////////

wxIcon WxIcon::load ( const AFW::Windows::Window *window, bool report )
{
  return ( ( window ) ? WxIcon::load ( window->icon(), report ) : wxIcon() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the icon.
//
///////////////////////////////////////////////////////////////////////////////

wxIcon WxIcon::load ( const AFW::Core::Icon &icon, bool report )
{
  return WxIcon::load ( icon.file(), report );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the icon.
//
///////////////////////////////////////////////////////////////////////////////

wxIcon WxIcon::load ( const std::string &file, bool report )
{
  // I think this is the most robust path.
  wxBitmap bitmap ( WxBitmap::load ( file.c_str(), report ) );
  if ( false == bitmap.Ok() )
    return wxIcon();

  // Make icon.
  wxIcon icon;
  icon.CopyFromBitmap ( bitmap );

  // Did it work?
  if ( false == icon.Ok() && true == report )
    Usul::Errors::Stack::instance().push ( "Warning 2397921922: Failed to load icon from file: " + file );

  // Return the icon.
  return icon;
}
