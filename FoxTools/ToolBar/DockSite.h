
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FOXTOOLS_TOOLBAR_DOCK_SITE_H__
#define __FOXTOOLS_TOOLBAR_DOCK_SITE_H__

#include "FoxTools/Export/Export.h"
#include "FoxTools/Items/Item.h"
#include "FoxTools/ToolBar/Bar.h"

namespace FX { class FXDockSite; class FXComposite; }

#include <vector>

namespace FoxTools {
namespace ToolBar {

class FOX_TOOLS_EXPORT DockSite : public FoxTools::Items::Item
{
public:
  typedef FoxTools::Items::Item               BaseClass;
  typedef FoxTools::ToolBar::Bar::ValidRefPtr BarPtr;
  typedef std::vector< BarPtr >               Bars;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DockSite );

  DockSite ( FX::FXComposite *parent, unsigned int layout );

  FX::FXDockSite*   dockSite();

  // Append a toolbar
  void              append( Bar* );

  // Build all toolbars
  void              build();

  // Clear
  void              clear();

  // Hide
  void              hide();

  // Show
  void              show();

protected:

  // Use reference counting.
  virtual ~DockSite ();

private:

  // No copying.
  DockSite ( const DockSite & );
  DockSite &operator = ( const DockSite & );

  Bars _bars;
  FX::FXDockSite* _docksite;

};

}
}

#endif // __FOXTOOLS_TOOLBAR_DOCK_SITE_H__

