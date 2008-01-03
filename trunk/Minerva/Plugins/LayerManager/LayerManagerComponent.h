
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
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _LAYER_MANAGER_COMPONENT_H_
#define _LAYER_MANAGER_COMPONENT_H_

#include "Minerva/Plugins/LayerManager/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IActiveDocumentListener.h"
#include "Usul/Interfaces/GUI/IAddDockWindow.h"

#include <vector>

class QDockWidget;
class LayersTree;
class Favorites;

class LayerManagerComponent : public Usul::Base::Referenced,
                              public Usul::Interfaces::IPlugin,
                              public Usul::Interfaces::IActiveDocumentListener,
                              public Usul::Interfaces::IAddDockWindow
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced                                   BaseClass;
  typedef Usul::Interfaces::IUnknown                               Unknown;
  typedef std::vector<QDockWidget*>                                Docks;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( LayerManagerComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor
  LayerManagerComponent();
  
protected:

  /// Return name of plugin.
  virtual std::string           getPluginName() const;

  // Do not copy.
  LayerManagerComponent ( const LayerManagerComponent & );
  LayerManagerComponent &operator = ( const LayerManagerComponent & );

  /// Use reference counting.
  virtual ~LayerManagerComponent();
  
  /// The active document has changed (IActiveDocumentListener).
  virtual void                          activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc );

  /// Add a dock window.
  virtual void                          addDockWindow ( Usul::Interfaces::IUnknown *caller = 0x0 );
  
  Usul::Interfaces::IUnknown::QueryPtr _caller;

  Docks            _docks;
  LayersTree      *_layers;
  Favorites       *_favorites;
};


#endif // _LAYER_MANAGER_COMPONENT_H_
