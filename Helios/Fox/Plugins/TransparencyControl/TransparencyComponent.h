
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

#ifndef _TRANSPARENCY_CONTROL_COMPONENT_CLASS_H_
#define _TRANSPARENCY_CONTROL_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IMenuEntry.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/IHandle.h"


class TransparencyComponent : public Usul::Base::Referenced,
                              public Usul::Interfaces::IPlugin,
                              public Usul::Interfaces::IMenuEntry,
                              public Usul::Interfaces::ICommand,
                              public Usul::Interfaces::IHandle

{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TransparencyComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  TransparencyComponent();


  /// Usul::Interfaces::IPlugin
  virtual std::string getPluginName() const { return "Transparency Control"; }

  ///  Usul::Interfaces::IMenuEntry
  virtual std::string     menuText() const;
  virtual std::string     hotKeyText() const;
  virtual std::string     statusBarText() const;
  virtual std::string     menuGroup() const;

  ///  Usul::Interfaces::ICommand
  virtual void            execute ( Usul::Interfaces::IUnknown *caller );

protected:

  // Do not copy.
  TransparencyComponent ( const TransparencyComponent & );
  TransparencyComponent &operator = ( const TransparencyComponent & );

  /// Use reference counting.
  virtual ~TransparencyComponent();

  /// Usul::Interfaces::IHandle
  virtual bool doesHandle( Usul::Interfaces::IUnknown* );


};



#endif // _TRANSPARENCY_CONTROL_COMPONENT_CLASS_H_
