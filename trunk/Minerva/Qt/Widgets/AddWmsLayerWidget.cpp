
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Qt/Widgets/AddWmsLayerWidget.h"
#include "Minerva/Qt/Widgets/OptionsDialog.h"
#include "Minerva/Qt/Widgets/WmsLayerItem.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Network/Names.h"
#include "Usul/Registry/Convert.h"
#include "Usul/Registry/Database.h"
#include "Usul/Registry/Qt.h"

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


using namespace Minerva::Widgets;


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

BaseAddNetworkLayerWidget::BaseAddNetworkLayerWidget ( const Options& options, const std::string& defaultCacheDirectory, QWidget *parent ) : BaseClass ( parent ),
  _imageTypes ( 0x0 ),
  _recentServers ( new QStringListModel ),
  _options ( options )
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

  // Set the cache directory.
  _cacheDirectory->setText ( defaultCacheDirectory.c_str() );
  
  // Connect the slots and signals.
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
  _server->setCompleter ( completer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseAddNetworkLayerWidget::~BaseAddNetworkLayerWidget()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Browse for a directory.
//
///////////////////////////////////////////////////////////////////////////////

void BaseAddNetworkLayerWidget::on_browseDirectory_clicked()
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

void BaseAddNetworkLayerWidget::on_capabilitiesButton_clicked()
{
  try
  {
    const std::string server ( _server->text().toStdString() );
    
    LayerInfos infos ( this->_getCapabilities() );
    
    for ( LayerInfos::const_iterator iter = infos.begin(); iter != infos.end(); ++iter )
    {
      QTreeWidgetItem *item ( new WmsLayerItem ( iter->name, iter->title, iter->extents, _layersTree ) );
      _layersTree->addTopLevelItem( item );
    }
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
//  Called when the server text has changed.
//
///////////////////////////////////////////////////////////////////////////////

void BaseAddNetworkLayerWidget::_onServerTextChanged ( const QString& text )
{
  emit serverValid ( false == text.isEmpty() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add an option.
//
///////////////////////////////////////////////////////////////////////////////

void BaseAddNetworkLayerWidget::on_viewOptionsButton_clicked()
{
  OptionsDialog dialog ( _options, this );
  
  if ( QDialog::Accepted == dialog.exec() )
    _options = dialog.options();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cache directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string BaseAddNetworkLayerWidget::cacheDirectory() const
{
  return _cacheDirectory->text().toStdString();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the server.
//
///////////////////////////////////////////////////////////////////////////////

std::string BaseAddNetworkLayerWidget::server() const
{
  return _server->text().toStdString();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add recent server.
//
///////////////////////////////////////////////////////////////////////////////

void BaseAddNetworkLayerWidget::_addRecentServer ( const std::string& server )
{
  // Save the server names.
  QStringList recent ( _recentServers->stringList() );
  recent.push_back ( server.c_str() );
  recent.erase ( std::unique ( recent.begin(), recent.end() ), recent.end() );
  Usul::Registry::Database::instance()[Detail::SECTION][Detail::KEY] = recent;
}
