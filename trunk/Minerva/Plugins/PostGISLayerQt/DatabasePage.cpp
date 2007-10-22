
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

#include "Minerva/Core/DB/Info.h"
#include "Minerva/Core/Layers/PointLayer.h"
#include "Minerva/Core/Layers/PointTimeLayer.h"
#include "Minerva/Core/Layers/LineLayer.h"
#include "Minerva/Core/Layers/PolygonLayer.h"


#include "QtGui/QListWidget.h"
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
  Minerva::Core::DB::Connection::ScopedConnection scoped ( *_connection );

  typedef Minerva::Core::DB::Info::Strings Strings;

  // Database info class.
  Minerva::Core::DB::Info::RefPtr info ( new Minerva::Core::DB::Info ( _connection.get () ) );

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

  if ( _connection.valid () )
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
  Minerva::Core::DB::Connection::ScopedConnection scoped ( *_connection );

  // Database info class.
  Minerva::Core::DB::Info::RefPtr info ( new Minerva::Core::DB::Info ( _connection.get () ) );

  Minerva::Core::Layers::Layer::RefPtr layer ( 0x0 );

  std::string table ( _listView->selectedItems().front()->text().toStdString () );

  // Check time classes first...
  if( info->isPointTimeTable ( table ) )
    layer = new Minerva::Core::Layers::PointTimeLayer;
  else if ( info->isPointTable ( table ) )
    layer = new Minerva::Core::Layers::PointLayer;
  else if ( info->isLineTable ( table ) )
    layer = new Minerva::Core::Layers::LineLayer;
  else if ( info->isPolygonTable ( table ) )
    layer = new Minerva::Core::Layers::PolygonLayer;

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

Minerva::Core::Layers::Layer* DatabasePage::layer ()
{
  return _layer.get();
}
