
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

#ifdef __sgi
#define _CPP_CMATH 1
#endif

#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Icons/Images.h"
#include "FoxTools/Functions/App.h"
#include "FoxTools/Headers/BMPIcon.h"
#include "FoxTools/Headers/GIFIcon.h"
#include "FoxTools/Headers/PNGIcon.h"

#include <stdexcept>

#include "Usul/Headers/SStream.h"

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
#define MAP_INSERT(format_type,name_space,image_name) \
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
  MAP_INSERT ( png, PNG, ABOUT    );
  MAP_INSERT ( gif, GIF, OPEN     );
  MAP_INSERT ( gif, GIF, COPY     );
  MAP_INSERT ( gif, GIF, CUT      );
  MAP_INSERT ( gif, GIF, DELETE   );
  MAP_INSERT ( gif, GIF, ERROR    );
  MAP_INSERT ( gif, GIF, FRONT    );
  MAP_INSERT ( gif, GIF, BACK     );
  MAP_INSERT ( gif, GIF, TOP      );
  MAP_INSERT ( gif, GIF, BOTTOM   );
  MAP_INSERT ( gif, GIF, LEFT     );
  MAP_INSERT ( gif, GIF, RIGHT    );
  MAP_INSERT ( gif, GIF, INFO     );
  MAP_INSERT ( gif, GIF, NEW      );
  MAP_INSERT ( gif, GIF, PASTE    );
  MAP_INSERT ( gif, GIF, PRINT    );
  MAP_INSERT ( gif, GIF, QUESTION );
  MAP_INSERT ( gif, GIF, SAVE     );
  MAP_INSERT ( gif, GIF, TUX      );
  MAP_INSERT ( gif, GIF, WARNING  );
  MAP_INSERT ( gif, GIF, CAMERA   );
  MAP_INSERT ( gif, GIF, SEEK     );
  MAP_INSERT ( gif, GIF, TOOL_SELECTION     );
  MAP_INSERT ( gif, GIF, TRIANGLE           );
  MAP_INSERT ( gif, GIF, DELETE_CONNECTED   );
  MAP_INSERT ( gif, GIF, KEEP_CONNECTED     );
  MAP_INSERT ( gif, GIF, ARROW              );
  MAP_INSERT ( gif, GIF, EYE                );
  MAP_INSERT ( gif, GIF, HAND               );
  MAP_INSERT ( gif, GIF, HOME               );
  MAP_INSERT ( gif, GIF, POLYGONS_FILLED    );
  MAP_INSERT ( gif, GIF, POLYGONS_WIRE      );
  MAP_INSERT ( gif, GIF, POLYGONS_HIDDEN    );
  MAP_INSERT ( gif, GIF, POLYGONS_POINTS    );
  MAP_INSERT ( gif, GIF, STOP               );
  MAP_INSERT ( gif, GIF, PAUSE              );
  MAP_INSERT ( gif, GIF, PLAY               );
  MAP_INSERT ( gif, GIF, PLAY_BACKWARDS     );
  MAP_INSERT ( gif, GIF, FORWARD_ARROW      );
  MAP_INSERT ( gif, GIF, FORWARD_ARROW_BAR  );
  MAP_INSERT ( gif, GIF, BACKWARD_ARROW     );
  MAP_INSERT ( gif, GIF, BACKWARD_ARROW_BAR );
  MAP_INSERT ( gif, GIF, RED_CIRCLE );
  MAP_INSERT ( gif, GIF, RED_X      );
  MAP_INSERT ( gif, GIF, WRITING    );
  MAP_INSERT ( gif, GIF, XYZ        );
  MAP_INSERT ( gif, GIF, SMALL_DOT  );
  MAP_INSERT ( gif, GIF, WRITE      );
  MAP_INSERT ( gif, GIF, LOGO       );
  MAP_INSERT ( gif, GIF, LIGHTING_OFF       );
  MAP_INSERT ( gif, GIF, LIGHTING_ON        );
  MAP_INSERT ( gif, GIF, LIGHTING_TWO_SIDED_OFF );
  MAP_INSERT ( gif, GIF, LIGHTING_TWO_SIDED_ON  );
  MAP_INSERT ( gif, GIF, NO_LIGHTNING_BOLT       );
  MAP_INSERT ( gif, GIF, LIGHTNING_BOLT       );
  MAP_INSERT ( bmp, BMP, SET_HOME   );
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
