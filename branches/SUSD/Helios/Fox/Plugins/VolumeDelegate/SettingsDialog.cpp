
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "SettingsDialog.h"

#include "FoxTools/Functions/Check.h"
#include "FoxTools/Functions/MainWindow.h"

#include "FoxTools/Headers/DialogBox.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/HorizontalFrame.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/GroupBox.h"
#include "FoxTools/Headers/RadioButton.h"
#include "FoxTools/Headers/List.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( SettingsDialog ) MessageMap[] = 
{
  //          Message Type      ID                                     Message Handler.
  FXMAPFUNC ( FX::SEL_COMMAND,  SettingsDialog::ID_2D_TEXTURE,       SettingsDialog::onCommand2DTexture   ),
  FXMAPFUNC ( FX::SEL_UPDATE,   SettingsDialog::ID_2D_TEXTURE,       SettingsDialog::onUpdate2DTexture    ),
  FXMAPFUNC ( FX::SEL_COMMAND,  SettingsDialog::ID_3D_TEXTURE,       SettingsDialog::onCommand3DTexture   ),
  FXMAPFUNC ( FX::SEL_UPDATE,   SettingsDialog::ID_3D_TEXTURE,       SettingsDialog::onUpdate3DTexture    ),
  FXMAPFUNC ( FX::SEL_COMMAND,  SettingsDialog::ID_VOXELS,           SettingsDialog::onCommandVoxels      ),
  FXMAPFUNC ( FX::SEL_UPDATE,   SettingsDialog::ID_VOXELS,           SettingsDialog::onUpdateVoxels       ),
};

FOX_TOOLS_IMPLEMENT ( SettingsDialog, SettingsDialog::BaseClass, MessageMap, ARRAYNUMBER ( MessageMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

SettingsDialog::SettingsDialog( ) : 
  BaseClass ( ( FX::FXWindow*) FoxTools::Functions::mainWindow(), "New Volume Document", FX::DECOR_CLOSE|DECOR_TITLE|DECOR_BORDER ),
  _mode ( TEXTURE_2D )
{
  FX::FXVerticalFrame* theFrame ( new FX::FXVerticalFrame ( this, LAYOUT_FILL_X|LAYOUT_FILL_Y ) );
  FX::FXHorizontalFrame* hFrame ( new FX::FXHorizontalFrame ( theFrame, LAYOUT_FILL_X ) );
  FX::FXVerticalFrame* leftFrame ( new FX::FXVerticalFrame ( hFrame, LAYOUT_FILL_Y ) );

  // File list
  FX::FXGroupBox *listGroup ( new FX::FXGroupBox ( leftFrame, "Files", GROUPBOX_TITLE_LEFT|FRAME_RIDGE|LAYOUT_FILL_X|LAYOUT_FILL_Y ) ); 

  // Add and remove buttons
  FX::FXHorizontalFrame *bFrame ( new FX::FXHorizontalFrame ( listGroup, LAYOUT_FILL_X ) );
  new FX::FXButton ( bFrame, "Add..." );
  new FX::FXButton ( bFrame, "Remove" );

  FX::FXList *list ( new FX::FXList ( listGroup, 0x0, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y  ) );

  FX::FXVerticalFrame* rightFrame ( new FX::FXVerticalFrame ( hFrame, LAYOUT_FILL_Y ) );

  // Texture type
  FX::FXGroupBox *renderOptions ( new FX::FXGroupBox( rightFrame, "Render Options", GROUPBOX_TITLE_LEFT|FRAME_RIDGE|LAYOUT_FILL_X ) );

  new FX::FXRadioButton ( renderOptions, "2D Texture", this, SettingsDialog::ID_2D_TEXTURE );
  new FX::FXRadioButton ( renderOptions, "3D Texture", this, SettingsDialog::ID_3D_TEXTURE );
  new FX::FXRadioButton ( renderOptions, "Voxels",     this, SettingsDialog::ID_VOXELS     );

  // Texture mode
  FX::FXGroupBox *modeGroup ( new FX::FXGroupBox( rightFrame, "Texture Mode", GROUPBOX_TITLE_LEFT|FRAME_RIDGE|LAYOUT_FILL_X ) );

  new FX::FXRadioButton ( modeGroup, "Decal",    0x0, 0 );
  new FX::FXRadioButton ( modeGroup, "Modulate", 0x0, 0 );
  new FX::FXRadioButton ( modeGroup, "Blend",    0x0, 0 );
  new FX::FXRadioButton ( modeGroup, "Replace",  0x0, 0 );
  new FX::FXRadioButton ( modeGroup, "Add",      0x0, 0 );

  //Accept and cancel buttons
  FX::FXHorizontalFrame*  buttonFrame ( new FX::FXHorizontalFrame ( theFrame, LAYOUT_FILL_X|LAYOUT_FILL_Y ) );
  const unsigned int layout ( BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK );
  FX::FXButton *button ( new FX::FXButton ( buttonFrame, "Accept", 0x0, this, SettingsDialog::ID_ACCEPT, FX::LAYOUT_LEFT | layout ) );
  new FX::FXButton ( buttonFrame, "Cancel", 0x0, this, SettingsDialog::ID_CANCEL, FX::LAYOUT_RIGHT | layout );
  button->setFocus();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

SettingsDialog::~SettingsDialog()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Change texture type
//
///////////////////////////////////////////////////////////////////////////////

long SettingsDialog::onCommand2DTexture  ( FX::FXObject *object, FX::FXSelector, void* )
{
  _mode = TEXTURE_2D;

  //Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update texture radio button
//
///////////////////////////////////////////////////////////////////////////////

long SettingsDialog::onUpdate2DTexture  ( FX::FXObject *object, FX::FXSelector, void* )
{
  FoxTools::Functions::check  ( _mode == TEXTURE_2D, object );

  //Handled
  return 1;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Change texture type
//
///////////////////////////////////////////////////////////////////////////////

long SettingsDialog::onCommand3DTexture  ( FX::FXObject *object, FX::FXSelector, void* )
{
  _mode = TEXTURE_3D;

  //Handled
  return 1;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Update texture radio button
//
///////////////////////////////////////////////////////////////////////////////

long SettingsDialog::onUpdate3DTexture  ( FX::FXObject *object, FX::FXSelector, void* )
{
  FoxTools::Functions::check  ( _mode == TEXTURE_3D, object );

  //Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Change texture type
//
///////////////////////////////////////////////////////////////////////////////

long SettingsDialog::onCommandVoxels  ( FX::FXObject *object, FX::FXSelector, void* )
{
  _mode = VOXELS;

  //Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update texture radio button
//
///////////////////////////////////////////////////////////////////////////////

long SettingsDialog::onUpdateVoxels  ( FX::FXObject *object, FX::FXSelector, void* )
{
  FoxTools::Functions::check  ( _mode == VOXELS, object );

  //Handled
  return 1;
}



