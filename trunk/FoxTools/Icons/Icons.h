
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

#ifndef _FOX_TOOLS_BINDING_ICONS_H_
#define _FOX_TOOLS_BINDING_ICONS_H_

#include "FoxTools/Icons/FoxIconsDll.h"

#include <vector>
#include <map>


namespace FoxTools {
namespace Icons {


class FOX_ICONS_EXPORT Icons
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
    ICON_INSERT, 
    ICON_PASTE, 
    ICON_PRINT, 
    ICON_QUESTION, 
    ICON_SAVE, 
    ICON_SAVE_AS, 
    ICON_TUX, 
    ICON_WARNING,
    ICON_ARROW,
    ICON_EYE,
    ICON_HAND,
    ICON_HOME,
    ICON_FILLED,
    ICON_LINES,
    ICON_POINTS,
    ICON_SEEK,
    ICON_SET_HOME,
    ICON_TOOL_SELECTION,
    ICON_TRIANGLE,
    ICON_DELETECONNECTED,
    ICON_KEEPCONNECTED,
    LAST
  };

  // It's a singleton.
  static Icons *      instance();

  // Get the icon data. Throws if value not found.
  const Data &        data ( unsigned int id ) const;

  // Set the icon data.
  void                data ( unsigned int id, const Data &d );

  // Get the icon. Throws if value not found.
  FX::FXIcon *        icon ( unsigned int id, FX::FXApp *app ) const;

private:

  // Constructor/destructor.
  Icons();
  ~Icons();

  // Cannot copy.
  Icons ( const Icons & );
  Icons &operator = ( const Icons & );

  typedef std::map < unsigned int, Data > Map;
  Map _map;
  static Icons *_instance;
};


}; // namespace Icons
}; // namespace FoxTools


#endif // _FOX_TOOLS_BINDING_ICONS_H_
