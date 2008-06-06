
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FOXTOOLS_TOOLBAR_FACTORY_H__
#define __FOXTOOLS_TOOLBAR_FACTORY_H__

#include "FoxTools/Export/Export.h"
#include "FoxTools/ToolBar/Bar.h"
#include "FoxTools/ToolBar/DockSite.h"

#include <map>

namespace FoxTools {
namespace ToolBar {

class FOX_TOOLS_EXPORT Factory
{
public:
  //Typedefs
  typedef std::map< std::string, Bar::ValidRefPtr > Toolbars;
  typedef Toolbars::const_iterator ConstIterator;

  // Get the instance
  static Factory& instance();

  // Release the instance
  static void     release();

  enum Site
  {
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
  };

  // Get toolbar by name
  Bar*            toolbar( const std::string& name );

  // Create toolbar at given site
  Bar*            create ( const std::string& name, Site site );

  // Build all toolbars
  void            build();

  // Clear all toolbars
  void            clear();

  // Get the dock sites.  Create them if they don't exist
  DockSite*       topDock();
  DockSite*       bottomDock();
  DockSite*       rightDock();
  DockSite*       leftDock();

  // Hide all
  void            hide();

  // Show all
  void            show();

  // Iterators to the toolbars.
  ConstIterator   begin() const { return _toolbars.begin(); }
  ConstIterator   end() const   { return _toolbars.end(); }

  // Perform layout on the dock sites.
  void            layout();

  // Remove the toolbar with the given name.
  void            remove ( const std::string& );

protected:
  Factory();
  ~Factory();

private:
  
  typedef FoxTools::ToolBar::DockSite::ValidAccessRefPtr DockSitePtr;

  Toolbars _toolbars;

  DockSitePtr _topDock;
  DockSitePtr _bottomDock;
  DockSitePtr _leftDock;
  DockSitePtr _rightDock;

  // It's a singleton
  static Factory* _instance;
};

}
}

#endif // __FOXTOOLS_TOOLBAR_FACTORY_H__

