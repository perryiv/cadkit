
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ANIMATION_CONTROL_COMPONENT_CLASS_H_
#define _ANIMATION_CONTROL_COMPONENT_CLASS_H_

#include "Helios/Qt/Plugins/AnimationControl/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IPluginInitialize.h"
#include "Usul/Interfaces/IPlugin.h"

namespace QtTools { class AnimationControl; }

class QDockWidget;

class AnimationControlComponent : public Usul::Base::Referenced,
                                  public Usul::Interfaces::IPluginInitialize,
                                  public Usul::Interfaces::IPlugin
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( AnimationControlComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  AnimationControlComponent();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IPlugin
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual std::string getPluginName() const { return "Animation Control Qt"; }

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IPluginInitialize
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void initializePlugin ( Usul::Interfaces::IUnknown *caller = 0x0 );

protected:

  // Do not copy.
  AnimationControlComponent ( const AnimationControlComponent & );
  AnimationControlComponent &operator = ( const AnimationControlComponent & );

  /// Use reference counting.
  virtual ~AnimationControlComponent();

  Usul::Interfaces::IUnknown::QueryPtr _caller;

  QDockWidget     *_dock;
	QtTools::AnimationControl *_animation;
};


#endif // _ANIMATION_CONTROL_COMPONENT_CLASS_H_
