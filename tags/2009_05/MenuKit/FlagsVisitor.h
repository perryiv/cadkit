
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Visitor that sets the flags to all items.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_FLAGS_VISITOR_H_
#define _MENUKIT_FLAGS_VISITOR_H_

#include "MenuKit/Visitor.h"


namespace MenuKit {

class Menu;
class Button;
class Item;

class MENUKIT_EXPORT FlagsVisitor : public Visitor
{
public:

  // Typedefs and smart-pointers.
  typedef Visitor BaseClass;
  USUL_DECLARE_REF_POINTERS ( FlagsVisitor );

  // The action to take.
  enum Action { ADD, REMOVE, TOGGLE };

  // Construction.
  FlagsVisitor ( Action action, unsigned int flags );

  // Apply the visitor to the items.
  virtual void      apply ( Menu & );
  virtual void      apply ( Button & );

protected:

  // Use reference counting.
  virtual ~FlagsVisitor();

  // Apply the visitor to the item.
  void              _apply ( Item &item );

private:

  // No copying.
  FlagsVisitor ( const FlagsVisitor & );
  FlagsVisitor &operator = ( const FlagsVisitor & );

  unsigned int _flags;
  Action _action;
};


}; // namespace MenuKit


#endif // _MENUKIT_FLAGS_VISITOR_H_
