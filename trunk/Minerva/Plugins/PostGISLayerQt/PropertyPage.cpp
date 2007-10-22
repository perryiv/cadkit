
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



///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PropertyPage::PropertyPage( Layer *layer, QWidget *parent  ) : BaseClass ( parent ),
_layer ( layer ),
_colorWidget ( 0x0 )
{
  this->setupUi ( this );

  connect ( _tabWidget, SIGNAL ( currentChanged ( int ) ), this, SLOT ( _tabChanged ( int ) ) );
  connect ( _colorTypeComboBox, SIGNAL ( currentIndexChanged ( int ) ), this, SLOT ( _colorTypeChanged ( int ) ) );

  _colorTypeComboBox->addItem ( tr ( "Single Color" ) );
  _colorTypeComboBox->addItem ( tr ( "Gradient Color" ) );
  _colorTypeComboBox->addItem ( tr ( "Unique Color" ) );
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
//  The tab has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PropertyPage::_tabChanged ( int index )
{
  if ( index > 0 && _tabWidget->widget ( index ) == _queryTab )
  {
    if ( _layer.valid() && false == _layer->customQuery () )
    {
      _queryEdit->setText ( _layer->defaultQuery().c_str() );
    }
  }
}

