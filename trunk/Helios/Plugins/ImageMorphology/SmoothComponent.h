
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

#ifndef _SMOOTH_IMAGE_COMPONENT_CLASS_H_
#define _SMOOTH_IMAGE_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IMenuEntry.h"
#include "Usul/Interfaces/ICommand.h"



class SmoothComponent : public Usul::Base::Referenced,
                           public Usul::Interfaces::IPlugin,
                           public Usul::Interfaces::IMenuEntry,
                           public Usul::Interfaces::ICommand

{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SmoothComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  SmoothComponent();


  /// Usul::Interfaces::IPlugin
  virtual std::string getPluginName() const { return "Smooth Image"; }

  ///  Usul::Interfaces::IMenuEntry
  virtual std::string     menuText() const;
  virtual std::string     hotKeyText() const;
  virtual std::string     statusBarText() const;
  virtual std::string     menuGroup() const;

  ///  Usul::Interfaces::ICommand
  virtual void            execute ( Usul::Interfaces::IUnknown *caller );



protected:

  // Do not copy.
  SmoothComponent ( const SmoothComponent & );
  SmoothComponent &operator = ( const SmoothComponent & );

  /// Use reference counting.
  virtual ~SmoothComponent();


};



#endif // _SMOOTH_IMAGE_COMPONENT_CLASS_H_
