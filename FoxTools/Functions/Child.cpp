
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  File functions.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Functions/Child.h"
#include "FoxTools/Headers/Window.h"

#include "Usul/Cast/Cast.h"

#include <stdexcept>


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number this window is in its parent's list of children.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int FoxTools::Functions::whichChild ( const FX::FXObject *obj )
{
  // We don't modify it.
  FX::FXObject *object ( const_cast < FX::FXObject * > ( obj ) );

  // Get the window.
  FX::FXWindow *window ( SAFE_CAST_FOX ( FX::FXWindow, object ) );
  if ( 0x0 == window )
    throw std::runtime_error ( "Error 3083484871: Given FOX object is not a window" );

  // Get the parent.
  FX::FXWindow *parent ( window->getParent() );
  if ( 0x0 == parent )
    throw std::runtime_error ( "Error 1004686958: Given FOX object does not have a parent" );

  // Loop through the children.
  unsigned int count ( 0 );
  for ( FX::FXWindow *child = parent->getFirst(); 0x0 != child; child = child->getNext() )
  {
    // Return if we have a match.
    if ( window == child )
      return count;

    // Increment.
    ++count;
  }

  // If we get to here then we failed.
  throw std::runtime_error ( "Error 3293115579: Failed to determine which child the given object is with respect to its parent" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Fill the vector with the children.
//
///////////////////////////////////////////////////////////////////////////////

void FoxTools::Functions::getChildren ( FX::FXWindow *parent, FoxTools::Functions::Children &v )
{
  if ( 0x0 == parent )
    return;

  v.clear();
  v.reserve ( parent->numChildren() );
  for ( FX::FXWindow *i = parent->getFirst(); i; i = i->getNext() )
    v.push_back ( i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a vector of the children.
//
///////////////////////////////////////////////////////////////////////////////

FoxTools::Functions::Children FoxTools::Functions::getChildren ( FX::FXWindow *parent )
{
  FoxTools::Functions::Children children;
  FoxTools::Functions::getChildren ( parent, children );
  return children;
}

#if 0
///////////////////////////////////////////////////////////////////////////////
//
//  Find the i'th of type.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXWindow *FoxTools::Functions::findChild ( FX::FXWindow *parent, unsigned int which, const FX::FXMetaClass *type )
{
  // Handle trivial case.
  if ( 0x0 == parent )
    return 0x0;

  // Initialize counter.
  unsigned int count ( 0 );

  // Look for child.
  for ( FX::FXWindow *i = parent->getFirst(); i; i = i->getNext() )
  {
    if ( i->isMemberOf ( type ) )
    {
      if ( count == which )
        return i;
      else
        ++count;
    }
  }

  // Did not find the child.
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the i'th of type.
//
///////////////////////////////////////////////////////////////////////////////

void FoxTools::Functions::hideChild ( FX::FXWindow *parent, unsigned int which, const FX::FXMetaClass *type )
{
  FX::FXWindow *child ( FoxTools::Functions::findChild ( parent, which, type ) );
  if ( child )
    child->hide();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Replace text in the first child that contains the string and is of type.
//
///////////////////////////////////////////////////////////////////////////////

void FoxTools::Functions::replaceText ( FX::FXWindow *window, const std::string &oldString, const std::string &newString, const FX::FXMetaClass *type )
{
  FX::FXWindow *child ( FoxTools::Functions::findChild ( parent, which, type ) );
  if ( child )
    child->hide();
}
#endif
