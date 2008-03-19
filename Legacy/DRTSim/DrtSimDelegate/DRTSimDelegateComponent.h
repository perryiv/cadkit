
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DRT_SIM_DELEGATE_MODEL_COMPONENT_CLASS_H_
#define _DRT_SIM_DELEGATE_MODEL_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"

#include <string>


class DRTSimDelegateComponent : public Usul::Base::Referenced,
                               public Usul::Interfaces::IPlugin,
                               public Usul::Interfaces::IGUIDelegate
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DRTSimDelegateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  DRTSimDelegateComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Disaster Response Testbed Simulation Delegate"; }

  /// Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle( const std::string& token ) const;
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );

protected: 

  // Do not copy.
  DRTSimDelegateComponent ( const DRTSimDelegateComponent & );
  DRTSimDelegateComponent &operator = ( const DRTSimDelegateComponent & );

  /// Use reference counting.
  virtual ~DRTSimDelegateComponent();


};


#endif // _DRT_SIM_DELEGATE_MODEL_COMPONENT_CLASS_H_
