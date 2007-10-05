
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADD_POSTGIS_LAYER_WIDGET_H__
#define __ADD_POSTGIS_LAYER_WIDGET_H__

#include "Minerva/Plugins/PostGISLayerQt/CompileGuard.h"
#include "Minerva/Core/DB/Connection.h"

#include "QtGui/QWidget"

#include <vector>
#include <string>

class QListWidget;

namespace Usul { namespace Interfaces { struct IUnknown; } }

class AddPostGISLayerWidget : public QWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;

  AddPostGISLayerWidget( QWidget *parent = 0x0 );
  virtual ~AddPostGISLayerWidget();

  void apply ( Usul::Interfaces::IUnknown * caller );

protected:
  void _listTables ();

protected slots:
  void _connectToDatabase ();

private:
  Minerva::Core::DB::Connection::RefPtr _connection;
  QListWidget *_listView;
};


#endif // __ADD_POSTGIS_LAYER_WIDGET_H__
