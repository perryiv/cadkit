
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Icon class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Icons.h"
#include "Images.h"

#include <stdexcept>
#include <sstream>

#include "fx.h" //TODO include only needed headers
#include "FXPNGIcon.h"

using namespace FoxTools;
using namespace FoxTools::Icons;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member(s).
//
///////////////////////////////////////////////////////////////////////////////

FoxTools::Icons::Icons *FoxTools::Icons::Icons::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to shorten the map-insertion lines.
//
///////////////////////////////////////////////////////////////////////////////

#define ITERATORS(array_name) array_name, array_name + sizeof ( array_name )


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxTools::Icons::Icons::Icons() : _map()
{
  // For readability.
  const FX::FXMetaClass *png = &FX::FXPNGIcon::metaClass;
  const FX::FXMetaClass *gif = &FX::FXGIFIcon::metaClass;
  const FX::FXMetaClass *bmp = &FX::FXBMPIcon::metaClass;
  namespace PNG = FoxTools::Icons::Images::PNG;
  namespace GIF = FoxTools::Icons::Images::GIF;
  namespace BMP = FoxTools::Icons::Images::BMP;
  typedef Map::value_type Value;

  // Populate the map.
  _map.insert ( Value ( ICON_ABOUT,     Data ( png, Array ( ITERATORS ( PNG::IMAGE_ABOUT    ) ) ) ) );
  _map.insert ( Value ( ICON_OPEN,      Data ( png, Array ( ITERATORS ( PNG::IMAGE_OPEN     ) ) ) ) );
  _map.insert ( Value ( ICON_INSERT,    Data ( png, Array ( ITERATORS ( PNG::IMAGE_INSERT   ) ) ) ) );
  _map.insert ( Value ( ICON_COPY,      Data ( gif, Array ( ITERATORS ( GIF::IMAGE_COPY     ) ) ) ) );
  _map.insert ( Value ( ICON_CUT,       Data ( gif, Array ( ITERATORS ( GIF::IMAGE_CUT      ) ) ) ) );
  _map.insert ( Value ( ICON_DELETE,    Data ( gif, Array ( ITERATORS ( GIF::IMAGE_DELETE   ) ) ) ) );
  _map.insert ( Value ( ICON_ERROR,     Data ( gif, Array ( ITERATORS ( GIF::IMAGE_ERROR    ) ) ) ) );
  _map.insert ( Value ( ICON_FRONT,     Data ( gif, Array ( ITERATORS ( GIF::IMAGE_FRONT    ) ) ) ) );
  _map.insert ( Value ( ICON_BACK,      Data ( gif, Array ( ITERATORS ( GIF::IMAGE_BACK     ) ) ) ) );
  _map.insert ( Value ( ICON_TOP,       Data ( gif, Array ( ITERATORS ( GIF::IMAGE_TOP      ) ) ) ) );
  _map.insert ( Value ( ICON_BOTTOM,    Data ( gif, Array ( ITERATORS ( GIF::IMAGE_BOTTOM   ) ) ) ) );
  _map.insert ( Value ( ICON_LEFT,      Data ( gif, Array ( ITERATORS ( GIF::IMAGE_LEFT     ) ) ) ) );
  _map.insert ( Value ( ICON_RIGHT,     Data ( gif, Array ( ITERATORS ( GIF::IMAGE_RIGHT    ) ) ) ) );
  _map.insert ( Value ( ICON_INFO,      Data ( gif, Array ( ITERATORS ( GIF::IMAGE_INFO     ) ) ) ) );
  _map.insert ( Value ( ICON_NEW,       Data ( gif, Array ( ITERATORS ( GIF::IMAGE_NEW      ) ) ) ) );
  _map.insert ( Value ( ICON_PASTE,     Data ( gif, Array ( ITERATORS ( GIF::IMAGE_PASTE    ) ) ) ) );
  _map.insert ( Value ( ICON_PRINT,     Data ( gif, Array ( ITERATORS ( GIF::IMAGE_PRINT    ) ) ) ) );
  _map.insert ( Value ( ICON_QUESTION,  Data ( gif, Array ( ITERATORS ( GIF::IMAGE_QUESTION ) ) ) ) );
  _map.insert ( Value ( ICON_SAVE,      Data ( gif, Array ( ITERATORS ( GIF::IMAGE_SAVE     ) ) ) ) );
  _map.insert ( Value ( ICON_SAVE_AS,   Data ( gif, Array ( ITERATORS ( GIF::IMAGE_SAVE_AS  ) ) ) ) );
  _map.insert ( Value ( ICON_TUX,       Data ( gif, Array ( ITERATORS ( GIF::IMAGE_TUX      ) ) ) ) );
  _map.insert ( Value ( ICON_WARNING,   Data ( gif, Array ( ITERATORS ( GIF::IMAGE_WARNING  ) ) ) ) );
  _map.insert ( Value ( ICON_ARROW,     Data ( bmp, Array ( ITERATORS ( BMP::IMAGE_ARROW    ) ) ) ) );
  _map.insert ( Value ( ICON_EYE,       Data ( bmp, Array ( ITERATORS ( BMP::IMAGE_EYE      ) ) ) ) );
  _map.insert ( Value ( ICON_HAND,      Data ( bmp, Array ( ITERATORS ( BMP::IMAGE_HAND     ) ) ) ) );
  _map.insert ( Value ( ICON_HOME,      Data ( bmp, Array ( ITERATORS ( BMP::IMAGE_HOME     ) ) ) ) );
  _map.insert ( Value ( ICON_FILLED,    Data ( bmp, Array ( ITERATORS ( BMP::IMAGE_FILLED   ) ) ) ) );
  _map.insert ( Value ( ICON_LINES,     Data ( bmp, Array ( ITERATORS ( BMP::IMAGE_LINES    ) ) ) ) );
  _map.insert ( Value ( ICON_POINTS,    Data ( bmp, Array ( ITERATORS ( BMP::IMAGE_POINTS   ) ) ) ) );
  _map.insert ( Value ( ICON_SEEK,      Data ( bmp, Array ( ITERATORS ( BMP::IMAGE_SEEK     ) ) ) ) );
  _map.insert ( Value ( ICON_SET_HOME,  Data ( bmp, Array ( ITERATORS ( BMP::IMAGE_SET_HOME ) ) ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxTools::Icons::Icons::~Icons()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the one instance.
//
///////////////////////////////////////////////////////////////////////////////

FoxTools::Icons::Icons *FoxTools::Icons::Icons::instance()
{
  if ( 0x0 == _instance )
    _instance = new Icons;
  return _instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the icon data. Throws if value not found.
//
///////////////////////////////////////////////////////////////////////////////

const FoxTools::Icons::Icons::Data &FoxTools::Icons::Icons::data ( unsigned int id ) const
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

void FoxTools::Icons::Icons::data ( unsigned int id, const Data &d )
{
  _map[id] = d;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the icon. Throws if value not found.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXIcon *FoxTools::Icons::Icons::icon ( unsigned int id, FX::FXApp *app ) const
{
  // Get the icon data.
  const Icons::Data &data = this->data ( id );
  const FX::FXMetaClass *type = data.first;
  const Icons::Array &array = data.second;

  // Make the icon.
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
