
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FOX_TOOLS_TOOLBAR_BAR_H__
#define __FOX_TOOLS_TOOLBAR_BAR_H__

#include "FoxTools/Export/Export.h"
#include "FoxTools/Items/Item.h"
#include "FoxTools/Items/Command.h"

#include "FoxTools/Headers/ToolBar.h"

namespace FX { class FXComposite; }

#include <vector>

namespace FoxTools {
namespace ToolBar {

class FOX_TOOLS_EXPORT Bar : public FoxTools::Items::Item
{
public:
  typedef FoxTools::Items::Item               BaseClass;
  typedef FoxTools::Items::Command            Command;

  typedef std::vector<FoxTools::Items::Command::ValidRefPtr> Commands;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Bar );
  
  Bar ( FX::FXComposite *dockedSite, FX::FXComposite *undockedSite );
  Bar ( const std::string& name, FX::FXComposite *dockedSite, FX::FXComposite *undockedSite );

  // Append a command
  void            append ( Command* command );

  // Build
  void            build ( bool shown = true,
                          unsigned int layout = FX::LAYOUT_CENTER_X | FX::LAYOUT_CENTER_Y | FX::FRAME_RAISED | FX::LAYOUT_DOCK_SAME, 
                          int x = 0, 
                          int y = 0, 
                          int width = 0,
                          int height = 0 );

  // Clear
  virtual void    clear();

  // Create
  void            create();

  // Find command by name
  Command*        find ( const std::string& );

  // Is this toolbar showing
  bool            shown() const;
  
  void            show();
  void            hide();

protected:
  // Use reference counting
  virtual ~Bar();

private:

  Commands _commands;
  FX::FXComposite *_dockedSite;
  FX::FXComposite *_undockedSite;
  FX::FXToolBar   *_bar;


};

}
}

#endif // __FOX_TOOLS_TOOLBAR_BAR_H__
