
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

#if 0
inline int MatrixDialog( FX::FXWindow* owner, FX::FXMatrix *matrix, const std::string& title, int layout )
{
  FX::FXDialogBox dialog ( owner, title.c_str(), layout ,0,0,0,0, 10,10,10,10, 4,4 );

  FX::FXVerticalFrame* theFrame ( new FX::FXVerticalFrame ( &dialog, LAYOUT_FILL_X|LAYOUT_FILL_Y ) );

  matrix->reparent( theFrame );

  //Accept and cancel buttons
  FX::FXHorizontalFrame*  buttonFrame ( new FX::FXHorizontalFrame ( theFrame, LAYOUT_FILL_X|LAYOUT_FILL_Y ) );
  FX::FXButton *accept ( new FX::FXButton ( buttonFrame, "Accept", 0x0, &dialog, FX::FXDialogBox::ID_ACCEPT,FX::LAYOUT_LEFT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20) );
  FX::FXButton *cancel ( new FX::FXButton ( buttonFrame, "Cancel", 0x0, &dialog, FX::FXDialogBox::ID_CANCEL,FX::LAYOUT_RIGHT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20) );

  dialog.create();

  return dialog.execute( FX::PLACEMENT_OWNER );
}
#endif

};
};

#endif
