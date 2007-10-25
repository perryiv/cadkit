
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/PostGISLayerQt/SingleColorWidget.h"
#include "Minerva/Core/Functors/SingleColorFunctor.h"

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

SingleColorWidget::SingleColorWidget ( Layer *layer, QWidget *parent ) :
BaseClass ( parent ),
_layer ( layer ),
_colorButton ( new QtTools::ColorButton )
{
  QHBoxLayout *layout ( new QHBoxLayout );
  QLabel *label ( new QLabel ( "Choose Color" ) );
  layout->addWidget ( label );
  layout->addWidget ( _colorButton );
  this->setLayout ( layout );

  connect ( _colorButton, SIGNAL ( colorChanged() ), this, SLOT ( _colorChanged() ) );

  _colorButton->color ( QColor ( 255, 0, 0 ) ); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  The color has changed.
//
///////////////////////////////////////////////////////////////////////////////

void SingleColorWidget::_colorChanged ()
{
  Minerva::Core::Functors::SingleColorFunctor::RefPtr functor ( new Minerva::Core::Functors::SingleColorFunctor );
  functor->color ( QtTools::Color< osg::Vec4 >::convert ( _colorButton->color () ) );

  if ( _layer.valid() )
    _layer->colorFunctor ( functor.get() );
}
