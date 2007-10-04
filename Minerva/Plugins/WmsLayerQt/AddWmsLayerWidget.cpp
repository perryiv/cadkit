
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/WmsLayerQt/CompileGuard.h"
#include "Minerva/Plugins/WmsLayerQt/AddWmsLayerWidget.h"

#include "Minerva/Interfaces/IAddLayer.h"

#include "Magrathea/WmsLayer.h"

#include "QtGui/QLabel"
#include "QtGui/QLineEdit"
#include "QtGui/QHBoxLayout"
#include "QtGui/QVBoxLayout"
#include "QtGui/QListWidget"
#include "QtGui/QPushButton"
#include "QtGui/QRadioButton"
#include "QtGui/QButtonGroup"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddWmsLayerWidget::AddWmsLayerWidget( QWidget *parent ) : BaseClass ( parent ),
_server ( 0x0 ),
_cacheDirectory ( 0x0 ),
_imageTypes ( 0x0 )
{
  _server = new QLineEdit ( this );
  _cacheDirectory = new QLineEdit ( this );

  QVBoxLayout *topLayout ( new QVBoxLayout );
  this->setLayout ( topLayout );

  QHBoxLayout *layout0 ( new QHBoxLayout );
  QHBoxLayout *layout1 ( new QHBoxLayout );

  QLabel *serverLabel ( new QLabel ( "Server:" ) );
  QLabel *cacheLabel  ( new QLabel ( "Cache Directory:" ) );

  layout0->addWidget ( serverLabel );
  layout0->addWidget ( _server );

  layout1->addWidget ( cacheLabel );
  layout1->addWidget ( _cacheDirectory );

  topLayout->addLayout ( layout0 );
  topLayout->addLayout ( layout1 );

  _imageTypes = new QButtonGroup;
  QRadioButton *jpeg ( new QRadioButton ( "image/jpeg" ) );
  QRadioButton *png  ( new QRadioButton ( "image/png" ) );

  _imageTypes->addButton ( jpeg );
  _imageTypes->addButton ( png );

  QVBoxLayout *layout2 ( new QVBoxLayout );
  layout2->addWidget ( jpeg );
  layout2->addWidget ( png );

  topLayout->addLayout ( layout2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AddWmsLayerWidget::~AddWmsLayerWidget()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add images to caller.
//
///////////////////////////////////////////////////////////////////////////////

void AddWmsLayerWidget::apply ( Usul::Interfaces::IUnknown * caller )
{
  Minerva::Interfaces::IAddLayer::QueryPtr al ( caller );

  if ( false == al.valid () )
    return;

  std::string server ( _server->text().toStdString() );
  std::string cacheDirectory ( _cacheDirectory->text().toStdString() );

  // Make sure we have a server and cache directory.
  if ( false == server.empty () && false == cacheDirectory.empty () )
  {
    Magrathea::WmsLayer::RefPtr layer ( new Magrathea::WmsLayer );
    layer->server ( server );
    layer->cacheDirectory ( cacheDirectory );

    // Get the checked button.
    QAbstractButton *button ( _imageTypes->checkedButton () );
    QString type ( 0x0 != button ? button->text() : "image/jpeg" );

    // Set the image type.
    layer->imageType ( type.toStdString () );

    al->addLayer ( layer.get () );
  }
}
