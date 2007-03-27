
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Factory classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_BUILDER_FACTORY_H_
#define _FOX_TOOLS_BUILDER_FACTORY_H_


//namespace FX { class FXWindow; }
#include "FoxTools/Headers/Window.h"

namespace FoxTools {
namespace Builder {


///////////////////////////////////////////////////////////////////////////////
//
//  Base factory class.
//
///////////////////////////////////////////////////////////////////////////////

struct Factory
{
  // Return a new window.
  virtual FX::FXWindow *create ( FX::FXWindow *parent ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Implementation for when only a parent is needed.
//
///////////////////////////////////////////////////////////////////////////////

template < class ParentType, class ClassType > struct NeedsParent : public Factory
{
  // Return a new window.
  virtual FX::FXWindow *create ( FX::FXWindow *p )
  {
    if ( p->isMemberOf ( &ParentType::metaClass ) )
      return new ClassType ( ( ParentType * ) p );
    return 0x0;
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Implementation for when an owner and string is needed.
//
///////////////////////////////////////////////////////////////////////////////

template < class OwnerType, class ClassType > struct NeedsOwnerAndString : public Factory
{
  // Return a new window.
  virtual FX::FXWindow *create ( FX::FXWindow *p )
  {
    if ( p->isMemberOf ( &OwnerType::metaClass ) )
      return new ClassType ( ( OwnerType * ) p, "" );
    return 0x0;
  }
};


} // namespace Builder
} // namespace FoxTools


#endif // _FOX_TOOLS_BUILDER_FACTORY_H_
