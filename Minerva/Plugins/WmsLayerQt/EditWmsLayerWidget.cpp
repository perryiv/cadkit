
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/WmsLayerQt/EditWmsLayerWidget.h"
#include "Minerva/Qt/Widgets/OptionsDialog.h"
#include "Minerva/Qt/Widgets/AlphasDialog.h"

#include "Usul/File/Boost.h"
#include "Usul/Strings/Format.h"

#include "QtGui/QMessageBox"

#include <list>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

EditWmsLayerWidget::EditWmsLayerWidget ( RasterLayerNetwork *layer, QWidget * parent ) : BaseClass ( parent ),
  _layer ( layer )
{
  // Initialize code from designer.
  this->setupUi ( this );
  
  if ( _layer.valid() )
  {
    _name->setText( _layer->name().c_str() );
    _server->setText ( _layer->urlBase().c_str() );
    
    // Find the number of files.
    typedef std::list<std::string> Filenames;
    Filenames files;
    Usul::File::findFiles ( _layer->cacheDirectory(), _layer->cacheFileExtension(), files );
    _cacheInfoText->setText ( Usul::Strings::format ( files.size() ).c_str() );
    _cacheDirectoryText->setText ( _layer->cacheDirectory().c_str() );
  }
  
  // Currently only implemented on OS X.
#ifndef __APPLE__
  viewCacheButton->setVisible ( false );
#endif
  
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
    Minerva::Widgets::OptionsDialog dialog ( _layer->options(), this );
    
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
    RasterLayerNetwork::Alphas alphas ( _layer->alphas() );
    
    Minerva::Widgets::AlphasDialog dialog ( alphas, this );
    
    if ( QDialog::Accepted == dialog.exec() )
      _layer->alphas ( dialog.alphas() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the cache.
//
///////////////////////////////////////////////////////////////////////////////

void EditWmsLayerWidget::on_deleteCacheButton_clicked()
{
  if ( 0x0 == _layer )
    return;
  
  const std::string message ( "Are you sure you want to delete the cache?" );
  if ( QMessageBox::Ok != QMessageBox::question ( this, "Confirm", message.c_str(), QMessageBox::Ok | QMessageBox::Cancel ) )
    return;
  
  const std::string directory ( _layer->cacheDirectory() );
  if ( boost::filesystem::exists ( directory ) && boost::filesystem::is_directory ( directory ) )
    boost::filesystem::remove_all ( directory );
}


///////////////////////////////////////////////////////////////////////////////
//
//  View the cache.
//
///////////////////////////////////////////////////////////////////////////////

void EditWmsLayerWidget::on_viewCacheButton_clicked()
{
  if ( 0x0 == _layer )
    return;
  
#ifdef __APPLE__
  const std::string directory ( _layer->cacheDirectory() );
  const std::string command ( Usul::Strings::format ( "open ", directory ) );
  ::system ( command.c_str() );
#elif _WIN32
  // TODO: handle windows (http://msdn.microsoft.com/en-us/library/bb762232(VS.85).aspx)
#endif
}
