
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Jeff Conner
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
#include "VaporIntrusionGUIViewer.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Shared/Preferences.h"
#include "Usul/Registry/Constants.h"

#include "Helios/Qt/Views/OSG/Viewer.h"
#include "Helios/Qt/Views/OSG/Format.h"

#include <string>


class VaporIntrusionGUIDelegateComponent : public Usul::Base::Referenced,
                               public Usul::Interfaces::IPlugin,
                               public Usul::Interfaces::IGUIDelegate,
                               public Usul::Interfaces::IMenuAdd 
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef VaporIntrusionGUIViewer QtViewer;

  // Non-ref'ing smart-pointers that throw if given null.
  typedef Usul::Pointers::Configs::NoRefCountingNullOk Policy;
  typedef Usul::Pointers::SmartPointer < QtViewer, Policy > QtViewerPtr;


  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( VaporIntrusionGUIDelegateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  VaporIntrusionGUIDelegateComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Vapor Intrusion GUI Delegate"; }

  /// Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle( const std::string& token ) const;
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );

  //Usul::Interfaces::IMenuAdd
  virtual void                menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );

  void                        tile( Usul::Interfaces::IUnknown* caller );

protected: 

  // Do not copy.
  VaporIntrusionGUIDelegateComponent ( const VaporIntrusionGUIDelegateComponent & );
  VaporIntrusionGUIDelegateComponent &operator = ( const VaporIntrusionGUIDelegateComponent & );

  /// Use reference counting.
  virtual ~VaporIntrusionGUIDelegateComponent();

private:
  QtViewerPtr xyzView;
  QtViewerPtr xyView;
  QtViewerPtr xzView;
  QtViewerPtr yzView;


};


#endif // _FIELD_VIEW_READER_DELEGATE_COMPONENT_CLASS_H_
