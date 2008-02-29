
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
#include "Minerva/Plugins/WmsLayerQt/OptionWidget.h"

#include "StarSystem/RasterLayerWms.h"

#include "Minerva/Interfaces/IAddLayer.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Contents.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Network/Curl.h"
#include "Usul/Network/Names.h"

#include "QtGui/QDialog"
#include "QtGui/QLabel"
#include "QtGui/QLineEdit"
#include "QtGui/QHBoxLayout"
#include "QtGui/QVBoxLayout"
#include "QtGui/QListWidget"
#include "QtGui/QPushButton"
#include "QtGui/QRadioButton"
#include "QtGui/QButtonGroup"
#include "QtGui/QFileDialog"
#include "QtGui/QTextEdit"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddWmsLayerWidget::AddWmsLayerWidget( QWidget *parent ) : BaseClass ( parent ),
  _imageTypes ( 0x0 ),
  _options()
{
  this->setupUi ( this );

  _imageTypes = new QButtonGroup;

  _imageTypes->addButton ( _jpegButton );
  _imageTypes->addButton ( _pngButton );
  
  _optionsWidget->setLayout ( new QVBoxLayout );
  
  _cacheDirectory->setText ( StarSystem::RasterLayerWms::defaultCacheDirectory().c_str() );
  
  QObject::connect ( _server, SIGNAL ( textChanged ( const QString& ) ), this, SLOT ( _onServerTextChanged ( const QString& ) ) );

  QObject::connect ( this, SIGNAL ( serverValid ( bool ) ), capabilitiesButton, SLOT ( setEnabled ( bool ) ) );
  QObject::connect ( this, SIGNAL ( serverValid ( bool ) ), addOptionButton,    SLOT ( setEnabled ( bool ) ) );
  QObject::connect ( this, SIGNAL ( serverValid ( bool ) ), _jpegButton,        SLOT ( setEnabled ( bool ) ) );
  QObject::connect ( this, SIGNAL ( serverValid ( bool ) ), _pngButton,         SLOT ( setEnabled ( bool ) ) );
  
  emit serverValid ( false );
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
  Minerva::Interfaces::IAddLayer::QueryPtr al ( Usul::Documents::Manager::instance().activeDocument() );

  if ( false == al.valid () )
    return;

  std::string server ( _server->text().toStdString() );
  std::string cacheDirectory ( _cacheDirectory->text().toStdString() );
  std::string name ( _name->text().toStdString() );

  // Get the checked button.
  QAbstractButton *button ( _imageTypes->checkedButton () );
  std::string format ( 0x0 != button ? button->text().toStdString() : "image/jpeg" );
  
  // Make sure we have a server and cache directory.
  if ( false == server.empty () && false == cacheDirectory.empty() )
  {
    StarSystem::RasterLayerWms::Extents extents ( -180, -90, 180, 90 );
    
    StarSystem::RasterLayerWms::Options options;
    
    for ( Options::const_iterator iter = _options.begin(); iter != _options.end(); ++iter )
    {
      std::string key ( (*iter)->key() );
      std::string value ( (*iter)->value() );
      
      if ( false == key.empty() )
      {
        options[key] = value;
      }
    }
    
    options[Usul::Network::Names::FORMAT] = format;
    
    // Make the layer
    StarSystem::RasterLayerWms::RefPtr layer ( new StarSystem::RasterLayerWms ( extents, server, options ) );

    // Set the cache directory.
    layer->cacheDirectory ( cacheDirectory, Qt::Checked == _makeDefaultDirectory->checkState() );
    
    // Set the name.
    layer->name ( false == name.empty() ? name : server );
    
    al->addLayer ( layer.get () );

  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Browse for a directory.
//
///////////////////////////////////////////////////////////////////////////////

void AddWmsLayerWidget::on_browseDirectory_clicked ()
{
  QString dir ( QFileDialog::getExistingDirectory ( this, tr ( "Cache Directory" ),
                                                    _cacheDirectory->text(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks ) );

  if ( dir.size() >  0 )
    _cacheDirectory->setText ( dir );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add an option.
//
///////////////////////////////////////////////////////////////////////////////

void AddWmsLayerWidget::on_addOptionButton_clicked()
{
  OptionWidget::Names names;
  names.push_back ( Usul::Network::Names::REQUEST );
  names.push_back ( Usul::Network::Names::SRS     );
  names.push_back ( Usul::Network::Names::STYLES  );
  names.push_back ( Usul::Network::Names::LAYERS  );
  names.push_back ( Usul::Network::Names::VERSION );
  
  OptionWidget *widget ( new OptionWidget ( names, _optionsWidget ) );
  _optionsWidget->layout()->addWidget( widget );
  
  _options.push_back ( widget );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the capabilites of the server.
//
///////////////////////////////////////////////////////////////////////////////

void AddWmsLayerWidget::on_capabilitiesButton_clicked()
{
  // Url request.
  std::string request ( _server->text().toStdString() + "?request=GetCapabilities" );
  
  // File to download to.
  Usul::File::Temp temp;
  
  // Download.
  Usul::Network::Curl curl ( request, temp.name() );
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( &curl, &Usul::Network::Curl::download ), "3034499311" );
  
  // File contents.
  std::string contents;
  Usul::File::contents ( temp.name(), contents );
  
  QDialog dialog;
  
  QVBoxLayout* layout ( new QVBoxLayout );
  
  QTextEdit *edit ( new QTextEdit );
  edit->setPlainText ( contents.c_str() );
  edit->setReadOnly( true );
  
  layout->addWidget ( edit );
  
  dialog.setLayout ( layout );

  dialog.exec();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the server text has changed.
//
///////////////////////////////////////////////////////////////////////////////

void AddWmsLayerWidget::_onServerTextChanged ( const QString& text )
{
  emit serverValid ( false == text.isEmpty() );
}
