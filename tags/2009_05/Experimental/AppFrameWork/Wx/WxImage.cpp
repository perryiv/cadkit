
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Image class.
//
///////////////////////////////////////////////////////////////////////////////

#include "WxPrecompiled.h"
#include "WxImage.h"

#include "Usul/Errors/Stack.h"
#include "Usul/Predicates/FileExists.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Load the image.
//
///////////////////////////////////////////////////////////////////////////////

wxImage WxImage::load ( const std::string &file, bool report )
{
  // Make sure the file exists, or wxWindows may pop up a dialog.
  const bool exists ( Usul::Predicates::FileExists() ( file ) );

  // Return default image if the file does not exist.
  if ( false == exists )
  {
    // Report errors if we should.
    if ( true == report )
      Usul::Errors::Stack::instance().push ( "Warning 4204040893: Image file does not exist: " + file );

    // Return default image.
    return wxImage();
  }

  // Load the image.
  wxImage image ( wxString ( file.c_str() ) );

  // Did it work?
  if ( false == image.Ok() && true == report )
    Usul::Errors::Stack::instance().push ( "Warning 1103027236: Failed to load image from file: " + file );

  // Return the image.
  return image;
}
