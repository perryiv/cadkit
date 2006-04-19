
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

#ifndef _VOLUME_DELEGATE_COMPONENT_CLASS_H_
#define _VOLUME_DELEGATE_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"
#include "Usul/Interfaces/IGetOptions.h"

#include <string>


class VolumeDelegateComponent : public Usul::Base::Referenced,
                                public Usul::Interfaces::IPlugin,
                                public Usul::Interfaces::IGUIDelegate,
                                public Usul::Interfaces::IGetOptions
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( VolumeDelegateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  VolumeDelegateComponent();

protected: 

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Volume Delegate"; }

  /// Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle( const std::string& token ) const;
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );
  virtual void                refreshView      ( Usul::Documents::Document *document, Usul::Interfaces::IViewer  *viewer );

  // Usul::Interfaces::IGetOptions
  virtual void                getOptions ( Options& );

  // Do not copy.
  VolumeDelegateComponent ( const VolumeDelegateComponent & );
  VolumeDelegateComponent &operator = ( const VolumeDelegateComponent & );

  /// Use reference counting.
  virtual ~VolumeDelegateComponent();


};


#endif // _VOLUME_DELEGATE_COMPONENT_CLASS_H_
