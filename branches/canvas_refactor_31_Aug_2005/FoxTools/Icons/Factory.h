
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

#ifndef _FOX_TOOLS_ICON_FACTORY_H_
#define _FOX_TOOLS_ICON_FACTORY_H_

#include "FoxTools/Export/Export.h"

#include <vector>
#include <map>

namespace FX { class FXMetaClass; class FXIcon; };


namespace FoxTools {
namespace Icons {


class FOX_TOOLS_EXPORT Factory
{
public:

  // Useful typedef(s).
  typedef std::vector < unsigned char > Array;
  typedef std::pair < const FX::FXMetaClass *, Array > Data;

  // Icons
  enum 
  {
    ICON_ABOUT, 
    ICON_COPY, 
    ICON_CUT, 
    ICON_DELETE, 
    ICON_ERROR, 
    ICON_FRONT,
    ICON_BACK,
    ICON_TOP,
    ICON_BOTTOM,
    ICON_LEFT,
    ICON_RIGHT,
    ICON_INFO, 
    ICON_NEW, 
    ICON_OPEN, 
    ICON_PASTE, 
    ICON_PRINT, 
    ICON_QUESTION, 
    ICON_SAVE, 
    ICON_TUX, 
    ICON_WARNING,
    ICON_ARROW,
    ICON_EYE,
    ICON_HAND,
    ICON_HOME,
    ICON_POLYGONS_FILLED,
    ICON_POLYGONS_WIRE,
    ICON_POLYGONS_HIDDEN,
    ICON_POLYGONS_POINTS,
    ICON_SEEK,
    ICON_SET_HOME,
    ICON_TOOL_SELECTION,
    ICON_NO_TRIANGLE,
    ICON_TRIANGLE,
    ICON_DELETE_CONNECTED,
    ICON_KEEP_CONNECTED,
    ICON_CAMERA,
    ICON_STOP,
    ICON_PAUSE,
    ICON_PLAY,
    ICON_PLAY_BACKWARDS,
    ICON_FORWARD_ARROW,
    ICON_FORWARD_ARROW_BAR,
    ICON_BACKWARD_ARROW,
    ICON_BACKWARD_ARROW_BAR,
    ICON_RED_CIRCLE,
    ICON_RED_X,
    ICON_WRITING,
    ICON_XYZ,
    ICON_SMALL_DOT,
    ICON_WRITE,
    ICON_LOGO,
    ICON_LIGHTING_OFF,
    ICON_LIGHTING_ON,
    ICON_LIGHTING_TWO_SIDED_OFF,
    ICON_LIGHTING_TWO_SIDED_ON,
    ICON_NO_LIGHTNING_BOLT,
    ICON_LIGHTNING_BOLT,
    ICON_CURVE,
    ICON_FLAT,
    ICON_SMOOTH,
    LAST
  };

  // It's a singleton.
  static Factory *    instance();

  // Get the icon data. Throws if value not found.
  const Data &        data ( unsigned int id ) const;

  // Set the icon data.
  void                data ( unsigned int id, const Data &d );

  // Get the icon. Throws if value not found.
  FX::FXIcon *        icon ( unsigned int id ) const;

private:

  // Constructor/destructor.
  Factory();
  ~Factory();

  // Cannot copy.
  Factory ( const Factory & );
  Factory &operator = ( const Factory & );

  typedef std::map < unsigned int, Data > Map;
  Map _map;
  static Factory *_instance;
};


}; // namespace Icons
}; // namespace FoxTools


#endif // _FOX_TOOLS_ICON_FACTORY_H_
