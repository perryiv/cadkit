
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __POSTGIS_COLOR_PAGE_H__
#define __POSTGIS_COLOR_PAGE_H__

#include "ui_LayerProperties.h"

#include "Minerva/DataSources/PG/Connection.h"
#include "Minerva/Layers/PostGIS/Layer.h"

#include "QtTools/ColorButton.h"

#include "QtGui/QWidget"

#include <vector>
#include <string>

class AddPostGISLayerWidget;

class PropertyPage : public QWidget,
                     private Ui::LayerProperties
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  typedef Minerva::DataSources::PG::Connection Connection;
  typedef Minerva::Layers::PostGIS::Layer  Layer;
  typedef std::vector < std::string > Strings;

  PropertyPage ( Layer *layer, QWidget *parent = 0x0 );
  virtual ~PropertyPage ();

  bool rasterize() const;
protected:
  void _initDrawingTab();
  void _initTimeTab();
  void _initGeneralTab();
  void _initQueryTab();

  void _initLabelProperties();
  void _initLegendProperties();
  
  void _updateQueryEdit();

protected slots:
  void _tabChanged ( int index );

  /// Slot for drawing tab.
  void _colorTypeChanged ( int index );

  /// The name for the layer has changed.
  void _nameChanged();

  /// Slots for label.
  void _labelShownChanged( int state );
  void _labelColorChanged();
  void _labelColumnChanged( int index );
  void _labelSizeChanged( int value );
  void _labelZOffsetChanged ( int value );

  /// Slots for legend.
  void _showInLegendChanged( int state );
  void _legendTextChanged();
  void _showCountLegendChanged( int state );
  void _showMinLegendChanged( int state );
  void _showMaxLegendChanged( int state );

  /// Slots for offset.
  void _xOffsetChanged ( double value );
  void _yOffsetChanged ( double value );
  void _zOffsetChanged ( double value );

  /// Slots for query tab.
  void _customQueryChanged ( int state );
  void _customQueryTextChanged ();
  void _primaryKeyChanged ();

private:
  Layer::RefPtr _layer;
  QWidget *_colorWidget;
  QWidget *_primitiveWidget;
};


#endif // __POSTGIS_COLOR_PAGE_H__
