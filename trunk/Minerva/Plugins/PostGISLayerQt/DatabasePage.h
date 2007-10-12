
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

#include "QtGui/QWizardPage"

class QListWidget;
class AddPostGISLayerWidget;

class DatabasePage : public QWizardPage
{
  Q_OBJECT;
public:
  typedef QWizardPage BaseClass;

  DatabasePage ( AddPostGISLayerWidget* );

  virtual void initializePage ();
  virtual bool isComplete () const;

protected:
  void _listTables ();

protected slots:
  void _connectToDatabase ();
  void _selectionChanged ();

private:
  AddPostGISLayerWidget *_widget;
  QListWidget *_listView;
  Minerva::Core::DB::Connection::RefPtr _connection;
};


#endif // __POSTGIS_DATABASE_PAGE_H__
