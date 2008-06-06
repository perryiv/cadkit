
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  Cursor class.
//
//////////////////////////////////////////////////////////////////////////

#include "FoxCursor.h"
#include "FoxCursorArrays.h"
#include "FoxDefine.h"

#include "Standard/SlAssert.h"
#include "Standard/SlAutoPtr.h"

#ifdef _WIN32
# define NOMINMAX // Fox includes <windows.h>
#endif

#include "fx.h"
#include "FXCURCursor.h"


using namespace CadKit;


//////////////////////////////////////////////////////////////////////////
//
//  Set the standard cursor.
//
//////////////////////////////////////////////////////////////////////////

FXCursor *FoxCursor::create ( const Type &type, const bool &createIt, FXApp *app )
{
  SL_ASSERT ( app );

  // Initialize the array pointers.
  const unsigned char *source = NULL;
  const unsigned char *mask = NULL;

  // Initialize these also.
  int width = 32;
  int height = 32;
  int hotSpotX = 12;
  int hotSpotY = 11;
  Format format = CURSOR_XBM;

  // Initialize this too.
  SlAutoPtr<FXCursor> cursor;

  // Declare the new icon.
  switch ( type )
  {
  case FoxCursor::CURSOR_ARROW:

    // Make the new cursor.
    cursor = new FXCursor ( app, FX::CURSOR_ARROW );
    GUI_FOX_CHECK_EXPRESSION ( cursor.isValid() );

    // Create the cursor if we're supposed to.
    if ( createIt ) 
      cursor->create();

    // Return the new cursor.
    return cursor.release();

  case FoxCursor::CURSOR_ROTATE:
    
    source = _rotateCursorArrayXBM;
    mask = _rotateCursorArrayXBM;
    width = _rotateCursorWidth;
    height = _rotateCursorHeight;
    hotSpotX = _rotateCursorHotSpotX;
    hotSpotY = _rotateCursorHotSpotY;
    format = CURSOR_XBM;
    break;

  case FoxCursor::CURSOR_TRANSLATE: 
    
    source = _translateCursorArrayXBM;
    mask = _translateCursorArrayXBM;
    width = _translateCursorWidth;
    height = _translateCursorHeight;
    hotSpotX = _translateCursorHotSpotX;
    hotSpotY = _translateCursorHotSpotY;
    format = CURSOR_XBM;
    break;

  case FoxCursor::CURSOR_SCALE:
    
    source = _scaleCursorArrayXBM;
    mask = _scaleCursorArrayXBM;
    width = _scaleCursorWidth;
    height = _scaleCursorHeight;
    hotSpotX = _scaleCursorHotSpotX;
    hotSpotY = _scaleCursorHotSpotY;
    format = CURSOR_XBM;
    break;

  case FoxCursor::CURSOR_SEEK:
    
    source = _seekCursorArrayXBM;
    mask = _seekCursorArrayXBM;
    width = _seekCursorWidth;
    height = _seekCursorHeight;
    hotSpotX = _seekCursorHotSpotX;
    hotSpotY = _seekCursorHotSpotY;
    format = CURSOR_XBM;
    break;

  case FoxCursor::CURSOR_HYPERLINK:
    
    source = _hyperlinkCursorArrayCUR;
    mask = _hyperlinkCursorArrayCUR;
    width = _hyperlinkCursorWidth;
    height = _hyperlinkCursorHeight;
    hotSpotX = _hyperlinkCursorHotSpotX;
    hotSpotY = _hyperlinkCursorHotSpotY;
    format = CURSOR_CUR;
    break;

  default:

    SL_ASSERT ( 0 );
    return NULL;
  }

  // Should be true.
  SL_ASSERT ( source );
  SL_ASSERT ( mask );
  SL_ASSERT ( cursor.isNull() ); // Only used when making an arrow cursor.

  // Call the other one.
  return FoxCursor::create ( source, mask, width, height, hotSpotX, hotSpotY, format, createIt, app );
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the cursor from the given array.
//
//////////////////////////////////////////////////////////////////////////

FXCursor *FoxCursor::create ( 
  const unsigned char *source, 
  const unsigned char *mask, 
  const int &width, 
  const int &height, 
  const int &hotSpotX, 
  const int &hotSpotY, 
  const Format &format,
  const bool &createIt,
  FXApp *app )
{
  SL_ASSERT ( app );
  SL_ASSERT ( source );
  SL_ASSERT ( mask );
  SL_ASSERT ( app );
  SL_ASSERT ( width > 0 && width <= 32 );
  SL_ASSERT ( height > 0 && height <= 32 );
  SL_ASSERT ( hotSpotX >= 0 && hotSpotX <= width );
  SL_ASSERT ( hotSpotY >= 0 && hotSpotY <= height );

  // Initialize.
  SlAutoPtr<FXCursor> cursor;

  // Process the format.
  switch ( format )
  {
  case FoxCursor::CURSOR_XBM:

    cursor = new FXCursor ( app, source, mask, width, height, hotSpotX, hotSpotY );
    break;

  case FoxCursor::CURSOR_CUR:

    cursor = new FXCURCursor ( app, source );
    break;

  default:

    SL_ASSERT ( 0 );
    return NULL;
  }

  // Should be true.
  SL_ASSERT ( cursor.isValid() );

  // Create the cursor if we're supposed to.
  if ( createIt ) 
    cursor->create();

  // Return it.
  return cursor.release();
}
