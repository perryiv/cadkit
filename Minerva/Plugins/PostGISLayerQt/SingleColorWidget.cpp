
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/PostGISLayerQt/SingleColorWidget.h"

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
  _colorButton ( new QtTools::ColorButton ),
  _functor ( 0x0 )
{
  QHBoxLayout *layout ( new QHBoxLayout );
  QLabel *label ( new QLabel ( "Choose Color" ) );
  layout->addWidget ( label );
  layout->addWidget ( _colorButton );
  this->setLayout ( layout );

  QObject::connect ( _colorButton, SIGNAL ( colorChanged() ), this, SLOT ( _colorChanged() ) );

  if ( 0x0 != layer )
    _functor = dynamic_cast < Minerva::Core::Functors::SingleColorFunctor* > ( layer->colorFunctor() );

  if ( false == _functor.valid() )
  {
    // Make a new functor.
    _functor = new Minerva::Core::Functors::SingleColorFunctor;

    // Set the new functor.
    if ( _layer.valid() )
      _layer->colorFunctor ( _functor.get() );
  }

  // Set the inital state of the color button.
  _colorButton->color ( _functor.valid() ? QtTools::Color<osg::Vec4>::convert ( _functor->color() ) : QColor ( 255, 0, 0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SingleColorWidget::~SingleColorWidget()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  The color has changed.
//
///////////////////////////////////////////////////////////////////////////////

void SingleColorWidget::_colorChanged ()
{
  if ( _functor.valid() )
    _functor->color ( QtTools::Color< osg::Vec4 >::convert ( _colorButton->color () ) ); 
}
