
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

#include "Usul/File/Path.h"

#include "Minerva/Interfaces/IAddLayer.h"

#include "Magrathea/ImageTextureLayer.h"
#include "Magrathea/KwlLayer.h"

#include "QtGui/QFileDialog"
#include "QtGui/QVBoxLayout"
#include "QtGui/QListWidget"
#include "QtGui/QPushButton"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddWmsLayerWidget::AddWmsLayerWidget( QWidget *parent ) : BaseClass ( parent ),
_server ( 0x0 ),
_cacheDirectory ( 0x0 )
{
  /*_listView = new QListWidget ( this );

  QPushButton *browse ( new QPushButton ( "Browse" ) );

  connect ( browse, SIGNAL ( clicked () ), this, SLOT ( _browseClicked () ) );

  QVBoxLayout *topLayout ( new QVBoxLayout );
  this->setLayout ( topLayout );

  topLayout->addWidget ( browse );
  topLayout->addWidget ( _listView );*/
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
}
