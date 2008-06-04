
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __HELIOS_QT_PLUGINS_CLIP_PLANES_H__
#define __HELIOS_QT_PLUGINS_CLIP_PLANES_H__

#include "ui_ClipPlanes.h"

#include "OsgTools/Configure/OSG.h"

#include "Usul/Interfaces/IUnknown.h"

#include "QtGui/QWidget"

namespace osg { class Plane; }

class ClipPlanes : public QWidget,
                   private Ui::ClipPlanes
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;

  ClipPlanes ( QWidget *parent = 0x0 );
  virtual ~ClipPlanes ();

  void     clear();

  void     build( Usul::Interfaces::IUnknown* caller );

protected:
  bool     _isAdvanced() const;
  void     _render();
  void     _addClippingPlane( const osg::Plane& );
  void     _setMinMax ( double min, double max );
  void     _setDistance ( double value );

protected slots:
  void _addClipPlaneClicked();
  void _removeClipPlaneClicked();
  void _distanceSliderChanged ( int );
  void _distanceSpinBoxChanged ( double );
  void _selectedItemChanged( int );

  // These are named so they get automatically connected.
  void on_posXAxis_clicked();
  void on_posYAxis_clicked();
  void on_posZAxis_clicked();
  void on_negXAxis_clicked();
  void on_negYAxis_clicked();
  void on_negZAxis_clicked();
  
  void on_clearButton_clicked();

private:
  Usul::Interfaces::IUnknown::QueryPtr _caller;
};


#endif // __HELIOS_QT_PLUGINS_CLIP_PLANES_H__
