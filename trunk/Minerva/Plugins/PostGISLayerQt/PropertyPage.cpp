
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/PostGISLayerQt/PropertyPage.h"
#include "Minerva/Plugins/PostGISLayerQt/AddPostGISLayerWidget.h"
#include "Minerva/Plugins/PostGISLayerQt/SingleColorWidget.h"
#include "Minerva/Plugins/PostGISLayerQt/GradientColorWidget.h"
#include "Minerva/Plugins/PostGISLayerQt/UniqueColorWidget.h"
#include "Minerva/Plugins/PostGISLayerQt/PointWidget.h"
#include "Minerva/Plugins/PostGISLayerQt/LineWidget.h"
#include "Minerva/Plugins/PostGISLayerQt/PolygonWidget.h"

#include "Minerva/DataSources/PG/Info.h"

#include "Usul/Strings/Qt.h"

#include "QtTools/Color.h"
#include "QtTools/ComboBox.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PropertyPage::PropertyPage( Layer *layer, QWidget *parent ) : BaseClass ( parent ),
  _layer ( layer ),
  _colorWidget ( 0x0 ),
  _primitiveWidget ( 0x0 )
{
  // Initialize code from Designer.
  this->setupUi ( this );

  // We want to know when the tab changes.
  QObject::connect ( _tabWidget, SIGNAL ( currentChanged ( int ) ), this, SLOT ( _tabChanged ( int ) ) );

  // Initialize all the tabs.
  this->_initDrawingTab();
  this->_initTimeTab();
  this->_initGeneralTab();
  this->_initQueryTab();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PropertyPage::~PropertyPage ()
{
  if ( 0x0 != _colorWidget )
    delete _colorWidget;

  if ( 0x0 != _primitiveWidget )
    delete _primitiveWidget;
}


///////////////////////////////////////////////////////////////////////////////
//
//  The color type has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_colorTypeChanged ( int index )
{
  _colorStack->removeWidget ( _colorWidget );
  delete _colorWidget;
  _colorWidget = 0x0;

  Layer::RefPtr layer ( _layer );

  switch ( index )
  {
  case 0:
    _colorWidget = new SingleColorWidget ( layer.get(), this );
    break;
  case 1:
    _colorWidget = new GradientColorWidget ( layer.get(), this );
    break;
  case 2:
    _colorWidget = new UniqueColorWidget ( layer.get(), this );
    break;
  }

  if ( 0x0 != _colorWidget )
  {
    _colorStack->addWidget ( _colorWidget );
    _colorStack->setCurrentWidget ( _colorWidget );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the query tab.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_initQueryTab ()
{
  // Connect slots and signals.
  QObject::connect ( _customQuery, SIGNAL ( stateChanged ( int ) ), this, SLOT ( _customQueryChanged ( int ) ) );
  QObject::connect ( _queryEdit, SIGNAL ( textChanged () ), this, SLOT ( _customQueryTextChanged () ) );
  QObject::connect ( _primaryKeyText, SIGNAL ( editingFinished () ), this, SLOT ( _primaryKeyChanged () ) );

  if ( _layer.valid() )
  {
    // Set the current state.
    _customQuery->setChecked ( _layer->customQuery() );
    _primaryKeyText->setText ( _layer->primaryKeyColumn().c_str () );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The tab has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_tabChanged ( int index )
{
  if ( index > 0 && _tabWidget->widget ( index ) == _queryTab )
  {
    this->_updateQueryEdit();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Custom query check box has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_customQueryChanged ( int state )
{
  bool customQuery ( Qt::Checked == state );

  if ( _layer.valid () )
    _layer->customQuery ( customQuery );

  _queryEdit->setReadOnly ( !customQuery );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Custom query text has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_customQueryTextChanged ()
{
  if ( _layer.valid () )
    _layer->query ( _queryEdit->toPlainText().toStdString() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update query edit.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_updateQueryEdit()
{
  if ( _layer.valid() )
  {
    std::string query ( _layer->customQuery() ? _layer->query() : _layer->defaultQuery() );
    _queryEdit->setText ( query.c_str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Primary Key has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_primaryKeyChanged ()
{
  if ( _layer.valid() )
    _layer->primaryKeyColumn ( _primaryKeyText->text().toStdString() );
  this->_updateQueryEdit();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the general tab.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_initGeneralTab()
{
  if ( _layer.valid() )
  {
    _nameEdit->setText ( _layer->name().c_str() );

    _xOffset->setValue ( _layer->xOffset() );
    _yOffset->setValue ( _layer->yOffset() );
    _zOffset->setValue ( _layer->zOffset() );
  }

  QObject::connect ( _nameEdit, SIGNAL ( editingFinished () ),      this, SLOT ( _nameChanged () ) );
  QObject::connect ( _xOffset,  SIGNAL ( valueChanged ( double ) ), this, SLOT ( _xOffsetChanged ( double ) ) );
  QObject::connect ( _yOffset,  SIGNAL ( valueChanged ( double ) ), this, SLOT ( _yOffsetChanged ( double ) ) );
  QObject::connect ( _zOffset,  SIGNAL ( valueChanged ( double ) ), this, SLOT ( _zOffsetChanged ( double ) ) );

  this->_initLabelProperties();
  this->_initLegendProperties();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the label properties.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_initLabelProperties()
{
  if ( _layer.valid() )
  {
    _labelShown->setChecked ( _layer->showLabel () );
    _labelColor->color ( QtTools::Color< Usul::Math::Vec4f >::convert ( _layer->labelColor() ) );
    
    // Get all columns.
    Connection::RefPtr connection ( _layer->connection() );
    if ( connection.valid() )
    {
      Connection::ScopedConnection sc ( *connection );
      Minerva::DataSources::PG::Info::RefPtr info ( new Minerva::DataSources::PG::Info ( connection ) );
      Strings columns ( info->getColumnNames ( _layer->tablename() ) );

      // Populate the combo box.
      QtTools::ComboBox::populate ( *_labelColumn, columns, _layer->labelColumn() );
    }

    _labelSize->setValue ( static_cast<int> ( _layer->labelSize () ) );
    _labelZOffset->setValue ( static_cast<int> ( _layer->labelZOffset () ) );
  }

  // Slots and signals for labels.
  QObject::connect ( _labelShown,   SIGNAL ( stateChanged ( int )        ), this, SLOT ( _labelShownChanged   ( int ) ) );
  QObject::connect ( _labelColor,   SIGNAL ( colorChanged ( )            ), this, SLOT ( _labelColorChanged   ( )     ) );
  QObject::connect ( _labelColumn,  SIGNAL ( currentIndexChanged ( int ) ), this, SLOT ( _labelColumnChanged  ( int ) ) );
  QObject::connect ( _labelSize,    SIGNAL ( valueChanged ( int )        ), this, SLOT ( _labelSizeChanged    ( int ) ) );
  QObject::connect ( _labelZOffset, SIGNAL ( valueChanged ( int )        ), this, SLOT ( _labelZOffsetChanged ( int ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the legend properties.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_initLegendProperties()
{
  if ( _layer.valid() )
  {
    _showInLegend->setChecked ( _layer->showInLegend() );
    _legendText->setText ( _layer->legendText().c_str() );
    _showCountLegend->setChecked ( _layer->showCountLegend() );
    _showMinLegend->setChecked ( _layer->showMinLegend() );
    _showMaxLegend->setChecked ( _layer->showMaxLegend() );
  }

  // Slots and signals for labels.
  QObject::connect ( _showInLegend,    SIGNAL ( stateChanged ( int )  ), this, SLOT ( _showInLegendChanged    ( int ) ) );
  QObject::connect ( _legendText,      SIGNAL ( editingFinished ( )   ), this, SLOT ( _legendTextChanged       ( )     ) );
  QObject::connect ( _showCountLegend, SIGNAL ( stateChanged ( int )  ), this, SLOT ( _showCountLegendChanged  ( int ) ) );
  QObject::connect ( _showMinLegend,   SIGNAL ( stateChanged ( int )  ), this, SLOT ( _showMinLegendChanged     ( int ) ) );
  QObject::connect ( _showMaxLegend,   SIGNAL ( stateChanged ( int )  ), this, SLOT ( _showMaxLegendChanged    ( int ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the drawing tab.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_initDrawingTab()
{
  QObject::connect ( _colorTypeComboBox, SIGNAL ( currentIndexChanged ( int ) ), this, SLOT ( _colorTypeChanged ( int ) ) );

  _colorTypeComboBox->addItem ( tr ( "Single Color" ) );
  _colorTypeComboBox->addItem ( tr ( "Gradient Color" ) );
  _colorTypeComboBox->addItem ( tr ( "Unique Color" ) );

  // Typedefs.
  typedef Minerva::Layers::PostGIS::PointLayer PointLayer;
  typedef Minerva::Layers::PostGIS::PolygonLayer PolygonLayer;
  typedef Minerva::Layers::PostGIS::LineLayer LineLayer;

  // Check to see if it's a point layer...
  if ( PointLayer *point = dynamic_cast < PointLayer * > ( _layer.get() ) )
    _primitiveWidget = new PointWidget ( point, this );

  // Check to see if it's a line layer...
  if ( LineLayer *line = dynamic_cast < LineLayer * > ( _layer.get() ) )
    _primitiveWidget = new LineWidget ( line, this );

  // Check to see if it's a polygon layer...
  if ( PolygonLayer *polygon = dynamic_cast < PolygonLayer * > ( _layer.get() ) )
    _primitiveWidget = new PolygonWidget ( polygon, this );

  if ( 0x0 != _primitiveWidget )
  {
    _primitiveStack->addWidget ( _primitiveWidget );
    _primitiveStack->setCurrentWidget ( _primitiveWidget );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the time tab.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_initTimeTab()
{
  if ( _layer.valid () )
  {
    // Get the conection.
    Connection::RefPtr connection ( _layer->connection() );
    if ( connection.valid() )
    {
      Connection::ScopedConnection sc ( *connection );
      Minerva::DataSources::PG::Info::RefPtr info ( new Minerva::DataSources::PG::Info ( connection ) );

      // Get all date columns.
      Strings columns ( info->getColumnNames ( _layer->tablename(), "date" ) );

      // Add the items to the combo box.
      QtTools::ComboBox::populate ( *_firstDateColumn, columns );
      QtTools::ComboBox::populate ( *_lastDateColumn, columns );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The label shown state has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_labelShownChanged( int state )
{
  if ( _layer.valid() )
    _layer->showLabel ( state != 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The label color has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_labelColorChanged()
{
  if ( _layer.valid() )
  {
    QColor c ( _labelColor->color() );
    _layer->labelColor ( QtTools::Color< Usul::Math::Vec4f >::convert ( c ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The label column has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_labelColumnChanged ( int index )
{
  if ( _layer.valid () )
    _layer->labelColumn ( _labelColumn->currentText().toStdString() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The label size has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_labelSizeChanged ( int value )
{
  if ( _layer.valid() )
    _layer->labelSize ( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The label z offset has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_labelZOffsetChanged ( int value )
{
  if ( _layer.valid () )
    _layer->labelZOffset ( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The name has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_nameChanged()
{
  if ( _layer.valid() )
    _layer->name ( _nameEdit->text().toStdString() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show in legend check changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_showInLegendChanged ( int state )
{
  if ( _layer.valid() )
    _layer->showInLegend ( state > 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Legend text has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_legendTextChanged ()
{
  if ( _layer.valid() )
    _layer->legendText ( _legendText->text().toStdString() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show count in legend has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_showCountLegendChanged ( int state )
{
  if ( _layer.valid() )
    _layer->showCountLegend ( state > 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show min in legend has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_showMinLegendChanged ( int state )
{
  if ( _layer.valid() )
    _layer->showMinLegend ( state > 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show max in legend has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_showMaxLegendChanged ( int state )
{
  if ( _layer.valid() )
    _layer->showMaxLegend ( state > 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  X offset has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_xOffsetChanged ( double value )
{
  if ( _layer.valid() )
    _layer->xOffset ( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Y offset has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_yOffsetChanged ( double value )
{
  if ( _layer.valid() )
    _layer->yOffset ( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Z offset has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_zOffsetChanged ( double value )
{
  if ( _layer.valid() )
    _layer->zOffset ( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rasterize.
//
///////////////////////////////////////////////////////////////////////////////

bool PropertyPage::rasterize() const
{
  bool rasterize ( false );
  
  if ( const PolygonWidget *pw = dynamic_cast<const PolygonWidget*> ( _primitiveWidget ) )
    rasterize = pw->rasterize();
  
  return rasterize;
}
