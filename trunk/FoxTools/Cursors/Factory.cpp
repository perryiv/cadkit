
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Cursor factory class.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Cursors/Factory.h"
#include "FoxTools/Cursors/Arrays.h"
#include "FoxTools/Errors/ErrorChecker.h"

#include <memory>

#include "fxver.h"
#include "fxdefs.h"
#include "FXHash.h"
#include "FXStream.h"
#include "FXCURCursor.h"

using namespace FoxTools;
using namespace FoxTools::Cursors;


///////////////////////////////////////////////////////////////////////////////
//
//  Set the standard cursor.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXCursor *Factory::create ( Type type, bool createIt, FX::FXApp *app )
{
  FOX_TOOLS_ERROR_CHECKER ( 0x0 != app );

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
    case Factory::CURSOR_ARROW:
    {
      // Make the new cursor.
      std::auto_ptr<FX::FXCursor> cursor ( new FX::FXCursor ( app, FX::CURSOR_ARROW ) );
      FOX_TOOLS_ERROR_CHECKER ( 0x0 != cursor.get() );

      // Create the cursor if we're supposed to.
      if ( createIt ) 
        cursor->create();

      // Return the new cursor.
      return cursor.release();
    }

    case Factory::CURSOR_ROTATE:
    
      source   = FoxTools::Cursors::Arrays::_rotateCursorArrayXBM;
      mask     = FoxTools::Cursors::Arrays::_rotateCursorArrayXBM;
      width    = FoxTools::Cursors::Arrays::_rotateCursorWidth;
      height   = FoxTools::Cursors::Arrays::_rotateCursorHeight;
      hotSpotX = FoxTools::Cursors::Arrays::_rotateCursorHotSpotX;
      hotSpotY = FoxTools::Cursors::Arrays::_rotateCursorHotSpotY;
      format   = CURSOR_XBM;
      break;

    case Factory::CURSOR_TRANSLATE: 
      
      source   = FoxTools::Cursors::Arrays::_translateCursorArrayXBM;
      mask     = FoxTools::Cursors::Arrays::_translateCursorArrayXBM;
      width    = FoxTools::Cursors::Arrays::_translateCursorWidth;
      height   = FoxTools::Cursors::Arrays::_translateCursorHeight;
      hotSpotX = FoxTools::Cursors::Arrays::_translateCursorHotSpotX;
      hotSpotY = FoxTools::Cursors::Arrays::_translateCursorHotSpotY;
      format   = CURSOR_XBM;
      break;

    case Factory::CURSOR_SCALE:
      
      source   = FoxTools::Cursors::Arrays::_scaleCursorArrayXBM;
      mask     = FoxTools::Cursors::Arrays::_scaleCursorArrayXBM;
      width    = FoxTools::Cursors::Arrays::_scaleCursorWidth;
      height   = FoxTools::Cursors::Arrays::_scaleCursorHeight;
      hotSpotX = FoxTools::Cursors::Arrays::_scaleCursorHotSpotX;
      hotSpotY = FoxTools::Cursors::Arrays::_scaleCursorHotSpotY;
      format   = CURSOR_XBM;
      break;

    case Factory::CURSOR_SEEK:
      
      source   = FoxTools::Cursors::Arrays::_seekCursorArrayXBM;
      mask     = FoxTools::Cursors::Arrays::_seekCursorArrayXBM;
      width    = FoxTools::Cursors::Arrays::_seekCursorWidth;
      height   = FoxTools::Cursors::Arrays::_seekCursorHeight;
      hotSpotX = FoxTools::Cursors::Arrays::_seekCursorHotSpotX;
      hotSpotY = FoxTools::Cursors::Arrays::_seekCursorHotSpotY;
      format   = CURSOR_XBM;
      break;

    case Factory::CURSOR_HYPERLINK:
      
      source   = FoxTools::Cursors::Arrays::_hyperlinkCursorArrayCUR;
      mask     = FoxTools::Cursors::Arrays::_hyperlinkCursorArrayCUR;
      width    = FoxTools::Cursors::Arrays::_hyperlinkCursorWidth;
      height   = FoxTools::Cursors::Arrays::_hyperlinkCursorHeight;
      hotSpotX = FoxTools::Cursors::Arrays::_hyperlinkCursorHotSpotX;
      hotSpotY = FoxTools::Cursors::Arrays::_hyperlinkCursorHotSpotY;
      format = CURSOR_CUR;
      break;

    case Factory::CURSOR_SELECTION:

      source   = FoxTools::Cursors::Arrays::_selectionCursorArrayCUR;
      mask     = FoxTools::Cursors::Arrays::_selectionCursorArrayCUR;
      width    = FoxTools::Cursors::Arrays::_selectionCursorWidth;
      height   = FoxTools::Cursors::Arrays::_selectionCursorHeight;
      hotSpotX = FoxTools::Cursors::Arrays::_selectionCursorHotSpotX;
      hotSpotY = FoxTools::Cursors::Arrays::_selectionCursorHotSpotY;
      format   = CURSOR_CUR;
      break;

    case Factory::CURSOR_GUN_SIGHT:

      source   = FoxTools::Cursors::Arrays::_gunSightCursorArrayCUR;
      mask     = FoxTools::Cursors::Arrays::_gunSightCursorArrayCUR;
      width    = FoxTools::Cursors::Arrays::_gunSightCursorWidth;
      height   = FoxTools::Cursors::Arrays::_gunSightCursorHeight;
      hotSpotX = FoxTools::Cursors::Arrays::_gunSightCursorHotSpotX;
      hotSpotY = FoxTools::Cursors::Arrays::_gunSightCursorHotSpotY;
      format   = CURSOR_CUR;
      break;

    default:

      throw std::runtime_error ( "Error 2242661329: invalid cursor type" );
      break;
  }

  // Should be true.
  FOX_TOOLS_ERROR_CHECKER ( 0x0 != source );
  FOX_TOOLS_ERROR_CHECKER ( 0x0 != mask );

  // Call the other one.
  return Factory::create ( source, mask, width, height, hotSpotX, hotSpotY, format, createIt, app );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the cursor from the given array.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXCursor *Factory::create ( 
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
  FOX_TOOLS_ERROR_CHECKER ( 0x0 != app );
  FOX_TOOLS_ERROR_CHECKER ( 0x0 != source );
  FOX_TOOLS_ERROR_CHECKER ( 0x0 != mask );
  FOX_TOOLS_ERROR_CHECKER ( width > 0 && width <= 32 );
  FOX_TOOLS_ERROR_CHECKER ( height > 0 && height <= 32 );
  FOX_TOOLS_ERROR_CHECKER ( hotSpotX >= 0 && hotSpotX <= width );
  FOX_TOOLS_ERROR_CHECKER ( hotSpotY >= 0 && hotSpotY <= height );

  // Initialize.
  std::auto_ptr<FX::FXCursor> cursor;

  // Process the format.
  switch ( format )
  {
  case Factory::CURSOR_XBM:

    cursor = std::auto_ptr<FX::FXCursor> ( new FX::FXCursor ( app, source, mask, width, height, hotSpotX, hotSpotY ) );
    break;

  case Factory::CURSOR_CUR:

    cursor = std::auto_ptr<FX::FXCursor> ( new FX::FXCURCursor ( app, source ) );
    break;

  default:

    FOX_TOOLS_ERROR_CHECKER ( 0 );
    return 0x0;
  }

  // Should be true.
  FOX_TOOLS_ERROR_CHECKER ( 0x0 != cursor.get() );

  // Create the cursor if we're supposed to.
  if ( createIt ) 
    cursor->create();

  // Return it.
  return cursor.release();
}
