
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Recent-file buttons.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_MENU_RECENT_FILES_CLASS_H_
#define _FOX_TOOLS_MENU_RECENT_FILES_CLASS_H_

#include "FoxTools/Items/Command.h"

#include <list>

namespace FX { class FXRecentFiles; class FXMenuCommand; }


namespace FoxTools {
namespace Menu {

class Group;

class FOX_TOOLS_EXPORT RecentFiles : public FoxTools::Items::Command
{
public:

  // Typedefs.
  typedef FoxTools::Items::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( RecentFiles );

  // Constructors.
  RecentFiles ( unsigned int token = 0 );
  RecentFiles ( const std::string &name, FX::FXObject *target, unsigned int selector, unsigned int token = 0 );

  // Append a file to the list.
  void                  appendFile ( const std::string & );

  // Clear the button.
  virtual void          clear();

protected:

  // Use reference counting.
  virtual ~RecentFiles();

  // Build the button.
  virtual void          _build ( FX::FXComposite *parent );

  // Create
  virtual void          _create ();

private:

  // No copying.
  RecentFiles ( const RecentFiles & );
  RecentFiles &operator = ( const RecentFiles & );

  friend class Group;
  typedef std::list<FX::FXMenuCommand*> CommandList;

  FX::FXRecentFiles *_recentFiles;
  CommandList _commands;
};


}; // namespace Menu
}; // namespace FoxTools


#endif // _FOX_TOOLS_MENU_RECENT_FILES_CLASS_H_
