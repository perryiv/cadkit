
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

#ifndef _SHOW_TIME_MODEL_COMPONENT_CLASS_H_
#define _SHOW_TIME_MODEL_COMPONENT_CLASS_H_

#include "ShowtimeDelegate/CompileGuard.h"

#include "Helios/Qt/Views/OSG/Delegate.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDefaultGUIDelegate.h"

#include <string>


class ShowtimeDelegateComponent : public CadKit::Helios::Views::OSG::Delegate,
                               public Usul::Interfaces::IPlugin,
                               public Usul::Interfaces::IDefaultGUIDelegate
{
public:

  /// Typedefs.
  typedef CadKit::Helios::Views::OSG::Delegate BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ShowtimeDelegateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  ShowtimeDelegateComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Showtime Delegate"; }

  /// Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle( const std::string& token ) const;

protected: 

  // Do not copy.
  ShowtimeDelegateComponent ( const ShowtimeDelegateComponent & );
  ShowtimeDelegateComponent &operator = ( const ShowtimeDelegateComponent & );

  /// Use reference counting.
  virtual ~ShowtimeDelegateComponent();
};


#endif // _SHOW_TIME_MODEL_COMPONENT_CLASS_H_
