
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __POSTGIS_DATABASE_PAGE_H__
#define __POSTGIS_DATABASE_PAGE_H__

#include "Minerva/Core/DB/Connection.h"
#include "Minerva/Core/Layers/Layer.h"

#include "QtGui/QWidget"

class QListWidget;

class DatabasePage : public QWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  typedef Minerva::Core::Layers::Layer Layer;

  DatabasePage  ( QWidget * parent = 0x0 );
  ~DatabasePage ();

  /// Get the layer.
  Layer*                  layer ();

signals:
  void layerChanged( bool );

protected:
  void _listTables ();

protected slots:
  void _connectToDatabase ();
  void _selectionChanged ();

private:
  Layer::RefPtr   _layer;
  QListWidget    *_listView;
  Minerva::Core::DB::Connection::RefPtr _connection;
};


#endif // __POSTGIS_DATABASE_PAGE_H__
