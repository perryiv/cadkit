
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

#ifndef _IMAGE_DELEGATE_COMPONENT_CLASS_H_
#define _IMAGE_DELEGATE_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IGUIDelegate.h"
#include "Usul/Interfaces/IHandleActivatingDocument.h"

#include <string>


class ImageDelegateComponent : public Usul::Base::Referenced,
                               public Usul::Interfaces::IPlugin,
                               public Usul::Interfaces::IGUIDelegate,
                               public Usul::Interfaces::IHandleActivatingDocument
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ImageDelegateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  ImageDelegateComponent();

protected: 
   /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Image Delegate"; }

  /// Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle( const std::string& token ) const;
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );
  virtual void                refreshView      ( Usul::Documents::Document *document, Usul::Interfaces::IViewer  *viewer );

  /// Usul::Interfaces::IHandleActivatingDocument
  virtual void                noLongerActive ( Usul::Documents::Document* document, const std::string& activeType );
  virtual void                nowActive      ( Usul::Documents::Document* document, const std::string& oldType );


  // Do not copy.
  ImageDelegateComponent ( const ImageDelegateComponent & );
  ImageDelegateComponent &operator = ( const ImageDelegateComponent & );

  /// Use reference counting.
  virtual ~ImageDelegateComponent();


};


#endif // _IMAGE_DELEGATE_COMPONENT_CLASS_H_
