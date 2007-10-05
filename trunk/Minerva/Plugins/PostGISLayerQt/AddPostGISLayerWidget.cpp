
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/PostGISLayerQt/AddPostGISLayerWidget.h"

#include "Minerva/Core/DB/Info.h"

#include "Minerva/Interfaces/IAddLayer.h"

#include "QtGui/QVBoxLayout"
#include "QtGui/QHBoxLayout"
#include "QtGui/QPushButton"
#include "QtGui/QLabel"
#include "QtGui/QLineEdit"
#include "QtGui/QDialog.h"
#include "QtGui/QListWidget.h"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddPostGISLayerWidget::AddPostGISLayerWidget( QWidget *parent ) : BaseClass ( parent ),
_connection ( 0x0 ),
_listView ( 0x0 )
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AddPostGISLayerWidget::~AddPostGISLayerWidget()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add images to caller.
//
///////////////////////////////////////////////////////////////////////////////

void AddPostGISLayerWidget::apply ( Usul::Interfaces::IUnknown * caller )
{
  Minerva::Interfaces::IAddLayer::QueryPtr al ( caller );

  if ( false == al.valid () )
    return;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect to a database.
//
///////////////////////////////////////////////////////////////////////////////

void AddPostGISLayerWidget::_connectToDatabase ()
{
  //QDialog *dialog ( new QDialog ( mw.valid() ? mw->mainWindow() : 0x0 ) );
  QDialog *dialog ( new QDialog ( this ) );

  QVBoxLayout *topLayout ( new QVBoxLayout );
  dialog->setLayout ( topLayout );

  QLineEdit *hostname ( new QLineEdit );
  QLineEdit *database ( new QLineEdit );
  QLineEdit *username ( new QLineEdit );
  QLineEdit *password ( new QLineEdit );
  password->setEchoMode ( QLineEdit::Password );

  // Hostname.
  {
    QHBoxLayout *layout ( new QHBoxLayout );
    QLabel *label ( new QLabel ( "Hostname:" ) );
    layout->addWidget ( label );
    layout->addWidget ( hostname );
    topLayout->addLayout ( layout );
  }

  // Database.
  {
    QHBoxLayout *layout ( new QHBoxLayout );
    QLabel *label ( new QLabel ( "Database:" ) );
    layout->addWidget ( label );
    layout->addWidget ( database );
    topLayout->addLayout ( layout );
  }

  // Username.
  {
    QHBoxLayout *layout ( new QHBoxLayout );
    QLabel *label ( new QLabel ( "Username:" ) );
    layout->addWidget ( label );
    layout->addWidget ( username );
    topLayout->addLayout ( layout );
  }

  // Password.
  {
    QHBoxLayout *layout ( new QHBoxLayout );
    QLabel *label ( new QLabel ( "Password:" ) );
    layout->addWidget ( label );
    layout->addWidget ( password );
    topLayout->addLayout ( layout );
  }

  topLayout->addStretch ();

  // Ok and cancel buttons.
  {
    QPushButton *ok ( new QPushButton ( "Ok" ) );
    QPushButton *cancel ( new QPushButton ( "Cancel" ) );

    connect ( ok,     SIGNAL ( clicked () ), dialog, SLOT ( accept () ) );
    connect ( cancel, SIGNAL ( clicked () ), dialog, SLOT ( reject () ) );

    QHBoxLayout *layout ( new QHBoxLayout );
    
    topLayout->addLayout ( layout );
    
    layout->addStretch  ();
    layout->addWidget ( ok );
    layout->addWidget ( cancel );
  }

  dialog->setModal ( true );
  
  if ( QDialog::Accepted == dialog->exec() )
  {
    Minerva::Core::DB::Connection::RefPtr connection ( new Minerva::Core::DB::Connection );
    connection->hostname ( hostname->text().toStdString () );
    connection->database ( database->text().toStdString () );
    connection->username ( username->text().toStdString () );
    connection->password ( password->text().toStdString () );

    _connection = connection;

    this->_listTables ();
  }

  delete dialog;
}


///////////////////////////////////////////////////////////////////////////////
//
//  List all database tables.
//
///////////////////////////////////////////////////////////////////////////////

void AddPostGISLayerWidget::_listTables ()
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
