
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FOXTOOLS_DIALOGS_H__
#define __FOXTOOLS_DIALOGS_H__

#include "FoxTools/Headers/Window.h"
#include "FoxTools/Headers/MessageBox.h"

#include <sstream>
#include <string>

namespace FoxTools {
namespace Dialogs {


inline int SaveDialog( FX::FXWindow* owner, const std::string& filename )
{
  std::ostringstream os;
  os << filename << " is not saved.  Do you want to save?";

  return FX::FXMessageBox::warning ( owner, FX::MBOX_YES_NO_CANCEL, "Save File?", os.str().c_str() );
}

};
};

#endif
