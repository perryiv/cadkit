
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FRAME_TAB_H__
#define __FRAME_TAB_H__

#include "CompileGuard.h"

#include "Usul/Interfaces/IUnknown.h"

#include "FoxTools/Headers/Object.h"

namespace FX { class FXVerticalFrame; }

class FrameTab : public FX::FXObject
{
public:

  enum
  {
    ID_CENTER_X = 1,
    ID_CENTER_Y,
    ID_CENTER_Z,
    ID_DISTANCE,
    ID_ROTATION_X,
    ID_ROTATION_Y,
    ID_ROTATION_Z,
    ID_ROTATION_W,
    ID_WIDTH_CHANGED,
    ID_HEIGHT_CHANGED,
    ID_EXPORT_IMAGE,
    ID_LAST
  };

  FrameTab();
  ~FrameTab();

  void initialize ( Usul::Interfaces::IUnknown *caller );

  long onCommandCenterX   ( FX::FXObject *, FX::FXSelector, void * );
  long onCommandCenterY   ( FX::FXObject *, FX::FXSelector, void * );
  long onCommandCenterZ   ( FX::FXObject *, FX::FXSelector, void * );
  long onCommandDistance   ( FX::FXObject *, FX::FXSelector, void * );
  long onCommandRotationX   ( FX::FXObject *, FX::FXSelector, void * );
  long onCommandRotationY   ( FX::FXObject *, FX::FXSelector, void * );
  long onCommandRotationZ   ( FX::FXObject *, FX::FXSelector, void * );
  long onCommandRotationW   ( FX::FXObject *, FX::FXSelector, void * );

  long onChangedHeight ( FX::FXObject *, FX::FXSelector, void * );
  long onChangedWidght ( FX::FXObject *, FX::FXSelector, void * );

  long onCommandExportImage ( FX::FXObject *, FX::FXSelector, void * );
  long onUpdateExportImage  ( FX::FXObject *, FX::FXSelector, void * );

  long onUpdateCenterX   ( FX::FXObject *, FX::FXSelector, void * );
  long onUpdateCenterY   ( FX::FXObject *, FX::FXSelector, void * );
  long onUpdateCenterZ   ( FX::FXObject *, FX::FXSelector, void * );
  long onUpdateDistance   ( FX::FXObject *, FX::FXSelector, void * );
  long onUpdateRotationX   ( FX::FXObject *, FX::FXSelector, void * );
  long onUpdateRotationY   ( FX::FXObject *, FX::FXSelector, void * );
  long onUpdateRotationZ   ( FX::FXObject *, FX::FXSelector, void * );
  long onUpdateRotationW   ( FX::FXObject *, FX::FXSelector, void * );

private:
  Usul::Interfaces::IUnknown::QueryPtr _caller;
  float _centerIncrement;
  float _distanceIncrement;
  float _rotationIncrement;

  unsigned int _height;
  unsigned int _width;

  FX::FXVerticalFrame *_contents;

  FXDECLARE( FrameTab )
};


#endif // __FRAME_TAB_H__
