
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu group class.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Menus/MenuGroup.h"
#include "AppFrameWork/Core/BaseVisitor.h"

#include <limits>

using namespace AFW::Menus;

AFW_IMPLEMENT_OBJECT ( MenuGroup );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MenuGroup::MenuGroup() : BaseClass(),
  _underline ( std::numeric_limits<unsigned short>::max() )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MenuGroup::~MenuGroup()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the underline index.
//
///////////////////////////////////////////////////////////////////////////////

unsigned short MenuGroup::underline() const
{
  Guard guard ( this->mutex() );
  return _underline;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the underline index.
//
///////////////////////////////////////////////////////////////////////////////

void MenuGroup::underline ( unsigned short u )
{
  Guard guard ( this->mutex() );
  _underline = u;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void MenuGroup::accept ( AFW::Core::BaseVisitor *v )
{
  Guard guard ( this->mutex() );
  if ( v )
    v->visit ( this );
}
