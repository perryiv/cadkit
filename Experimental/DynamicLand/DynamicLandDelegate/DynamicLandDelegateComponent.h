
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

#ifndef _DYNAMIC_LAND_DELEGATE_MODEL_COMPONENT_CLASS_H_
#define _DYNAMIC_LAND_DELEGATE_MODEL_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"

#include <string>


class DynamicLandDelegateComponent : public Usul::Base::Referenced,
                               public Usul::Interfaces::IPlugin,
                               public Usul::Interfaces::IGUIDelegate
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DynamicLandDelegateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  DynamicLandDelegateComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Dynamic Land Delegate"; }

  /// Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle( const std::string& token ) const;
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );

protected: 

  // Do not copy.
  DynamicLandDelegateComponent ( const DynamicLandDelegateComponent & );
  DynamicLandDelegateComponent &operator = ( const DynamicLandDelegateComponent & );

  /// Use reference counting.
  virtual ~DynamicLandDelegateComponent();


};


#endif // _DYNAMIC_LAND_DELEGATE_MODEL_COMPONENT_CLASS_H_
