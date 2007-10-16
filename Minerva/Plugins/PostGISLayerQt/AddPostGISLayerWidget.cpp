
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/PostGISLayerQt/AddPostGISLayerWidget.h"
#include "Minerva/Plugins/PostGISLayerQt/DatabasePage.h"
#include "Minerva/Plugins/PostGISLayerQt/ColorPage.h"

#include "Minerva/Interfaces/IAddLayer.h"

#include <iostream>

#if ( QT_VERSION >= QT_VERSION_CHECK ( 4, 3, 0 ) )

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddPostGISLayerWidget::AddPostGISLayerWidget( QWidget *parent ) : BaseClass ( parent ),
_layer ( 0x0 )
{
  //this->setButton ( BackButton, new QPushButton ( "Previous" ) );
  //this->setButton ( NextButton, new QPushButton ( "Next"     ) );

  QList<QWizard::WizardButton> layout;
  layout << QWizard::Stretch << QWizard::BackButton  << QWizard::NextButton << QWizard::FinishButton;
  this->setButtonLayout(layout);

  this->addPage ( new DatabasePage ( this ) );
  this->addPage ( new ColorPage ( this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AddPostGISLayerWidget::~AddPostGISLayerWidget()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add images to caller.
//
///////////////////////////////////////////////////////////////////////////////

void AddPostGISLayerWidget::apply ( Usul::Interfaces::IUnknown * caller )
{
  Minerva::Interfaces::IAddLayer::QueryPtr al ( caller );

  if ( false == al.valid () )
    return;

  if ( _layer.valid () )
    al->addLayer ( _layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the layer.
//
///////////////////////////////////////////////////////////////////////////////

void AddPostGISLayerWidget::layer ( Layer* layer )
{
  _layer = layer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Layers::Layer* AddPostGISLayerWidget::layer ()
{
  return _layer.get();
}

#endif // QT_VERSION
