
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Visitor that sends messages.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_MESSAGE_VISITOR_H_
#define _MENUKIT_MESSAGE_VISITOR_H_

#include "MenuKit/Visitor.h"
#include "MenuKit/Messages.h"


namespace MenuKit {

class Menu;
class Button;
class Item;

class MENUKIT_EXPORT MessageVisitor : public Visitor
{
public:

  // Typedefs and smart-pointers.
  typedef Visitor BaseClass;
  MENUKIT_DECLARE_POINTER ( MessageVisitor );

  // Construction.
  MessageVisitor ( Message m );

  // Apply the visitor to the items.
  virtual void      apply ( Menu & );
  virtual void      apply ( Button & );

protected:

  // Use reference counting.
  virtual ~MessageVisitor();

private:

  // No copying.
  MessageVisitor ( const MessageVisitor & );
  MessageVisitor &operator = ( const MessageVisitor & );

  Message _message;
};


}; // namespace MenuKit


#endif // _MENUKIT_MESSAGE_VISITOR_H_
