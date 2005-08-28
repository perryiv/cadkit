
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FOX_TOOLS_TOOLBAR_SEPARATOR_H__
#define __FOX_TOOLS_TOOLBAR_SEPARATOR_H__

#include "FoxTools/Export/Export.h"
#include "FoxTools/Items/Command.h"

namespace FX { class FXComposite; class FXSeparator; }

namespace FoxTools {
namespace ToolBar {


class FOX_TOOLS_EXPORT Separator : public FoxTools::Items::Command
{
public:
  typedef FoxTools::Items::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Separator );

  // Constructors.
  Separator ( unsigned int token = 0 );

  // Clear
  virtual void clear();

protected:

  // Use reference counting
  virtual ~Separator();

  virtual void          _build ( FX::FXComposite *parent );

  // Create
  virtual void          _create ();

private:

  // No copying.
  Separator ( const Separator & );
  Separator &operator= ( const Separator & );

  FX::FXSeparator* _separator;

};


}
}

#endif // __FOX_TOOLS_TOOLBAR_SEPARATOR_H__

