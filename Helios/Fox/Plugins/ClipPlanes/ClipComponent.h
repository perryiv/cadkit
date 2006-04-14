
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CLIP_PLANES_COMPONENT_CLASS_H_
#define _CLIP_PLANES_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "FoxTools/Headers/Object.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IFoxAddTab.h"

namespace FX { class FXVerticalFrame; class FXList; }

namespace OsgFox {
namespace Plugins {
namespace ClipPlanes {


class ClipComponent : public Usul::Base::Referenced,
                      public Usul::Interfaces::IPlugin,
                      public Usul::Interfaces::IFoxAddTab,
                      public FX::FXObject

{
  FXDECLARE ( ClipComponent );
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ClipComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  ClipComponent();

  enum
  {
    ID_NEW_CLIP_PLANE = 1,
    ID_DELETE_CLIP_PLANE,
    ID_CLIP_LIST
  };

  long onCommandNewClipPlane     ( FX::FXObject *, FX::FXSelector, void * );
  long onCommandDeleteClipPlane  ( FX::FXObject *, FX::FXSelector, void * );
  long onCommandClipList         ( FX::FXObject *, FX::FXSelector, void * );

  long onUpdateNewClipPlane      ( FX::FXObject *, FX::FXSelector, void * );
  long onUpdateDeleteClipPlane   ( FX::FXObject *, FX::FXSelector, void * );
  long onUpdateClipList          ( FX::FXObject *, FX::FXSelector, void * );

  /// Usul::Interfaces::IPlugin
  virtual std::string getPluginName() const { return "Clipping Planes"; }

  //Usul::Interfaces::FoxAddTab
  virtual void foxAddTab( Usul::Interfaces::IUnknown *caller );

protected:

  // Do not copy.
  ClipComponent ( const ClipComponent & );
  ClipComponent &operator = ( const ClipComponent & );

  /// Use reference counting.
  virtual ~ClipComponent();

  Usul::Interfaces::IUnknown::ValidAccessQueryPtr _caller;

  FX::FXVerticalFrame *_contents;
  FX::FXList          *_list;

};

}; // namespace ClipPlanes
}; // namespace Plugins
}; // namespace OsgFox


#endif // _CLIP_PLANES_COMPONENT_CLASS_H_
