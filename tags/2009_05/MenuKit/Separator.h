
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_KIT_SEPARATOR_H__
#define __MENU_KIT_SEPARATOR_H__

#include "MenuKit/Export.h"
#include "MenuKit/Item.h"

namespace MenuKit {

class MENUKIT_EXPORT Separator : public MenuKit::Item
{
public:
  typedef MenuKit::Item BaseClass;

  USUL_DECLARE_REF_POINTERS ( Separator );

  // Construction.
  Separator();
  Separator ( const Separator & );

  // Assignment.
  Separator& operator = ( const Separator & );

  // Accept the visitor.
  virtual void        accept ( Visitor &v );

  // Is this item a separator?
  virtual bool        separator () const { return true; }

protected:
  virtual ~Separator ();
};

}

#endif // __MENU_KIT_SEPARATOR_H__
