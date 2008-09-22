
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
//  Component for ArcIMS GUI.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WMS_LAYER_QT_H_
#define _WMS_LAYER_QT_H_

#include "Minerva/Plugins/QtArcIMS/CompileGuard.h"
#include "Minerva/Core/Layers/RasterLayerArcIMS.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ILayerAddGUIQt.h"

namespace Minerva {  namespace Widgets { template <class Layer> class AddNetworkLayerWidget; } }

class QtArcIMSComponent : public Usul::Base::Referenced,
                          public Usul::Interfaces::IPlugin,
                          public Usul::Interfaces::ILayerAddGUIQt

{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced               BaseClass;
  typedef Usul::Interfaces::IUnknown           Unknown;
  typedef Minerva::Widgets::AddNetworkLayerWidget<Minerva::Core::Layers::RasterLayerArcIMS> AddArcIMSLayerWidget;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( QtArcIMSComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor
  QtArcIMSComponent();
  
protected:

  /// ILayerAddGUIQt.
  virtual QWidget*            layerAddGUI ( Usul::Interfaces::IUnknown *caller = 0x0 );
  virtual std::string         name () const;
  virtual void                apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller );

  /// Return name of plugin.
  virtual std::string         getPluginName() const;

  // Do not copy.
  QtArcIMSComponent ( const QtArcIMSComponent & );
  QtArcIMSComponent &operator = ( const QtArcIMSComponent & );

  /// Use reference counting.
  virtual ~QtArcIMSComponent();

private:
  AddArcIMSLayerWidget *_widget;
};


#endif // _WMS_LAYER_QT_H_
