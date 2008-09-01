
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

class GeoRSSQtComponent : public Usul::Base::Referenced,
                          public Usul::Interfaces::IPlugin,
                          public Usul::Interfaces::ILayerAddGUIQt,
                          public Usul::Interfaces::ILayerModifyGUIQt
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced                                   BaseClass;
  typedef Usul::Interfaces::IUnknown                               Unknown;

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
  virtual bool                handle ( Usul::Interfaces::ILayer* ) const;
  virtual void                showModifyGUI ( Usul::Interfaces::ILayer*, Usul::Interfaces::IUnknown* caller = 0x0 );
  
  /// Return name of plugin.
  virtual std::string         getPluginName() const;

  // Do not copy.
  GeoRSSQtComponent ( const GeoRSSQtComponent & );
  GeoRSSQtComponent &operator = ( const GeoRSSQtComponent & );

  /// Use reference counting.
  virtual ~GeoRSSQtComponent();

private:
  AddGeoRSSLayerWidget *_widget;
};


#endif // _OSSIM_LAYER_QT_H_
