
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

#include "MenuKit/Export.h"
#include "Usul/Base/Object.h"

namespace MenuKit {

class Menu;
class Separator;
class Button;
class RadioButton;
class ToggleButton;

class MENUKIT_EXPORT Visitor : public Usul::Base::Object
{
public:

  // Typedefs and smart-pointers.
  typedef Usul::Base::Object BaseClass;
  USUL_DECLARE_REF_POINTERS ( Visitor );

  // Possible modes.
  enum Mode { NONE, EXPANDED, ALL };

  // Apply this visitor to the items.
  virtual void        apply ( Menu &m );
  virtual void        apply ( Button &b );
  virtual void        apply ( Separator &s );
  virtual void        apply ( RadioButton &radio );
  virtual void        apply ( ToggleButton &toggle );

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
  virtual ~Visitor();

private:

  // No copying.
  Visitor ( const Visitor & );
  Visitor &operator = ( const Visitor & );

  Mode _mode;
};


}; // namespace MenuKit


#endif // _MENUKIT_ITEM_VISITOR_
