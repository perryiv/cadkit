
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

#ifndef _FIELD_VIEW_READER_DELEGATE_COMPONENT_CLASS_H_
#define _FIELD_VIEW_READER_DELEGATE_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"

#include <string>


class P3D2FVDelegateComponent : public Usul::Base::Referenced,
                               public Usul::Interfaces::IPlugin,
                               public Usul::Interfaces::IGUIDelegate
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( P3D2FVDelegateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  P3D2FVDelegateComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Plot3D to FieldView Converter Delegate"; }

  /// Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle( const std::string& token ) const;
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );

protected: 

  // Do not copy.
  P3D2FVDelegateComponent ( const P3D2FVDelegateComponent & );
  P3D2FVDelegateComponent &operator = ( const P3D2FVDelegateComponent & );

  /// Use reference counting.
  virtual ~P3D2FVDelegateComponent();


};


#endif // _FIELD_VIEW_READER_DELEGATE_COMPONENT_CLASS_H_
