
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

#ifndef _FLASH_DELEGATEL_COMPONENT_CLASS_H_
#define _FLASH_DELEGATEL_COMPONENT_CLASS_H_

#include "Flash/FlashDelegate/CompileGuard.h"

#include "Helios/Qt/Views/OSG/Delegate.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IMenuAdd.h"

#include <string>


class FlashDelegateComponent : public CadKit::Helios::Views::OSG::Delegate,
                               public Usul::Interfaces::IPlugin,
                               public Usul::Interfaces::IMenuAdd
{
public:

  /// Typedefs.
  typedef CadKit::Helios::Views::OSG::Delegate BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( FlashDelegateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  FlashDelegateComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string        getPluginName() const { return "Flash Delegate"; }

  /// Usul::Interfaces::IGUIDelegate
  virtual bool               doesHandle( const std::string& token ) const;
  
  /// Add to the menu. (IMenuAdd).
  virtual void               menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown* caller = 0x0 );

protected: 
 
  // Do not copy.
  FlashDelegateComponent ( const FlashDelegateComponent & );
  FlashDelegateComponent &operator = ( const FlashDelegateComponent & );

  /// Use reference counting.
  virtual ~FlashDelegateComponent();
};


#endif // _FLASH_DELEGATEL_COMPONENT_CLASS_H_
