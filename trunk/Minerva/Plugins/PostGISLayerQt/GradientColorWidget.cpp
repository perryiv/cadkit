
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/PostGISLayerQt/GradientColorWidget.h"

#include "Minerva/DataSources/PG/Info.h"
#include "Minerva/DataSources/PG/Connection.h"

#include "Usul/Strings/Format.h"

#include "QtTools/ColorButton.h"
#include "QtTools/Color.h"
#include "QtTools/ComboBox.h"

#include "QtGui/QHBoxLayout"
#include "QtGui/QPushButton"
#include "QtGui/QLabel"
#include "QtGui/QComboBox"

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
  _maxColorButton ( new QtTools::ColorButton ),
  _columns ( new QComboBox )
{
  if ( _layer.valid() )
  {
    QHBoxLayout *layout ( new QHBoxLayout );
    QLabel *label ( new QLabel ( "Choose Color" ) );
    layout->addWidget ( label );
    layout->addWidget ( _columns );
    layout->addWidget ( _minColorButton );
    layout->addWidget ( _maxColorButton );
    this->setLayout ( layout );
    
    QObject::connect ( _minColorButton, SIGNAL ( colorChanged() ), this, SLOT ( _minColorChanged() ) );
    QObject::connect ( _maxColorButton, SIGNAL ( colorChanged() ), this, SLOT ( _maxColorChanged() ) );
    QObject::connect ( _columns,  SIGNAL ( currentIndexChanged ( int ) ), this, SLOT ( _columnChanged  ( int ) ) );
    
    if ( 0x0 != layer )
      _gradientColor = dynamic_cast < GradientColor* > ( layer->colorFunctor() );
    
    if ( false == _gradientColor.valid() )
    {
      // Make a new functor.
      _gradientColor = new GradientColor;
      
      // Set the new functor.
      _layer->colorFunctor ( _gradientColor.get() );
    }
    
    // Get all columns.
    typedef Minerva::DataSources::PG::Connection Connection;
    Connection::RefPtr connection ( _layer->connection() );
    if ( connection.valid() )
    {
      Connection::ScopedConnection sc ( *connection );
      Minerva::DataSources::PG::Info::RefPtr info ( new Minerva::DataSources::PG::Info ( connection ) );
      Minerva::DataSources::PG::Info::Strings columns ( info->getColumnNames ( _layer->tablename() ) );
      
      // Populate the combo box.
      QtTools::ComboBox::populate ( *_columns, columns, _layer->colorColumn() );
    }
    
    // Set the inital state of the color button.
    _minColorButton->color ( _gradientColor.valid() ? QtTools::Color<osg::Vec4>::convert ( _gradientColor->minColor() ) : QColor ( 255, 0, 0 ) );
    _maxColorButton->color ( _gradientColor.valid() ? QtTools::Color<osg::Vec4>::convert ( _gradientColor->maxColor() ) : QColor ( 255, 0, 0 ) );
  }
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


///////////////////////////////////////////////////////////////////////////////
//
//  The color has changed.
//
///////////////////////////////////////////////////////////////////////////////

void GradientColorWidget::_columnChanged ( int )
{
  if ( _layer.valid () )
  {
    _layer->colorColumn ( _columns->currentText().toStdString() );
    
    typedef Minerva::DataSources::PG::Connection Connection;
    Connection::RefPtr connection ( _layer->connection() );
    if ( connection.valid() )
    {
      Connection::ScopedConnection sc ( *connection );
      Minerva::DataSources::PG::Info::RefPtr info ( new Minerva::DataSources::PG::Info ( connection ) );
      
      double min ( 0.0 ), max ( 0.0 );
      info->getMinMaxValue ( Usul::Strings::format ( "SELECT * FROM ", _layer->tablename() ), _layer->colorColumn(), min, max );
      
      if ( _gradientColor.valid() )
      {
        _gradientColor->minValue ( min );
        _gradientColor->maxValue ( max );    
      }
    }      
  }
}
