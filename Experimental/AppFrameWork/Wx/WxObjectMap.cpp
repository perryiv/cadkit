
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

#include "WxPrecompiled.h"
#include "WxObjectMap.h"

#include "AppFrameWork/Core/Window.h"

#include "Usul/Errors/Assert.h"
#include "Usul/Errors/Stack.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Start of the details.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Print info.
//
///////////////////////////////////////////////////////////////////////////////

void printInfo ( const wxObject *object, std::ostream &out )
{
  if ( 0x0 == object )
    return;

  wxClassInfo *classInfo ( object->GetClassInfo() );
  if ( classInfo )
  {
    wxString className ( classInfo->GetClassName() );
    if ( false == className.IsEmpty() )
    {
      out << ", Object Class Name: " << className;
    }
  }

  wxWindow *window ( wxDynamicCast ( object, wxWindow ) );
  if ( window )
  {
    wxString label ( window->GetLabel() );
    if ( false == label.IsEmpty() )
    {
      out << ", Object Label: " << label;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print info.
//
///////////////////////////////////////////////////////////////////////////////

void printInfo ( const AFW::Core::Object *object, std::ostream &out )
{
  if ( 0x0 == object )
    return;

  out << ", Object Class Name: " << object->typeId().name();

  const AFW::Core::Window *window ( dynamic_cast < const AFW::Core::Window * > ( object ) );
  if ( window )
  {
    out << ", Window Title: " << window->title();
    out << ", Window Text: " << window->textGet();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Internal class for mapping one way.
//
///////////////////////////////////////////////////////////////////////////////

template < class Key, class Value > struct Map
{
  // Not reference-counting by design.
  typedef std::map < const Key *, Value * > MapType;

  Map() : _map()
  {
  }

  ~Map()
  {
    for ( MapType::iterator i = _map.begin(); i != _map.end(); ++i )
    {
      std::ostringstream out;
      const Key *key ( i->first );
      const Value *value ( i->second );
      out << "Warning 3819733065: Key/Value pair remaining in map, key: " << key << ", value: " << value;
      Detail::printInfo ( key, out );
      Detail::printInfo ( value, out );
      Usul::Errors::Stack::instance().push ( out.str() );
    }
  }

  Value *find ( const Key *key )
  {
    MapType::iterator i ( _map.find ( key ) );
    return ( ( _map.end() == i ) ? 0x0 : i->second );
  }

  void remove ( const Key *key )
  {
    _map.erase ( key );
  }

  void set ( const Key *key, Value *value )
  {
    if ( 0x0 != key )
      _map[key] = value;
  }

private:

  MapType _map;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Local maps.
//
///////////////////////////////////////////////////////////////////////////////

typedef Map < AFW::Core::Object, wxObject > ObjectToWx;
typedef Map < wxObject, AFW::Core::Object > WxToObject;
ObjectToWx objectToWx;
WxToObject wxToObject;


///////////////////////////////////////////////////////////////////////////////
//
//  End of the details.
//
///////////////////////////////////////////////////////////////////////////////

} // namespace Detail


///////////////////////////////////////////////////////////////////////////////
//
//  Find the object.
//
///////////////////////////////////////////////////////////////////////////////

AFW::Core::Object *WxObjectMap::findObject ( const wxObject *object )
{
  return Detail::wxToObject.find ( object );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the object.
//
///////////////////////////////////////////////////////////////////////////////

wxObject *WxObjectMap::findObject ( const AFW::Core::Object *object )
{
  return Detail::objectToWx.find ( object );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the object.
//
///////////////////////////////////////////////////////////////////////////////

void WxObjectMap::set ( wxObject *wx, AFW::Core::Object *object )
{
  WxObjectMap::set ( object, wx );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the object.
//
///////////////////////////////////////////////////////////////////////////////

void WxObjectMap::set ( AFW::Core::Object *object, wxObject *wx )
{
  Detail::objectToWx.set ( object, wx );
  Detail::wxToObject.set ( wx, object );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the object.
//
///////////////////////////////////////////////////////////////////////////////

void WxObjectMap::remove ( const wxObject *wx )
{
  // Do not use ref-pointer here! We could be inside of the object's destructor.
  AFW::Core::Object *object ( WxObjectMap::findObject ( wx ) );

  // Remove from both maps.
  Detail::wxToObject.remove ( wx );
  Detail::objectToWx.remove ( object );

  // Should be true.
  USUL_ASSERT ( 0x0 == WxObjectMap::findObject ( wx ) );
  USUL_ASSERT ( 0x0 == WxObjectMap::findObject ( object ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the object.
//
///////////////////////////////////////////////////////////////////////////////

void WxObjectMap::remove ( const AFW::Core::Object *object )
{
  // Do not use ref-pointer here! We could be inside of the object's destructor.
  wxObject *wx ( WxObjectMap::findObject ( object ) );

  // Remove from both maps.
  Detail::objectToWx.remove ( object );
  Detail::wxToObject.remove ( wx );

  // Should be true.
  USUL_ASSERT ( 0x0 == WxObjectMap::findObject ( object ) );
  USUL_ASSERT ( 0x0 == WxObjectMap::findObject ( wx ) );
}
