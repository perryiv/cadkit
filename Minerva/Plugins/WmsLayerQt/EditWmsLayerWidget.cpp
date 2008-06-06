
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/WmsLayerQt/EditWmsLayerWidget.h"
#include "Minerva/Plugins/WmsLayerQt/OptionsDialog.h"
#include "Minerva/Plugins/WmsLayerQt/AlphasDialog.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

EditWmsLayerWidget::EditWmsLayerWidget ( RasterLayerWms *layer, QWidget * parent ) : BaseClass ( parent ),
  _layer ( layer )
{
  // Initialize code from designer.
  this->setupUi ( this );
  
  if ( _layer.valid() )
  {
    _name->setText( _layer->name().c_str() );
    _server->setText ( _layer->urlBase().c_str() );
  }
  
  // Connect slots and signals.
  this->connect ( _name, SIGNAL ( editingFinished() ), this, SLOT ( _nameFinishedEditing() ) );
  this->connect ( _server, SIGNAL ( editingFinished() ), this, SLOT ( _serverFinishedEditing() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

EditWmsLayerWidget::~EditWmsLayerWidget()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the new name.
//
///////////////////////////////////////////////////////////////////////////////

void EditWmsLayerWidget::_nameFinishedEditing()
{
  if ( _layer.valid() )
    _layer->name ( _name->text().toStdString() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the new server.
//
///////////////////////////////////////////////////////////////////////////////

void EditWmsLayerWidget::_serverFinishedEditing()
{
  if ( _layer.valid() )
    _layer->urlBase ( _server->text().toStdString() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The view options button has been clicked.
//
///////////////////////////////////////////////////////////////////////////////

void EditWmsLayerWidget::on_viewOptionsButton_clicked()
{
  if ( _layer.valid() )
  {
    OptionsDialog dialog ( _layer->options(), this );
    
    if ( QDialog::Accepted == dialog.exec() )
      _layer->options ( dialog.options() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The view options button has been clicked.
//
///////////////////////////////////////////////////////////////////////////////

void EditWmsLayerWidget::on_viewAlphasButton_clicked()
{
  if ( _layer.valid() )
  {
    RasterLayerWms::Alphas alphas ( _layer->alphas() );
    
    AlphasDialog dialog ( alphas, this );
    
    if ( QDialog::Accepted == dialog.exec() )
      _layer->alphas ( dialog.alphas() );
  }
}
