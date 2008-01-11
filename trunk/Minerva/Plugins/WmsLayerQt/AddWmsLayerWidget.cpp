
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Config.h"

#include "Minerva/Plugins/WmsLayerQt/CompileGuard.h"
#include "Minerva/Plugins/WmsLayerQt/AddWmsLayerWidget.h"
#include "Minerva/Plugins/WmsLayerQt/OptionWidget.h"

#if USE_STAR_SYSTEM
#include "StarSystem/RasterLayerWms.h"
#else
#include "Minerva/Plugins/WmsLayerQt/WmsLayer.h"
#endif

#include "Minerva/Interfaces/IAddLayer.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Network/Names.h"

#include "QtGui/QLabel"
#include "QtGui/QLineEdit"
#include "QtGui/QHBoxLayout"
#include "QtGui/QVBoxLayout"
#include "QtGui/QListWidget"
#include "QtGui/QPushButton"
#include "QtGui/QRadioButton"
#include "QtGui/QButtonGroup"
#include "QtGui/QFileDialog"

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
  
#if USE_STAR_SYSTEM
  _cacheDirectory->setText( StarSystem::RasterLayerWms::defaultCacheDirectory().c_str() );
#endif
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
#if USE_STAR_SYSTEM
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
#else
    WmsLayer::RefPtr layer ( new WmsLayer );
    layer->server ( server );
    

    // Set the image type.
    layer->imageType ( format );

#endif
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
