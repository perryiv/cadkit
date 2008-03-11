
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WMS_LAYER_QT_H_
#define _WMS_LAYER_QT_H_

#include "Minerva/Plugins/WmsLayerQt/CompileGuard.h"
#include "Minerva/Plugins/WmsLayerQt/AddWmsLayerWidget.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ILayerAddGUIQt.h"

class WmsLayerQtComponent : public Usul::Base::Referenced,
                            public Usul::Interfaces::IPlugin,
                            public Usul::Interfaces::ILayerAddGUIQt

{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced                                   BaseClass;
  typedef Usul::Interfaces::IUnknown                               Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( WmsLayerQtComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor
  WmsLayerQtComponent();
  
protected:

  virtual QWidget*            layerAddGUI ( Usul::Interfaces::IUnknown *caller = 0x0 );
  virtual std::string         name () const;
  virtual void                apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller );

  /// Return name of plugin.
  virtual std::string           getPluginName() const;

  // Do not copy.
  WmsLayerQtComponent ( const WmsLayerQtComponent & );
  WmsLayerQtComponent &operator = ( const WmsLayerQtComponent & );

  /// Use reference counting.
  virtual ~WmsLayerQtComponent();

private:
  AddWmsLayerWidget *_widget;
};


#endif // _WMS_LAYER_QT_H_
