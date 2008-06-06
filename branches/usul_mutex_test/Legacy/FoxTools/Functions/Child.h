
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Child functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_CHILD_FUNCTIONS_H_
#define _FOX_TOOLS_CHILD_FUNCTIONS_H_

#include "FoxTools/Export/Export.h"
#include "FoxTools/Headers/Window.h"

#include "Usul/MPL/SameType.h"


namespace FoxTools {
namespace Functions {


///////////////////////////////////////////////////////////////////////////////
//
//  Get container of the children that are of the specified type.
//
///////////////////////////////////////////////////////////////////////////////

template < class Container_, class ObjectType_ > struct Children
{
  typedef Container_ Container;
  typedef ObjectType_ ObjectType;

  static void get ( FX::FXWindow *parent, Container &v, bool traverse = false )
  {
    // Take this out if it proves to be a problem. If there is a way to get the 
    // object-type automatically from the container's value_type (which is a 
    // pointer type) then that would be better.
    USUL_ASSERT_SAME_TYPE ( typename Container::value_type, ObjectType * );

    if ( 0x0 == parent )
      return;
    v.reserve ( v.size() + parent->numChildren() );
    for ( FX::FXWindow *i = parent->getFirst(); i; i = i->getNext() )
    {
      if ( i->isMemberOf ( FXMETACLASS ( ObjectType ) ) )
        v.push_back ( reinterpret_cast < ObjectType * > ( i ) );
      if ( traverse )
        get ( i, v, traverse );
    }
  }

  static Container get ( FX::FXWindow *parent, bool traverse = false )
  {
    Container v;
    get ( parent, v, traverse );
    return v;
  }
};


}; // namespace Functions
}; // namespace FoxTools


#endif // _FOX_TOOLS_CHILD_FUNCTIONS_H_
