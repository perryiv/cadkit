
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_KIT_UPDATE_VISITOR_H__
#define __MENU_KIT_UPDATE_VISITOR_H__

#include "MenuKit/Visitor.h"

namespace MenuKit {

class MENUKIT_EXPORT UpdateVisitor : public MenuKit::Visitor
{
public:
  typedef MenuKit::Visitor BaseClass;
  USUL_DECLARE_REF_POINTERS ( UpdateVisitor );

  UpdateVisitor ();

  // Apply this visitor to the items.
  virtual void        apply ( Menu &m );
  virtual void        apply ( Button &b );

protected:
  virtual ~UpdateVisitor ();
};

}

#endif // __MENU_KIT_UPDATE_VISITOR_H__
