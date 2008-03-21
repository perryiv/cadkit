
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#define NOMINMAX
#endif

#include "Minerva/Plugins/WmsLayerQt/CompileGuard.h"
#include "Minerva/Plugins/WmsLayerQt/AddWmsLayerWidget.h"
#include "Minerva/Plugins/WmsLayerQt/OptionWidget.h"
#include "Minerva/Plugins/WmsLayerQt/WmsLayerItem.h"

#include "Minerva/Core/Layers/RasterLayerWms.h"

#include "Minerva/Interfaces/IAddLayer.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Contents.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Network/Curl.h"
#include "Usul/Network/Names.h"

#include "XmlTree/Document.h"

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
#include "QtGui/QTreeWidget"
#include "QtGui/QHeaderView"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddWmsLayerWidget::AddWmsLayerWidget( QWidget *parent ) : BaseClass ( parent ),
  _imageTypes ( 0x0 ),
  _tree ( new QTreeWidget )
{
  this->setupUi ( this );

  _imageTypes = new QButtonGroup;

  _imageTypes->addButton ( _jpegButton );
  _imageTypes->addButton ( _pngButton );
  
  _optionsWidget->setLayout ( new QVBoxLayout );
  _optionsWidget->layout()->addWidget( _tree );
  
  _tree->setColumnCount( 2 );
  
  QStringList titles;
  titles.push_back( "Name" );
  titles.push_back( "Title");
  _tree->setHeaderLabels ( titles );
  
  // We want exteneded selection.
  _tree->setSelectionMode ( QAbstractItemView::ExtendedSelection );
  
  _cacheDirectory->setText ( Minerva::Core::Layers::RasterLayerWms::defaultCacheDirectory().c_str() );
  
  QObject::connect ( _server, SIGNAL ( textChanged ( const QString& ) ), this, SLOT ( _onServerTextChanged ( const QString& ) ) );

  QObject::connect ( this, SIGNAL ( serverValid ( bool ) ), capabilitiesButton, SLOT ( setEnabled ( bool ) ) );
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

void AddWmsLayerWidget::apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown * caller )
{
  Minerva::Interfaces::IAddLayer::QueryPtr al ( parent );

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
    Minerva::Core::Layers::RasterLayerWms::Extents extents ( 0, 0, 0, 0 );
    
    Minerva::Core::Layers::RasterLayerWms::Options options;
    
    options[Usul::Network::Names::REQUEST] = "GetMap";
    options[Usul::Network::Names::SRS    ] = "EPSG:4326";
    options[Usul::Network::Names::VERSION] = "1.1.1";

    typedef QList<QTreeWidgetItem *> Items;
    Items items ( _tree->selectedItems() );
    
    if ( true == items.isEmpty() )
      return;
    
    QStringList layers;
    QStringList styles;
    
    for ( Items::const_iterator iter = items.begin(); iter != items.end(); ++iter )
    {
      if ( WmsLayerItem *item = dynamic_cast<WmsLayerItem*> ( *iter ) )
      {
        layers.push_back( item->name().c_str() );
        styles.push_back( item->style().c_str() );
        
        extents.expand ( item->extents() );
      }
    }
    
    options[Usul::Network::Names::LAYERS] = layers.join(",").toStdString();
    options[Usul::Network::Names::STYLES] = styles.join(",").toStdString();
    
    options[Usul::Network::Names::FORMAT] = format;
    
    // Make the layer
    Minerva::Core::Layers::RasterLayerWms::RefPtr layer ( new Minerva::Core::Layers::RasterLayerWms ( extents, server, options ) );

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
//  Get the capabilites of the server.
//
///////////////////////////////////////////////////////////////////////////////

void AddWmsLayerWidget::on_capabilitiesButton_clicked()
{
  // Url request.
  std::string request ( _server->text().toStdString() + "?request=GetCapabilities&Service=WMS&Version=1.1.1" );
  
  // File to download to.
  Usul::File::Temp temp;
  
  // Download.
  Usul::Network::Curl curl ( request, temp.name() );
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( &curl, &Usul::Network::Curl::download ), static_cast<std::ostream*> ( 0x0 ), "3034499311" );
  
  // Open the xml document.
  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  document->load ( temp.name() );
  
  typedef XmlTree::Node::Children    Children;
  
  Children layers ( document->find ( "Layer", true ) );

  _tree->clear();
  
  for ( Children::const_iterator iter = layers.begin(); iter != layers.end(); ++iter )
  {
    QTreeWidgetItem *item ( new WmsLayerItem ( (*iter).get(), _tree ) );
    _tree->addTopLevelItem( item );
  }
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
