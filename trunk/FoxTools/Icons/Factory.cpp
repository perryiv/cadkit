
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Icon factory class.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Icons/Images.h"
#include "FoxTools/Functions/App.h"
#include "FoxTools/Headers/BMPIcon.h"
#include "FoxTools/Headers/GIFIcon.h"
#include "FoxTools/Headers/PNGIcon.h"

#include <stdexcept>
#include <sstream>

using namespace FoxTools;
using namespace FoxTools::Icons;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member(s).
//
///////////////////////////////////////////////////////////////////////////////

Factory *Factory::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Macros to shorten the map-insertion lines.
//
///////////////////////////////////////////////////////////////////////////////

#define ITERATORS(array_name) array_name, array_name + sizeof ( array_name )
#define POPULATE(format_type,name_space,image_name) \
  _map.insert ( Value ( ICON_##image_name, Data ( format_type, Array \
    ( ITERATORS ( name_space::IMAGE_##image_name ) ) ) ) )


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Factory() : _map()
{
  // For readability.
  const FX::FXMetaClass *png = &FX::FXPNGIcon::metaClass;
  const FX::FXMetaClass *gif = &FX::FXGIFIcon::metaClass;
  const FX::FXMetaClass *bmp = &FX::FXBMPIcon::metaClass;
  namespace PNG = Images::PNG;
  namespace GIF = Images::GIF;
  namespace BMP = Images::BMP;
  typedef Map::value_type Value;

  // Populate the map.
  POPULATE ( png, PNG, ABOUT    );
  POPULATE ( gif, GIF, OPEN     );
  POPULATE ( gif, GIF, COPY     );
  POPULATE ( gif, GIF, CUT      );
  POPULATE ( gif, GIF, DELETE   );
  POPULATE ( gif, GIF, ERROR    );
  POPULATE ( gif, GIF, FRONT    );
  POPULATE ( gif, GIF, BACK     );
  POPULATE ( gif, GIF, TOP      );
  POPULATE ( gif, GIF, BOTTOM   );
  POPULATE ( gif, GIF, LEFT     );
  POPULATE ( gif, GIF, RIGHT    );
  POPULATE ( gif, GIF, INFO     );
  POPULATE ( gif, GIF, NEW      );
  POPULATE ( gif, GIF, PASTE    );
  POPULATE ( gif, GIF, PRINT    );
  POPULATE ( gif, GIF, QUESTION );
  POPULATE ( gif, GIF, SAVE     );
  POPULATE ( gif, GIF, TUX      );
  POPULATE ( gif, GIF, WARNING  );
  POPULATE ( gif, GIF, CAMERA   );
  POPULATE ( gif, GIF, SEEK     );
  POPULATE ( gif, GIF, TOOL_SELECTION     );
  POPULATE ( gif, GIF, TRIANGLE           );
  POPULATE ( gif, GIF, DELETE_CONNECTED   );
  POPULATE ( gif, GIF, KEEP_CONNECTED     );
  POPULATE ( gif, GIF, ARROW              );
  POPULATE ( gif, GIF, EYE                );
  POPULATE ( gif, GIF, HAND               );
  POPULATE ( gif, GIF, HOME               );
  POPULATE ( gif, GIF, FILLED             );
  POPULATE ( gif, GIF, LINES              );
  POPULATE ( gif, GIF, POINTS             );
  POPULATE ( gif, GIF, STOP               );
  POPULATE ( gif, GIF, PAUSE              );
  POPULATE ( gif, GIF, PLAY               );
  POPULATE ( gif, GIF, PLAY_BACKWARDS     );
  POPULATE ( gif, GIF, FORWARD_ARROW      );
  POPULATE ( gif, GIF, FORWARD_ARROW_BAR  );
  POPULATE ( gif, GIF, BACKWARD_ARROW     );
  POPULATE ( gif, GIF, BACKWARD_ARROW_BAR );
  POPULATE ( gif, GIF, RED_CIRCLE );
  POPULATE ( gif, GIF, RED_X );
  POPULATE ( gif, GIF, WRITING );
  POPULATE ( gif, GIF, XYZ );
  POPULATE ( gif, GIF, SMALL_DOT );
  POPULATE ( gif, GIF, WRITE );
  POPULATE ( gif, GIF, LOGO );
  POPULATE ( bmp, BMP, SET_HOME );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::~Factory()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the one instance. TODO, make thread-safe.
//
///////////////////////////////////////////////////////////////////////////////

Factory *Factory::instance()
{
  if ( 0x0 == _instance )
    _instance = new Factory;
  return _instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the icon data. Throws if value not found.
//
///////////////////////////////////////////////////////////////////////////////

const Factory::Data &Factory::data ( unsigned int id ) const
{
  // Look for the icon.
  Map::const_iterator i = _map.find ( id );

  // Make sure it worked.
  if ( _map.end() == i )
  {
    std::ostringstream message;
    message << "Error 3446334204, invalid icon id = " << id;
    throw std::runtime_error ( message.str() );
  }

  // Return the array.
  return i->second;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the icon data.
//
///////////////////////////////////////////////////////////////////////////////

void Factory::data ( unsigned int id, const Data &d )
{
  _map[id] = d;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the icon. Throws if value not found.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXIcon *Factory::icon ( unsigned int id ) const
{
  // Get the icon data.
  const Factory::Data &data = this->data ( id );
  const FX::FXMetaClass *type = data.first;
  const Factory::Array &array = data.second;

  // Make the icon.
  FX::FXApp *app ( FoxTools::Functions::application() );
  FX::FXIcon *icon = 0x0;
  if ( FXMETACLASS ( FX::FXGIFIcon ) == type )
    return new FX::FXGIFIcon ( app, &array[0] );
  else if ( FXMETACLASS ( FX::FXPNGIcon ) == type )
    return new FX::FXPNGIcon ( app, &array[0] );
  else if ( FXMETACLASS ( FX::FXBMPIcon ) == type )
    return new FX::FXBMPIcon ( app, &array[0] );
  else
  {
    std::ostringstream message;
    message << "Error 2351912630, invalid icon type = " << type->getClassName();
    throw std::runtime_error ( message.str() );
  }
}
