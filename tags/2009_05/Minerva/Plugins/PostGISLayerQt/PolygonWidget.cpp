
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/PostGISLayerQt/PolygonWidget.h"

#include "QtTools/Color.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonWidget::PolygonWidget ( PolygonLayer* layer, QWidget* parent ) :
  BaseClass ( parent ),
  _layer ( layer )
{
  this->setupUi ( this );

  if ( _layer.valid () )
  {
    _drawBorder->setChecked ( _layer->showBorder() );
    _drawInterior->setChecked ( _layer->showInterior () );
    _borderWidth->setValue ( static_cast<int> ( _layer->borderWidth () ) );
    _borderColor->color ( QtTools::Color< Usul::Math::Vec4f >::convert ( _layer->borderColor() ) );
  }

  connect ( _drawBorder, SIGNAL ( stateChanged ( int ) ), this, SLOT ( _drawBorderChanged ( int ) ) );
  connect ( _drawInterior, SIGNAL ( stateChanged ( int ) ), this, SLOT ( _drawInteriorChanged ( int ) ) );
  connect ( _borderWidth, SIGNAL ( valueChanged ( int ) ), this, SLOT ( _borderWidthChanged ( int ) ) );
  connect ( _borderColor, SIGNAL ( colorChanged () ), this, SLOT ( _borderColorChanged ( ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw border has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonWidget::_drawBorderChanged ( int state )
{
  if ( _layer.valid () )
    _layer->showBorder ( state > 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw interior has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonWidget::_drawInteriorChanged ( int state )
{
  if ( _layer.valid () )
    _layer->showInterior ( state > 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Border width has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonWidget::_borderWidthChanged ( int value )
{
  if ( _layer.valid () )
    _layer->borderWidth ( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Border color has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonWidget::_borderColorChanged ()
{
  if ( _layer.valid() )
    _layer->borderColor ( QtTools::Color< Usul::Math::Vec4f >::convert ( _borderColor->color() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the polygon data be rasterized?
//
///////////////////////////////////////////////////////////////////////////////

bool PolygonWidget::rasterize() const
{
  return Qt::Checked == _rasterize->checkState();
}
