
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/PostGISLayerQt/GradientColorWidget.h"

#include "QtTools/ColorButton.h"

#include "QtGui/QHBoxLayout"
#include "QtGui/QPushButton"
#include "QtGui/QLabel"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GradientColorWidget::GradientColorWidget ( Layer *layer, QWidget *parent ) :
BaseClass ( parent ),
_layer ( layer ),
_gradientColor ( 0x0 ),
_minColorButton ( new QtTools::ColorButton ),
_maxColorButton ( new QtTools::ColorButton )
{
  QHBoxLayout *layout ( new QHBoxLayout );
  QLabel *label ( new QLabel ( "Choose Color" ) );
  layout->addWidget ( label );
  layout->addWidget ( _minColorButton );
  layout->addWidget ( _maxColorButton );
  this->setLayout ( layout );

  QObject::connect ( _minColorButton, SIGNAL ( colorChanged() ), this, SLOT ( _minColorChanged() ) );
  QObject::connect ( _maxColorButton, SIGNAL ( colorChanged() ), this, SLOT ( _maxColorChanged() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GradientColorWidget::~GradientColorWidget()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  The color has changed.
//
///////////////////////////////////////////////////////////////////////////////

void GradientColorWidget::_minColorChanged ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  The color has changed.
//
///////////////////////////////////////////////////////////////////////////////

void GradientColorWidget::_maxColorChanged ()
{
}
