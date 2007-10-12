
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADD_POSTGIS_LAYER_WIDGET_H__
#define __ADD_POSTGIS_LAYER_WIDGET_H__

#include "Minerva/Plugins/PostGISLayerQt/CompileGuard.h"
#include "Minerva/Core/Layers/Layer.h"

#include "QtGui/QWizard"

#include <vector>
#include <string>


class QPushButton;

namespace Usul { namespace Interfaces { struct IUnknown; } }

class AddPostGISLayerWidget : public QWizard
{
  Q_OBJECT;
public:
  typedef QWizard BaseClass;
  typedef Minerva::Core::Layers::Layer Layer;

  AddPostGISLayerWidget( QWidget *parent = 0x0 );
  virtual ~AddPostGISLayerWidget();

  void apply ( Usul::Interfaces::IUnknown * caller );

  /// Get/Set the layer.
  void                    layer ( Layer* layer );
  Layer*                  layer ();
private:
  Layer::RefPtr _layer;
};


#endif // __ADD_POSTGIS_LAYER_WIDGET_H__
