
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base visitor class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_ITEM_VISITOR_
#define _MENUKIT_ITEM_VISITOR_

#include "MenuKit/Referenced.h"

namespace MenuKit {

class Menu;
class Button;

class MENUKIT_EXPORT Visitor : public Referenced
{
public:

  // Typedefs and smart-pointers.
  typedef Referenced BaseClass;
  MENUKIT_DECLARE_POINTER ( Visitor );

  // Possible modes.
  enum Mode { NONE, EXPANDED, ALL };

  // Apply this visitor to the items.
  virtual void        apply ( Menu &m );
  virtual void        apply ( Button &b );

  // Set/get the mode.
  void                mode ( Mode m ) { _mode = m; }
  Mode                mode() const { return _mode; }

  // Traverse the tree.
  void                traverse ( Menu &m );

protected:

  // Construction.
  Visitor();
  Visitor ( Mode m );

  // Use reference counting.
  ~Visitor();

private:

  // No copying.
  Visitor ( const Visitor & );
  Visitor &operator = ( const Visitor & );

  Mode _mode;
};


}; // namespace MenuKit


#endif // _MENUKIT_ITEM_VISITOR_
