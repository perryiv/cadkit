
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
//  GeoRSS Qt GUI Component
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GEOSS_LAYER_QT_H_
#define _GEOSS_LAYER_QT_H_

#include "Minerva/Plugins/GeoRSSQt/CompileGuard.h"
#include "Minerva/Plugins/GeoRSSQt/AddGeoRSSLayerWidget.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ILayerAddGUIQt.h"
#include "Usul/Interfaces/ILayerModifyGUIQt.h"

namespace Minerva { namespace Layers { namespace GeoRSS { class GeoRSSLayer; class Item; } } }

class GeoRSSQtComponent : public Usul::Base::Referenced,
                          public Usul::Interfaces::IPlugin,
                          public Usul::Interfaces::ILayerAddGUIQt,
                          public Usul::Interfaces::ILayerModifyGUIQt
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced                BaseClass;
  typedef Usul::Interfaces::IUnknown            Unknown;
  typedef Minerva::Layers::GeoRSS::GeoRSSLayer  GeoRSSLayer;
  typedef Minerva::Layers::GeoRSS::Item         GeoRSSItem;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( GeoRSSQtComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor
  GeoRSSQtComponent();
  
protected:

  /// ILayerAddGUIQt.
  virtual QWidget*            layerAddGUI ( Usul::Interfaces::IUnknown* caller = 0x0 );
  virtual std::string         name () const;
  virtual void                apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller );

  /// ILayerModifyQtGUI
  virtual bool                handle ( Usul::Interfaces::IUnknown* ) const;
  virtual void                showModifyGUI ( Usul::Interfaces::IUnknown* layer, Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller = 0x0 );
  
  /// Return name of plugin.
  virtual std::string         getPluginName() const;

  // Do not copy.
  GeoRSSQtComponent ( const GeoRSSQtComponent & );
  GeoRSSQtComponent &operator = ( const GeoRSSQtComponent & );

  /// Use reference counting.
  virtual ~GeoRSSQtComponent();

private:
  
  /// Show the GUI to edit the GeoRSSLayer.
  void                        _displayGeoRSSLayerGUI ( GeoRSSLayer& layer, Usul::Interfaces::IUnknown* caller );
  
  /// Show the GUI to edit the GeoRSSItem.
  void                        _displayGeoRSSItemGUI ( GeoRSSItem& item, Usul::Interfaces::IUnknown* caller );
  
  /// Set the properties of the layer using the current state of the widget.
  static void                 _setLayerMembers ( AddGeoRSSLayerWidget& widget, GeoRSSLayer& layer );
  
  AddGeoRSSLayerWidget *_widget;
};


#endif // _OSSIM_LAYER_QT_H_
