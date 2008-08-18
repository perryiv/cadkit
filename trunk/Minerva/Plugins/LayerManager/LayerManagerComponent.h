
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  This plugin needs to be renamed.  It has become the Minerva delegate.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _LAYER_MANAGER_COMPONENT_H_
#define _LAYER_MANAGER_COMPONENT_H_

#include "Minerva/Plugins/LayerManager/CompileGuard.h"

#include "Helios/Qt/Views/OSG/Delegate.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IActiveDocumentListener.h"
#include "Usul/Interfaces/IModifiedObserver.h"
#include "Usul/Interfaces/IPluginInitialize.h"
#include "Usul/Interfaces/IDefaultGUIDelegate.h"

#include <vector>

class QDockWidget;
class Favorites;

namespace Minerva { class LayersTree; }

class LayerManagerComponent : public CadKit::Helios::Views::OSG::Delegate,
                              public Usul::Interfaces::IPlugin,
                              public Usul::Interfaces::IActiveDocumentListener,
                              public Usul::Interfaces::IModifiedObserver,
                              public Usul::Interfaces::IPluginInitialize,
                              public Usul::Interfaces::IDefaultGUIDelegate
{
public:

  /// Typedefs.
  typedef CadKit::Helios::Views::OSG::Delegate   BaseClass;
  typedef Usul::Interfaces::IUnknown             Unknown;
  typedef std::vector<QDockWidget*>              Docks;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( LayerManagerComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor
  LayerManagerComponent();
  
protected:

  /// Use reference counting.
  virtual ~LayerManagerComponent();
  
  /// Return name of plugin.
  virtual std::string                   getPluginName() const;

  /// Usul::Interfaces::IGUIDelegate
  virtual void                          createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );
  
  /// Usul::Interfaces::IGUIDelegate
  virtual bool                          doesHandle( const std::string& token ) const;
  
  /// The active document has changed (IActiveDocumentListener).
  virtual void                          activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc );

  /// The document is modified.
  virtual void                          subjectModified ( Usul::Interfaces::IUnknown *caller = 0x0 );
  
  /// Add a dock window.
  virtual void                          initializePlugin ( Usul::Interfaces::IUnknown *caller = 0x0 );
  
private:
  
  // Do not copy.
  LayerManagerComponent ( const LayerManagerComponent & );
  LayerManagerComponent &operator = ( const LayerManagerComponent & );
  
  Usul::Interfaces::IUnknown::QueryPtr _caller;

  Docks                     _docks;
  Minerva::LayersTree      *_layers;
  Favorites                *_favorites;
};


#endif // _LAYER_MANAGER_COMPONENT_H_
