
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ANIMATE_FRAME_DUMP_DIALOG_H__
#define __ANIMATE_FRAME_DUMP_DIALOG_H__

#include "FoxTools/Headers/Object.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IFrameDump.h"

namespace FX { class FXTextField; }


namespace Animate {


class FrameDumpDialog : public FX::FXObject
{
  FXDECLARE( FrameDumpDialog  )
public:
  typedef FX::FXObject BaseClass;

  enum
  {
    ID_GET_DIRECTORY = 1,
  };

  FrameDumpDialog (  );

  long onCommandGetDirectory     ( FX::FXObject *, FX::FXSelector, void * );

  Usul::Interfaces::IFrameDump* getFrameDumpProperties( Usul::Interfaces::IUnknown *caller );

private:
  FX::FXTextField *_directory;
};

}


#endif // __ANIMATE_FRAME_DUMP_DIALOG_H__

