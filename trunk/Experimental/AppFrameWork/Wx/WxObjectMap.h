
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Map between objects.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GUI_OBJECT_MAP_CLASS_H_
#define _WX_GUI_OBJECT_MAP_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"

namespace AFW { namespace Core { class Object; } }
class wxObject;


namespace WxObjectMap
{
  AFW::Core::Object *    findObject ( const wxObject *key );
  wxObject *             findObject ( const AFW::Core::Object *key );

  template < class T > T *find ( const wxObject *key )
  {
    return ( dynamic_cast < T * > ( findObject ( key ) ) );
  }
  template < class T > T *find ( const AFW::Core::Object *key )
  {
    wxObject *object ( findObject ( key ) );
    return ( ( object ) ? wxDynamicCast ( object, T ) : 0x0 );
  }

  void                   set ( AFW::Core::Object *, wxObject * );
  void                   set ( wxObject *, AFW::Core::Object * );

  void                   remove ( const AFW::Core::Object * );
  void                   remove ( const wxObject * );
};


#endif // _WX_GUI_OBJECT_MAP_CLASS_H_
