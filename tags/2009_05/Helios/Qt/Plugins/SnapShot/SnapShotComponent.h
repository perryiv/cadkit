
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

#ifndef _SNAP_SHOT_COMPONENT_CLASS_H_
#define _SNAP_SHOT_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IPluginInitialize.h"
#include "Usul/Interfaces/IActiveViewListener.h"


class QDockWidget;


class SnapShotComponent : public Usul::Base::Referenced,
                           public Usul::Interfaces::IPluginInitialize,
                           public Usul::Interfaces::IPlugin,
                           public Usul::Interfaces::IActiveViewListener
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SnapShotComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  SnapShotComponent();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IPlugin
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual std::string getPluginName() const { return "Snap Shot Qt"; }

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IFoxAddTab
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void initializePlugin ( Usul::Interfaces::IUnknown *caller = 0x0 );

protected:

  /// The active document has changed.
  virtual void                          activeViewChanged ( Usul::Interfaces::IUnknown *oldView, Usul::Interfaces::IUnknown *newView );

  // Do not copy.
  SnapShotComponent ( const SnapShotComponent & );
  SnapShotComponent &operator = ( const SnapShotComponent & );

  /// Use reference counting.
  virtual ~SnapShotComponent();

  Usul::Interfaces::IUnknown::QueryPtr _caller;

  QDockWidget     *_dock;
};


#endif // _SNAP_SHOT_COMPONENT_CLASS_H_
