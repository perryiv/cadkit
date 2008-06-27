
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all menu items.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_ITEM_
#define _MENUKIT_ITEM_

#include "MenuKit/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include <string>
#include <map>


namespace MenuKit {

class Visitor;
class Menu;

class MENUKIT_EXPORT Item : public Usul::Base::Referenced
{
public:

  // Typedefs and smart-pointers.
  typedef Usul::Base::Referenced        BaseClass;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex>   Guard;
  
  USUL_DECLARE_REF_POINTERS ( Item );

  // Possible flags.
  enum
  {
    ENABLED     = 0x00000001, // Default. Without this it is 'greyed' out, unusable
    EXPANDED    = 0x00000002, // flag for Menus, here for ease, opens/closes the menu
    MARKED      = 0x00000004, // flag shows an item is 'current' or should be 'highlighted'
  };

  // Construction.
  Item();
  Item ( const Item & );
  Item &operator = ( const Item & );

  // Accept the visitor.
  virtual void          accept ( Visitor & ) = 0;

  // Get the parent.
  const Menu *          parent() const { return _parent; }
  Menu *                parent()       { return _parent; }

  // Set/get the flags.
  unsigned int          flags() const;
  void                  flags ( unsigned int f );

  // Set/get the enabled flag.
  bool                  enabled() const;
  void                  enabled ( bool e );

  // Set/get the expansion flag.
  bool                  expanded() const;
  void                  expanded ( bool e );

  // Set/get the current flag.
  bool                  marked() const;
  void                  marked ( bool e );
  
  /// Get the mutex.
  Mutex&                mutex() const { return _mutex; }

  // Is this item a separator?
  virtual bool          separator () const { return false; }

protected:

  friend class Menu;

  // Use reference counting.
  virtual ~Item();

  // Set the parent.
  void                  _setParent ( Menu *m ) { _parent = m; }

private:

  mutable Mutex _mutex;
  Menu *_parent;
  unsigned int _flags;
};


} // namespace MenuKit


#endif // _MENUKIT_ITEM_
