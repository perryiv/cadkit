
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
#include "QtTools/Color.h"

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

  if ( 0x0 != layer )
    _gradientColor = dynamic_cast < GradientColor* > ( layer->colorFunctor() );

  if ( false == _gradientColor.valid() )
  {
    // Make a new functor.
    _gradientColor = new GradientColor;

    // Set the new functor.
    if ( _layer.valid() )
      _layer->colorFunctor ( _gradientColor.get() );
  }

  // Set the inital state of the color button.
  _minColorButton->color ( _gradientColor.valid() ? QtTools::Color<osg::Vec4>::convert ( _gradientColor->minColor() ) : QColor ( 255, 0, 0 ) );
  _maxColorButton->color ( _gradientColor.valid() ? QtTools::Color<osg::Vec4>::convert ( _gradientColor->maxColor() ) : QColor ( 255, 0, 0 ) );
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
  if ( _gradientColor.valid() )
    _gradientColor->minColor ( QtTools::Color< osg::Vec4 >::convert ( _minColorButton->color () ) ); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  The color has changed.
//
///////////////////////////////////////////////////////////////////////////////

void GradientColorWidget::_maxColorChanged ()
{
  if ( _gradientColor.valid() )
    _gradientColor->maxColor ( QtTools::Color< osg::Vec4 >::convert ( _maxColorButton->color () ) ); 
}
