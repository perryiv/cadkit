
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_KIT_COMMAND_VISITOR_H__
#define __MENU_KIT_COMMAND_VISITOR_H__

#include "MenuKit/Visitor.h"

#include <string>
#include <vector>

namespace MenuKit {


class MENUKIT_EXPORT CommandVisitor : public MenuKit::Visitor
{
public:
  typedef MenuKit::Visitor BaseClass;
  USUL_DECLARE_REF_POINTERS ( CommandVisitor );

  CommandVisitor ( const std::string &remove = std::string() );

  // Apply this visitor to the items.
  virtual void        apply ( Menu &m );
  virtual void        apply ( Button &b );
  //virtual void        apply ( ToggleButton &toggle );
  //virtual void        apply ( RadioButton &radio );

protected:
  virtual ~CommandVisitor ();

private:
  std::vector< std::string > _names;
  std::string _remove;
};


}


#endif // __MENU_KIT_UPDATE_VISITOR_H__
