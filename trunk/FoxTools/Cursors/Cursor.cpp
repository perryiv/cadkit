
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Cursor class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Cursor.h"
#include "CursorArrays.h"
#include "OsgFox/OsgFox/ErrorChecker.h"

#include "fx.h" //TODO include only needed headers

using namespace FoxTools;
using namespace FoxTools::Cursors;


///////////////////////////////////////////////////////////////////////////////
//
//  Set the standard cursor.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXCursor *Cursor::create ( Type type, bool createIt, FX::FXApp *app )
{
  OSG_FOX_ERROR_CHECKER ( 0x0 != app );

  // Initialize the array pointers.
  const unsigned char *source = 0x0;
  const unsigned char *mask = 0x0;

  // Initialize these also.
  int width = 32;
  int height = 32;
  int hotSpotX = 12;
  int hotSpotY = 11;
  Format format = CURSOR_XBM;

  // Declare the new icon.
  switch ( type )
  {
    case Cursor::CURSOR_ARROW:
    {
      // Make the new cursor.
      std::auto_ptr<FX::FXCursor> cursor ( new FX::FXCursor ( app, FX::CURSOR_ARROW ) );
      OSG_FOX_ERROR_CHECKER ( 0x0 != cursor.get() );

      // Create the cursor if we're supposed to.
      if ( createIt ) 
        cursor->create();

      // Return the new cursor.
      return cursor.release();
    }

    case Cursor::CURSOR_ROTATE:
    
      source = _rotateCursorArrayXBM;
      mask = _rotateCursorArrayXBM;
      width = _rotateCursorWidth;
      height = _rotateCursorHeight;
      hotSpotX = _rotateCursorHotSpotX;
      hotSpotY = _rotateCursorHotSpotY;
      format = CURSOR_XBM;
      break;

    case Cursor::CURSOR_TRANSLATE: 
      
      source = _translateCursorArrayXBM;
      mask = _translateCursorArrayXBM;
      width = _translateCursorWidth;
      height = _translateCursorHeight;
      hotSpotX = _translateCursorHotSpotX;
      hotSpotY = _translateCursorHotSpotY;
      format = CURSOR_XBM;
      break;

    case Cursor::CURSOR_SCALE:
      
      source = _scaleCursorArrayXBM;
      mask = _scaleCursorArrayXBM;
      width = _scaleCursorWidth;
      height = _scaleCursorHeight;
      hotSpotX = _scaleCursorHotSpotX;
      hotSpotY = _scaleCursorHotSpotY;
      format = CURSOR_XBM;
      break;

    case Cursor::CURSOR_SEEK:
      
      source = _seekCursorArrayXBM;
      mask = _seekCursorArrayXBM;
      width = _seekCursorWidth;
      height = _seekCursorHeight;
      hotSpotX = _seekCursorHotSpotX;
      hotSpotY = _seekCursorHotSpotY;
      format = CURSOR_XBM;
      break;

    case Cursor::CURSOR_HYPERLINK:
      
      source = _hyperlinkCursorArrayCUR;
      mask = _hyperlinkCursorArrayCUR;
      width = _hyperlinkCursorWidth;
      height = _hyperlinkCursorHeight;
      hotSpotX = _hyperlinkCursorHotSpotX;
      hotSpotY = _hyperlinkCursorHotSpotY;
      format = CURSOR_CUR;
      break;

    case Cursor::CURSOR_SELECTION:

      source = _selectionCursorArrayCUR;
      mask = _selectionCursorArrayCUR;
      width = _selectionCursorWidth;
      height = _selectionCursorHeight;
      hotSpotX = _selectionCursorHotSpotX;
      hotSpotY = _selectionCursorHotSpotY;
      format = CURSOR_CUR;
      break;

    case Cursor::CURSOR_GUN_SIGHT:

      source = _gunSightCursorArrayCUR;
      mask = _gunSightCursorArrayCUR;
      width = _gunSightCursorWidth;
      height = _gunSightCursorHeight;
      hotSpotX = _gunSightCursorHotSpotX;
      hotSpotY = _gunSightCursorHotSpotY;
      format = CURSOR_CUR;
      break;

    default:

      OSG_FOX_ERROR_CHECKER ( false );
      return 0x0;
  }

  // Should be true.
  OSG_FOX_ERROR_CHECKER ( 0x0 != source );
  OSG_FOX_ERROR_CHECKER ( 0x0 != mask );

  // Call the other one.
  return Cursor::create ( source, mask, width, height, hotSpotX, hotSpotY, format, createIt, app );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the cursor from the given array.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXCursor *Cursor::create ( 
  const unsigned char *source, 
  const unsigned char *mask, 
  int width, 
  int height, 
  int hotSpotX, 
  int hotSpotY, 
  Format format,
  bool createIt,
  FX::FXApp *app )
{
  OSG_FOX_ERROR_CHECKER ( 0x0 != app );
  OSG_FOX_ERROR_CHECKER ( 0x0 != source );
  OSG_FOX_ERROR_CHECKER ( 0x0 != mask );
  OSG_FOX_ERROR_CHECKER ( width > 0 && width <= 32 );
  OSG_FOX_ERROR_CHECKER ( height > 0 && height <= 32 );
  OSG_FOX_ERROR_CHECKER ( hotSpotX >= 0 && hotSpotX <= width );
  OSG_FOX_ERROR_CHECKER ( hotSpotY >= 0 && hotSpotY <= height );

  // Initialize.
  std::auto_ptr<FX::FXCursor> cursor;

  // Process the format.
  switch ( format )
  {
  case Cursor::CURSOR_XBM:

    cursor = std::auto_ptr<FX::FXCursor> ( new FX::FXCursor ( app, source, mask, width, height, hotSpotX, hotSpotY ) );
    break;

  case Cursor::CURSOR_CUR:

    cursor = std::auto_ptr<FX::FXCursor> ( new FX::FXCURCursor ( app, source ) );
    break;

  default:

    OSG_FOX_ERROR_CHECKER ( 0 );
    return 0x0;
  }

  // Should be true.
  OSG_FOX_ERROR_CHECKER ( 0x0 != cursor.get() );

  // Create the cursor if we're supposed to.
  if ( createIt ) 
    cursor->create();

  // Return it.
  return cursor.release();
}
