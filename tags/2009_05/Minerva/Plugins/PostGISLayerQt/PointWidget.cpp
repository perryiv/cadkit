
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/PostGISLayerQt/PointWidget.h"

#include "Minerva/DataSources/PG/Info.h"

#include "QtTools/ComboBox.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointWidget::PointWidget ( PointLayer* layer, QWidget* parent ) :
  BaseClass ( parent ),
  _layer ( layer )
{
  this->setupUi ( this );

  // Set the current values.
  if ( _layer.valid() )
  {
    _stackPoints->setChecked ( _layer->stackPoints() );
    _autoTransform->setChecked ( _layer->autotransform() );
    _primitiveType->setCurrentIndex ( _layer->primitiveID() - 1);
    _size->setValue ( _layer->size() );
    _secondarySize->setValue ( _layer->secondarySize() );
    _quality->setValue ( static_cast<int> ( _layer->quality() * 100 ) );

    // Get all columns.
    Minerva::DataSources::PG::Connection::RefPtr connection ( _layer->connection() );
    if ( connection.valid() )
    {
      Minerva::DataSources::PG::Connection::ScopedConnection sc ( *connection );
      Minerva::DataSources::PG::Info::RefPtr info ( new Minerva::DataSources::PG::Info ( connection ) );
      Minerva::DataSources::PG::Info::Strings columns ( info->getColumnNames ( _layer->tablename() ) );

      // Populate the combo box.
      QtTools::ComboBox::populate ( *_sizeColumn, columns, _layer->primitiveSizeColumn() );
    }
  }

  // Connect slots and signals.
  connect ( _stackPoints,   SIGNAL ( stateChanged ( int ) ),    this, SLOT ( _stackPointsChanged ( int ) ) );
  connect ( _autoTransform, SIGNAL ( stateChanged ( int ) ),    this, SLOT ( _autoTransformChanged ( int ) ) );
  connect ( _primitiveType, SIGNAL ( currentIndexChanged ( int) ), this, SLOT ( _primitiveTypeChanged ( int ) ) );
  connect ( _sizeColumn,    SIGNAL ( currentIndexChanged ( int ) ), this, SLOT ( _sizeColumnChanged ( int ) ) );
  connect ( _size,          SIGNAL ( valueChanged ( double ) ), this, SLOT ( _sizeChanged ( double ) ) );
  connect ( _secondarySize, SIGNAL ( valueChanged ( double ) ), this, SLOT ( _secondarySizeChanged ( double ) ) );
  connect ( _quality,       SIGNAL ( valueChanged ( int ) ),    this, SLOT ( _qualityChanged ( int ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PointWidget::~PointWidget()
{
}


//////////////////////////////////////////////////////////////////////////////
//
//  Stack points has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PointWidget::_stackPointsChanged ( int state )
{
  if ( _layer.valid() )
    _layer->stackPoints ( state > 0 );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Auto transform has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PointWidget::_autoTransformChanged ( int state )
{
  if ( _layer.valid() )
    _layer->autotransform ( state > 0 );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Primitive type has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PointWidget::_primitiveTypeChanged ( int index )
{
  if ( _layer.valid() && index >= 0 )
    _layer->primitiveID ( index + 1 );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Size column has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PointWidget::_sizeColumnChanged ( int index )
{
  if ( _layer.valid() )
    _layer->primitiveSizeColumn ( _sizeColumn->currentText().toStdString() );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Size has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PointWidget::_sizeChanged ( double value )
{
  if ( _layer.valid() )
    _layer->primitiveSize ( value );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Secondary size has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PointWidget::_secondarySizeChanged ( double value )
{
  if ( _layer.valid() )
    _layer->secondarySize ( value );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Quality has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PointWidget::_qualityChanged ( int value )
{
  if ( _layer.valid () && value > 0 && value <= 100 )
    _layer->quality ( static_cast < double > ( value ) / 100 );
}
