
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
class wxObject; class wxMenuItem;


namespace WxObjectMap
{
  AFW::Core::Object *    findObject ( const wxObject *key );
  wxObject *             findObject ( const AFW::Core::Object *key );
  wxMenuItem *           findMenuItem ( wxWindowID key );

  template < class T > T *find ( const wxObject *key )
  {
    AFW::Core::Object *object ( findObject ( key ) );
    T *t ( dynamic_cast < T * > ( object ) );
    return t;
  }
  template < class T > T *find ( const AFW::Core::Object *key )
  {
    wxObject *object ( findObject ( key ) );
    T *t ( ( object ) ? wxDynamicCast ( object, T ) : 0x0 );
    return t;
  }

  void                   set ( AFW::Core::Object *, wxObject * );
  void                   set ( wxObject *, AFW::Core::Object * );
  void                   set ( wxWindowID, wxMenuItem * );

  void                   remove ( const AFW::Core::Object * );
  void                   remove ( const wxObject * );
  void                   remove ( wxWindowID );
};


#endif // _WX_GUI_OBJECT_MAP_CLASS_H_
