
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Frame class to arrange child windows.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Windows/Frame.h"
#include "AppFrameWork/Core/BaseVisitor.h"

using namespace AFW::Windows;

AFW_IMPLEMENT_OBJECT ( Frame );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Frame::Frame() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Frame::~Frame()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Frame::accept ( AFW::Core::BaseVisitor *v )
{
  Guard guard ( this->mutex() );
  if ( v )
    v->visit ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the frame maximized?
//
///////////////////////////////////////////////////////////////////////////////

bool Frame::maximized() const
{
  Guard guard ( this->mutex() );
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the frame's maximized state
//
///////////////////////////////////////////////////////////////////////////////

void Frame::maximized ( bool )
{
  Guard guard ( this->mutex() );
}
