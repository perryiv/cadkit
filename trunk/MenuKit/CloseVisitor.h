
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Visitor that closes the menu.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_CLOSE_VISITOR_H_
#define _MENUKIT_CLOSE_VISITOR_H_

#include "MenuKit/Visitor.h"

namespace MenuKit {

class Menu;

class MENUKIT_EXPORT CloseVisitor : public Visitor
{
public:

  // Typedefs and smart-pointers.
  typedef Visitor BaseClass;
  MENUKIT_DECLARE_POINTER ( CloseVisitor );

  // Construction.
  CloseVisitor();

  // Apply the visitor to the menu.
  virtual void apply ( Menu & );

protected:

  // Use reference counting.
  virtual ~CloseVisitor();

private:

  // No copying.
  CloseVisitor ( const CloseVisitor & );
  CloseVisitor &operator = ( const CloseVisitor & );
};


}; // namespace MenuKit


#endif // _MENUKIT_CLOSE_VISITOR_H_
