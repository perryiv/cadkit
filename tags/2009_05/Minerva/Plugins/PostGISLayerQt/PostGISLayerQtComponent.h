
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

#ifndef _POSTGIS_LAYER_QT_H_
#define _POSTGIS_LAYER_QT_H_

#include "Minerva/Plugins/PostGISLayerQt/CompileGuard.h"
#include "Minerva/Plugins/PostGISLayerQt/AddPostGISLayerWidget.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Headers/Qt.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ILayerAddGUIQt.h"
#include "Usul/Interfaces/ILayerModifyGUIQt.h"

class PostGISLayerQtComponent : public Usul::Base::Referenced,
                                public Usul::Interfaces::IPlugin,
                                public Usul::Interfaces::ILayerAddGUIQt,
                                public Usul::Interfaces::ILayerModifyGUIQt

{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced                                   BaseClass;
  typedef Usul::Interfaces::IUnknown                               Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PostGISLayerQtComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor
  PostGISLayerQtComponent();
  
protected:

  /// ILayerAddGUIQt.
  virtual QWidget*            layerAddGUI ( Usul::Interfaces::IUnknown *caller = 0x0 );
  virtual std::string         name () const;
  virtual void                apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller );

  /// Return name of plugin.
  virtual std::string         getPluginName() const;

  /// ILayerModifyQtGUI
  virtual bool                handle ( Usul::Interfaces::IUnknown* ) const;
  virtual void                showModifyGUI ( Usul::Interfaces::IUnknown*, Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller = 0x0 );

  // Do not copy.
  PostGISLayerQtComponent ( const PostGISLayerQtComponent & );
  PostGISLayerQtComponent &operator = ( const PostGISLayerQtComponent & );

  /// Use reference counting.
  virtual ~PostGISLayerQtComponent();

private:

  AddPostGISLayerWidget *_widget;
};


#endif // _POSTGIS_LAYER_QT_H_
