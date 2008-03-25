
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
#include "Usul/Registry/Convert.h"
#include "Usul/Registry/Database.h"
#include "Usul/Registry/Qt.h"

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
#include "QtGui/QScrollArea"
#include "QtGui/QStringListModel"
#include "QtGui/QCompleter"

#include <iostream>


namespace Detail
{
  const std::string SECTION ( "add_wms_server" );
  const std::string KEY     ( "recent_servers" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddWmsLayerWidget::AddWmsLayerWidget( QWidget *parent ) : BaseClass ( parent ),
  _options(),
  _imageTypes ( 0x0 ),
  _optionsWidget ( new QWidget ),
  _recentServers ( new QStringListModel )
{
  this->setupUi ( this );

  _imageTypes = new QButtonGroup;

  _imageTypes->addButton ( _jpegButton );
  _imageTypes->addButton ( _pngButton );
  
  QScrollArea *scrollArea ( new QScrollArea );
  _scrollOptionsWidget->setLayout ( new QVBoxLayout );
  _scrollOptionsWidget->layout()->addWidget ( scrollArea );
  scrollArea->setWidget ( _optionsWidget );
  scrollArea->setWidgetResizable ( true );

  _optionsWidget->setLayout ( new QVBoxLayout );
  
  _layersTree->setColumnCount( 2 );
  
  QStringList titles;
  titles.push_back( "Name" );
  titles.push_back( "Title");
  _layersTree->setHeaderLabels ( titles );
  
  // We want exteneded selection.
  _layersTree->setSelectionMode ( QAbstractItemView::ExtendedSelection );
  
  _cacheDirectory->setText ( Minerva::Core::Layers::RasterLayerWms::defaultCacheDirectory().c_str() );
  
  QObject::connect ( _server, SIGNAL ( textChanged ( const QString& ) ), this, SLOT ( _onServerTextChanged ( const QString& ) ) );

  QObject::connect ( this, SIGNAL ( serverValid ( bool ) ), capabilitiesButton, SLOT ( setEnabled ( bool ) ) );
  QObject::connect ( this, SIGNAL ( serverValid ( bool ) ), addOptionButton,    SLOT ( setEnabled ( bool ) ) );
  QObject::connect ( this, SIGNAL ( serverValid ( bool ) ), _jpegButton,        SLOT ( setEnabled ( bool ) ) );
  QObject::connect ( this, SIGNAL ( serverValid ( bool ) ), _pngButton,         SLOT ( setEnabled ( bool ) ) );
  QObject::connect ( this, SIGNAL ( serverValid ( bool ) ), _layersTree,        SLOT ( setEnabled ( bool ) ) );
  
  emit serverValid ( false );
  
#ifndef _DEBUG
  _optionsGroupBox->setVisible ( false );
#endif
  
  // Get recent-server list.
  QStringList recent ( Usul::Registry::Database::instance()[Detail::SECTION][Detail::KEY].get<QStringList> ( QStringList() ) );
  
  // Sort.
  recent.sort();
  
  // Make sure all are unique.
  recent.erase ( std::unique ( recent.begin(), recent.end() ), recent.end() );
  
  // Se the model's string list.
  _recentServers->setStringList ( recent );
  
  // Make the completer.
  QCompleter *completer ( new QCompleter );
  
  // Set the model.
  completer->setModel ( _recentServers );
  
  // Set the completer.
  _server->setCompleter( completer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AddWmsLayerWidget::~AddWmsLayerWidget()
{
  // Save the server names.
  Usul::Registry::Database::instance()[Detail::SECTION][Detail::KEY] = _recentServers->stringList();
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
    Items items ( _layersTree->selectedItems() );
    
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
    
    // Add user specified options.
    for ( Options::const_iterator iter = _options.begin(); iter != _options.end(); ++iter )
    {
      std::string key ( (*iter)->key() );
      std::string value ( (*iter)->value() );
       
      if ( false == key.empty() )
      {
        options[key] = value;
      }
    }
    
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

  _layersTree->clear();
  
  // Return now if we don't have any layers.
  if ( true == layers.empty() )
    return;
  
  WmsLayerItem::Extents defaultExtents ( WmsLayerItem::parseExtents ( *layers.front() ) );
  
  for ( Children::const_iterator iter = layers.begin() + 1; iter != layers.end(); ++iter )
  {
    QTreeWidgetItem *item ( new WmsLayerItem ( (*iter).get(), defaultExtents, _layersTree ) );
    _layersTree->addTopLevelItem( item );
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
  //names.push_back ( Usul::Network::Names::STYLES  );
  //names.push_back ( Usul::Network::Names::LAYERS  );
  names.push_back ( Usul::Network::Names::VERSION );
  
  OptionWidget *widget ( new OptionWidget ( names, _optionsWidget ) );
  _optionsWidget->layout()->addWidget( widget );
  
  _options.push_back ( widget );
}
