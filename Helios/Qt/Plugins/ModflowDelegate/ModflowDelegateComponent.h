
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_DELEGATE_COMPONENT_CLASS_H_
#define _MODFLOW_DELEGATE_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"

#include <string>


class ModflowDelegateComponent : public Usul::Base::Object,
                                 public Usul::Interfaces::IPlugin,
                                 public Usul::Interfaces::IGUIDelegate
{
public:

  /// Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ModflowDelegateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  ModflowDelegateComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Modflow Delegate"; }

  /// Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle ( const std::string& token ) const;
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );

protected: 

  // Do not copy.
  ModflowDelegateComponent ( const ModflowDelegateComponent & );
  ModflowDelegateComponent &operator = ( const ModflowDelegateComponent & );

  /// Use reference counting.
  virtual ~ModflowDelegateComponent();


};


#endif // _MODFLOW_DELEGATE_COMPONENT_CLASS_H_
