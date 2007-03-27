
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
#include "FoxTools/Cursors/CUR.h"
#include "FoxTools/Errors/ErrorChecker.h"
#include "FoxTools/Functions/App.h"
#include "FoxTools/Headers/CURCursor.h"

#include <memory>

using namespace FoxTools;
using namespace FoxTools::Cursors;


///////////////////////////////////////////////////////////////////////////////
//
//  Create the cursor.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXCursor *Factory::create ( Type type )
{
  FX::FXCursor *cursor ( Factory::make ( type ) );
  if ( cursor )
    cursor->create();
  return cursor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the cursor.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXCursor *Factory::make ( Type type )
{
  switch ( type )
  {
    case Factory::CURSOR_ARROW:
      return new FX::FXCursor ( FoxTools::Functions::application(), FX::CURSOR_ARROW );
    case Factory::CURSOR_ROTATE:
      return new FX::FXCURCursor ( FoxTools::Functions::application(), FoxTools::Cursors::CUR::Rotate::source );

    case Factory::CURSOR_TRANSLATE: 
      return new FX::FXCURCursor ( FoxTools::Functions::application(), FoxTools::Cursors::CUR::Translate::source );

    case Factory::CURSOR_SCALE:
      return new FX::FXCURCursor ( FoxTools::Functions::application(), FoxTools::Cursors::CUR::Scale::source );

    case Factory::CURSOR_SEEK:
      return new FX::FXCURCursor ( FoxTools::Functions::application(), FoxTools::Cursors::CUR::Seek::source );

    case Factory::CURSOR_HYPERLINK:
      return new FX::FXCURCursor ( FoxTools::Functions::application(), FoxTools::Cursors::CUR::_hyperlinkCursorArrayCUR );

    case Factory::CURSOR_SELECTION:
      return new FX::FXCURCursor ( FoxTools::Functions::application(), FoxTools::Cursors::CUR::_selectionCursorArrayCUR );

    case Factory::CURSOR_GUN_SIGHT:
      return new FX::FXCURCursor ( FoxTools::Functions::application(), FoxTools::Cursors::CUR::_gunSightCursorArrayCUR );
    default:
      throw std::runtime_error ( "Error 2242661329: invalid cursor type" );
      break;
  }
}
