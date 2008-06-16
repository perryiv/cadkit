
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CLIP_PLANES_COMPONENT_CLASS_H_
#define _CLIP_PLANES_COMPONENT_CLASS_H_

#include "Helios/Qt/Plugins/ClipPlanes/CompileGuard.h"

#include "OsgTools/Configure/OSG.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IPluginInitialize.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IActiveViewListener.h"

class QDockWidget;
class ClipPlanes;

class ClipPlanesComponent : public Usul::Base::Referenced,
                           public Usul::Interfaces::IPluginInitialize,
                           public Usul::Interfaces::IPlugin,
                           public Usul::Interfaces::IActiveViewListener
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ClipPlanesComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  ClipPlanesComponent();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IPlugin
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual std::string getPluginName() const { return "Cross Section Tool Qt"; }

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IPluginInitialize
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void initializePlugin ( Usul::Interfaces::IUnknown *caller = 0x0 );

protected:

  /// The active document has changed.
  virtual void                          activeViewChanged ( Usul::Interfaces::IUnknown *oldView, Usul::Interfaces::IUnknown *newView );

  // Do not copy.
  ClipPlanesComponent ( const ClipPlanesComponent & );
  ClipPlanesComponent &operator = ( const ClipPlanesComponent & );

  /// Use reference counting.
  virtual ~ClipPlanesComponent();

  Usul::Interfaces::IUnknown::QueryPtr _caller;

  QDockWidget     *_dock;
  ClipPlanes      *_clipPlanes;
};


#endif // _CLIP_PLANES_COMPONENT_CLASS_H_
