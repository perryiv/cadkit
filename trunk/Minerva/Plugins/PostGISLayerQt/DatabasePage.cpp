
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/PostGISLayerQt/DatabasePage.h"
#include "Minerva/Plugins/PostGISLayerQt/AddPostGISLayerWidget.h"
#include "Minerva/Plugins/PostGISLayerQt/ConnectToDatabase.h"

#include "Minerva/DataSources/PG/Info.h"
#include "Minerva/Layers/PostGIS/PointLayer.h"
#include "Minerva/Layers/PostGIS/LineLayer.h"
#include "Minerva/Layers/PostGIS/PolygonLayer.h"


#include "QtGui/QListWidget"
#include "QtGui/QVBoxLayout"
#include "QtGui/QHBoxLayout"
#include "QtGui/QPushButton"
#include "QtGui/QLabel"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DatabasePage::DatabasePage ( QWidget * parent ) : BaseClass ( parent ),
  _layer ( 0x0 ),
  _listView ( 0x0 ),
  _connection ( 0x0 )
{
  QVBoxLayout *topLayout ( new QVBoxLayout );
  this->setLayout ( topLayout );

  // Connect to database.
  {
    QPushButton *button ( new QPushButton ( "Connect..." ) );
    connect ( button, SIGNAL ( clicked () ), this, SLOT ( _connectToDatabase () ) );
    topLayout->addWidget ( button );
  }

  _listView = new QListWidget;
  topLayout->addWidget ( _listView );

  // Single select.
  _listView->setSelectionMode ( QListWidget::SingleSelection );

  connect ( _listView, SIGNAL ( itemSelectionChanged () ), this, SLOT ( _selectionChanged () ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DatabasePage::~DatabasePage()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  List all database tables.
//
///////////////////////////////////////////////////////////////////////////////

void DatabasePage::_listTables ()
{
  if ( false == _connection.valid () )
    return;

  // Scope our connection.
  Minerva::DataSources::PG::Connection::ScopedConnection scoped ( *_connection );

  typedef Minerva::DataSources::PG::Info::Strings Strings;

  // Database info class.
  Minerva::DataSources::PG::Info::RefPtr info ( new Minerva::DataSources::PG::Info ( _connection.get () ) );

  Strings tables ( info->geometryTables () );
  for ( Strings::const_iterator iter = tables.begin (); iter != tables.end (); ++iter )
    _listView->addItem ( iter->c_str () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect to a database.
//
///////////////////////////////////////////////////////////////////////////////

void DatabasePage::_connectToDatabase ()
{
  ConnectToDatabase dialog ( this );

  _connection = dialog.exec();

  if ( _connection.valid() )
    this->_listTables ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  The selection has changed.
//
///////////////////////////////////////////////////////////////////////////////

void DatabasePage::_selectionChanged ()
{
  if ( false == _connection.valid() )
    return;

  // Scope our connection.
  Minerva::DataSources::PG::Connection::ScopedConnection scoped ( *_connection );

  // Database info class.
  Minerva::DataSources::PG::Info::RefPtr info ( new Minerva::DataSources::PG::Info ( _connection.get () ) );

  Minerva::Layers::PostGIS::Layer::RefPtr layer ( 0x0 );

  std::string table ( _listView->selectedItems().front()->text().toStdString () );

  if ( info->isPointTable ( table ) )
    layer = new Minerva::Layers::PostGIS::PointLayer;
  else if ( info->isLineTable ( table ) )
    layer = new Minerva::Layers::PostGIS::LineLayer;
  else if ( info->isPolygonTable ( table ) )
    layer = new Minerva::Layers::PostGIS::PolygonLayer;

  if ( layer.valid() )
  {
    layer->connection ( _connection.get() );
    layer->tablename ( table );
  }

  _layer = layer;

  emit layerChanged( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Layers::PostGIS::Layer* DatabasePage::layer ()
{
  return _layer.get();
}
