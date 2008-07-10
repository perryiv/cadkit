
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
#include "Minerva/Plugins/WmsLayerQt/OptionsDialog.h"
#include "Minerva/Plugins/WmsLayerQt/WmsLayerItem.h"

#include "Minerva/Core/Layers/RasterGroup.h"

#include "Minerva/Interfaces/IAddLayer.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Network/Curl.h"
#include "Usul/Network/Names.h"
#include "Usul/Registry/Convert.h"
#include "Usul/Registry/Database.h"
#include "Usul/Registry/Qt.h"
#include "Usul/Scope/RemoveFile.h"

#include "XmlTree/Document.h"
#include "XmlTree/XercesLife.h"

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
#include "QtGui/QMessageBox"		

#include <iostream>
#include <fstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Constants for registry.
//
///////////////////////////////////////////////////////////////////////////////

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
  _imageTypes ( 0x0 ),
  _recentServers ( new QStringListModel ),
  _options()
{
  this->setupUi ( this );

  _imageTypes = new QButtonGroup;

  _imageTypes->addButton ( _jpegButton );
  _imageTypes->addButton ( _pngButton );
  _imageTypes->addButton ( _tifButton );
  
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
  QObject::connect ( this, SIGNAL ( serverValid ( bool ) ), viewOptionsButton,  SLOT ( setEnabled ( bool ) ) );
  QObject::connect ( this, SIGNAL ( serverValid ( bool ) ), _jpegButton,        SLOT ( setEnabled ( bool ) ) );
  QObject::connect ( this, SIGNAL ( serverValid ( bool ) ), _pngButton,         SLOT ( setEnabled ( bool ) ) );
  QObject::connect ( this, SIGNAL ( serverValid ( bool ) ), _layersTree,        SLOT ( setEnabled ( bool ) ) );
  
  emit serverValid ( false );

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
  completer->setCompletionMode ( QCompleter::PopupCompletion );
  
  // Set the model.
  completer->setModel ( _recentServers );
  
  // Set the completer.
  _server->setCompleter( completer );
  
  // Set the default options. 
  _options[Usul::Network::Names::REQUEST] = "GetMap";
  _options[Usul::Network::Names::SRS    ] = "EPSG:4326";
  _options[Usul::Network::Names::VERSION] = "1.1.1";
  _options["service"] = "WMS";
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
  
  // Make sure we have a valid interface.
  if ( false == al.valid() )
    return;
  
  const std::string server ( _server->text().toStdString() );
  const std::string cacheDirectory ( _cacheDirectory->text().toStdString() );
  
  // Check for valid state...
  if ( true == server.empty () || true == cacheDirectory.empty() )
    return;
  
  // Get the checked button.
  QAbstractButton *button ( _imageTypes->checkedButton () );
  std::string format ( 0x0 != button ? button->text().toStdString() : "image/jpeg" );
  
  const bool addAllAsGroup      ( Qt::Checked == _addAllAsGroup->checkState() );
  const bool addSelectedAsGroup ( Qt::Checked == _addSelectedAsGroup->checkState() );
  
  if ( addAllAsGroup || addSelectedAsGroup )
  {    
    // Get all the items.
    Items items ( addAllAsGroup ? _layersTree->findItems ( "*", Qt::MatchWildcard ) : _layersTree->selectedItems() );

    al->addLayer ( this->_makeGroup ( items, format ) );
  }
  else
  {
    Extents extents ( 0, 0, 0, 0 );
    
    Items items ( _layersTree->selectedItems() );

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
    
    Layer::ValidRefPtr layer ( this->_makeLayer ( extents, format, layers.join(",").toStdString(), styles.join(",").toStdString() ) );
    
    // Get the name
    const std::string name ( _name->text().toStdString() );
    
    // Set the name.
    layer->name ( false == name.empty() ? name : server );
    
    al->addLayer ( layer.get () );
  }
  
  // Save the server names.
  QStringList recent ( _recentServers->stringList() );
  recent.push_back ( server.c_str() );
  recent.erase ( std::unique ( recent.begin(), recent.end() ), recent.end() );
  Usul::Registry::Database::instance()[Detail::SECTION][Detail::KEY] = recent;
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
  try
  {
    this->_getCapabilities();
  }
  catch ( const std::exception& e )
  {
    QMessageBox::critical ( this, "Error trying to retrieve capabilities", e.what() );
  }
  catch ( ... )
  {
    QMessageBox::critical ( this, "Unknown error", "An unknown error was encountered." );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the capabilities.
//
///////////////////////////////////////////////////////////////////////////////

void AddWmsLayerWidget::_getCapabilities()
{
  const std::string server ( _server->text().toStdString() );
  
  const std::string::size_type pos ( server.find_first_of ( '?' ) );
  const std::string prefix ( ( std::string::npos == pos ) ? "?" : "&" );
  
  // Url request.
  const std::string request ( Usul::Strings::format ( server, prefix, "request=GetCapabilities&service=WMS&version=1.1.1" ) );
  
  // File to download to.
	std::string name ( Usul::File::Temp::file() );
	Usul::Scope::RemoveFile remove ( name );
  
	// Download.
	{
    // Get the timeout.
    const unsigned int timeout ( Usul::Registry::Database::instance()["network_download"]["wms_get_capabilities"]["timeout_milliseconds"].get<unsigned int> ( 60000, true ) );
		Usul::Network::Curl curl ( request, name );
		curl.download( timeout, static_cast<std::ostream*> ( 0x0 ), "" );
	}
  
  // Open the xml document.
	XmlTree::XercesLife life;
  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  document->load ( name );
  
  typedef XmlTree::Node::Children    Children;
  
  // Look for errors first.
  Children exceptions ( document->find ( "ServiceException", true ) );
  
  if ( false == exceptions.empty() )
  {
    std::string message ( Usul::Strings::format ( "Could not retrieve capabilities using request: ", request ) );
    for ( Children::const_iterator iter = exceptions.begin(); iter != exceptions.end(); ++iter )
    {
      message += Usul::Strings::format ( "  Reason: ", (*iter)->value() );
    }
    
    throw std::runtime_error ( message.c_str() );
  }
  
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

void AddWmsLayerWidget::on_viewOptionsButton_clicked()
{
  OptionsDialog dialog ( _options, this );
  
  if ( QDialog::Accepted == dialog.exec() )
    _options = dialog.options();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a WMS layer.
//
///////////////////////////////////////////////////////////////////////////////

AddWmsLayerWidget::Layer* AddWmsLayerWidget::_makeLayer ( const Extents& extents, const std::string& format, const std::string& layers, const std::string& styles ) const
{
  const std::string server ( _server->text().toStdString() );
  const std::string cacheDirectory ( _cacheDirectory->text().toStdString() );
  
  Minerva::Core::Layers::RasterLayerWms::RefPtr layer ( new Minerva::Core::Layers::RasterLayerWms );
  
  // Get the current options.
  Options options ( _options );
  
  options[Usul::Network::Names::LAYERS] = layers;
  options[Usul::Network::Names::STYLES] = styles;
  
  // Set the format.
  options[Usul::Network::Names::FORMAT] = format;
  
  // Set the options.
  layer->options ( options );
  
  // Set the extents.
  layer->extents ( extents );
  
  // Set the base url.
  layer->urlBase ( server );
  
  // Set the cache directory.
  layer->cacheDirectory ( cacheDirectory, Qt::Checked == _makeDefaultDirectory->checkState() );
  
  return layer.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a raster group.
//
///////////////////////////////////////////////////////////////////////////////

AddWmsLayerWidget::Layer* AddWmsLayerWidget::_makeGroup ( const Items& items, const std::string& format ) const
{
  // Get the name
  const std::string name ( _name->text().toStdString() );
  
  // Get the server.
  const std::string server ( _server->text().toStdString() );
  
  // Make a group.
  Minerva::Core::Layers::RasterGroup::RefPtr group ( new Minerva::Core::Layers::RasterGroup );
  group->name ( false == name.empty() ? name : server );

  // Possibly used below in the loop.
  unsigned int count ( 0 );

  for ( Items::const_iterator iter = items.begin(); iter != items.end(); ++iter )
  {
    if ( WmsLayerItem *item = dynamic_cast<WmsLayerItem*> ( *iter ) )
    {
      Layer::ValidRefPtr layer ( this->_makeLayer ( item->extents(), format, item->name().c_str(), item->style().c_str()  ) );
      layer->name ( ( false == item->name().empty() ) ? item->name() : Usul::Strings::format ( group->name(), ": ", ++count ) );
      
      layer->showLayer ( false );
      
      group->append ( layer.get() );
    }
  }
  
  return group.release();
}
