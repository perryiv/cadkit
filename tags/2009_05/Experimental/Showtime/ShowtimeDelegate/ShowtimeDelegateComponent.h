
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

#include "Showtime/ShowtimeDelegate/CompileGuard.h"

#include "Usul/Base/Object.h"
#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IDefaultGUIDelegate.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"

#include <string>


class ShowtimeDelegateComponent : public Usul::Base::Object,
                                  public Usul::Interfaces::IPlugin,
                                  public Usul::Interfaces::IDefaultGUIDelegate,
                                  public Usul::Interfaces::IGUIDelegate
{
public:

  /// Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ShowtimeDelegateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  ShowtimeDelegateComponent();

  /// Usul::Interfaces::IGUIDelegate
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Showtime Delegate"; }

  /// Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle ( const std::string& token ) const;

protected: 

  // Do not copy.
  ShowtimeDelegateComponent ( const ShowtimeDelegateComponent & );
  ShowtimeDelegateComponent &operator = ( const ShowtimeDelegateComponent & );

  /// Use reference counting.
  virtual ~ShowtimeDelegateComponent();
};


#endif // _SHOW_TIME_MODEL_COMPONENT_CLASS_H_
