
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Bitmap class.
//
///////////////////////////////////////////////////////////////////////////////

#include "WxPrecompiled.h"
#include "WxBitmap.h"
#include "WxImage.h"

#include "Usul/Errors/Stack.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Load the bitmap.
//
///////////////////////////////////////////////////////////////////////////////

wxBitmap WxBitmap::load ( const std::string &file, bool report )
{
  // I think this is the most robust path.
  wxImage image ( WxImage::load ( file.c_str(), report ) );
  if ( false == image.Ok() )
    return wxBitmap();

  // Make bitmap.
  wxBitmap bitmap ( image );

  // Did it work?
  if ( false == bitmap.Ok() && true == report )
    Usul::Errors::Stack::instance().push ( "Warning 2631623776: Failed to load bitmap from file: " + file );

  // Return the bitmap.
  return bitmap;
}
