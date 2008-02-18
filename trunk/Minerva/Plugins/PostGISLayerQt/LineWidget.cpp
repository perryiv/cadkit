
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/PostGISLayerQt/LineWidget.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineWidget::LineWidget ( LineLayer* layer, QWidget* parent ) :
BaseClass ( parent ),
_layer ( layer )
{
  this->setupUi ( this );

  if ( _layer.valid () )
    _lineWidth->setValue ( _layer->lineWidth() );

  connect ( _lineWidth, SIGNAL ( valueChanged ( double ) ), this, SLOT ( _lineWidthChanged ( double ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The line width has changed.
//
///////////////////////////////////////////////////////////////////////////////

void LineWidget::_lineWidthChanged( double value )
{
  if ( _layer.valid() )
    _layer->lineWidth ( value );
}