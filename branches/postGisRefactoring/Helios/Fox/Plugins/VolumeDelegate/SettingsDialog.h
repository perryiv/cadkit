
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VOLUME_DOCUMENT_SETTINGS_DIALOG_H__
#define __VOLUME_DOCUMENT_SETTINGS_DIALOG_H__

#include "FoxTools/Headers/DialogBox.h"

#include <map>

class SettingsDialog : public FX::FXDialogBox
{
  FXDECLARE( SettingsDialog );
public:
  typedef FX::FXDialogBox BaseClass;

  enum
  {
    ID_2D_TEXTURE = BaseClass::ID_LAST,
    ID_3D_TEXTURE,
    ID_VOXELS,
    ID_LAST
  };

  enum Mode
  {
    TEXTURE_2D,
    TEXTURE_3D,
    VOXELS
  };

  SettingsDialog ( );
  virtual ~SettingsDialog();

  long                onCommand2DTexture  ( FX::FXObject*, FX::FXSelector, void* );
  long                onUpdate2DTexture   ( FX::FXObject*, FX::FXSelector, void* );

  long                onCommand3DTexture  ( FX::FXObject*, FX::FXSelector, void* );
  long                onUpdate3DTexture   ( FX::FXObject*, FX::FXSelector, void* );

  long                onCommandVoxels     ( FX::FXObject*, FX::FXSelector, void* );
  long                onUpdateVoxels      ( FX::FXObject*, FX::FXSelector, void* );

  template< class Options > 
  void                options ( Options& opts )
  {
    switch ( _mode )
    {
    case TEXTURE_2D:
      opts["TextureType"] = "2DTexture";
      break;
    case TEXTURE_3D:
      opts["TextureType"] = "3DTexture";
      break;
    case VOXELS:
      opts["TextureType"] = "Voxels";
      break;
    };      
  }

private:

  Mode _mode;
};


#endif // __VOLUME_DOCUMENT_SETTINGS_DIALOG_H__
