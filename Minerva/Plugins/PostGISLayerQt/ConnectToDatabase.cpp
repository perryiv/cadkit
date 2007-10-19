
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/PostGISLayerQt/ConnectToDatabase.h"

#include "Usul/Registry/Database.h"
#include "Usul/Registry/Convert.h"
#include "Usul/Strings/Split.h"

#include "QtGui/QComboBox"
#include "QtGui/QLineEdit"
#include "QtGui/QVBoxLayout"
#include "QtGui/QHBoxLayout"
#include "QtGui/QPushButton"
#include "QtGui/QLabel"

namespace Detail
{
  const std::string KEY  ( "recent_databases" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Converter for a saved connection.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Registry
  {

    template <> struct Convert < ConnectToDatabase::SavedConnections >
    {
      typedef ConnectToDatabase::SavedConnections SavedConnections;
      typedef std::vector< std::string > Strings;
      static std::string to ( const SavedConnections &sc )
      {
        std::ostringstream out;

        for ( SavedConnections::const_iterator iter = sc.begin(); iter != sc.end(); ++iter )
        {
          out << iter->database << "|" << iter->hostname << "|" << iter->username << "|";
          if ( iter->savePassword )
            out << iter->password;
          out << ":";
        }
        return out.str();
      }
      static void from ( const std::string &s, SavedConnections &sc )
      {
        if ( true == s.empty() )
          return;

        Strings connections;
        Usul::Strings::split ( s, ':', false, connections );

        for ( Strings::const_iterator iter = connections.begin(); iter != connections.end(); ++ iter )
        {
          Strings strings;
          Usul::Strings::split ( *iter, '|', true, strings );

          if ( 4 == strings.size() )
          {
            SavedConnections::value_type saved;
            saved.database = strings[0];
            saved.hostname = strings[1];
            saved.username = strings[2];
            saved.password = strings[3];
            sc.push_back ( saved );
          }
        }
      }
    };
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ConnectToDatabase::ConnectToDatabase ( QWidget * widget ) :
BaseClass ( widget ),
_recentConnections ( new QComboBox ),
_hostname ( new QLineEdit ),
_database ( new QLineEdit ),
_username ( new QLineEdit ),
_password ( new QLineEdit ),
_savedConnections ()
{
  QVBoxLayout *topLayout ( new QVBoxLayout );
  this->setLayout ( topLayout );

  _password->setEchoMode ( QLineEdit::Password );

  // Recent connections
  {
    topLayout->addWidget ( _recentConnections );
    connect ( _recentConnections, SIGNAL ( currentIndexChanged ( int ) ), this, SLOT ( _recentConnectionChanged ( int ) ) );
  }

  // Hostname.
  {
    QHBoxLayout *layout ( new QHBoxLayout );
    QLabel *label ( new QLabel ( "Hostname:" ) );
    layout->addWidget ( label );
    layout->addWidget ( _hostname );
    topLayout->addLayout ( layout );
  }

  // Database.
  {
    QHBoxLayout *layout ( new QHBoxLayout );
    QLabel *label ( new QLabel ( "Database:" ) );
    layout->addWidget ( label );
    layout->addWidget ( _database );
    topLayout->addLayout ( layout );
  }

  // Username.
  {
    QHBoxLayout *layout ( new QHBoxLayout );
    QLabel *label ( new QLabel ( "Username:" ) );
    layout->addWidget ( label );
    layout->addWidget ( _username );
    topLayout->addLayout ( layout );
  }

  // Password.
  {
    QHBoxLayout *layout ( new QHBoxLayout );
    QLabel *label ( new QLabel ( "Password:" ) );
    layout->addWidget ( label );
    layout->addWidget ( _password );
    topLayout->addLayout ( layout );
  }

  topLayout->addStretch ();

  // Ok and cancel buttons.
  {
    QPushButton *ok ( new QPushButton ( "Ok" ) );
    QPushButton *cancel ( new QPushButton ( "Cancel" ) );

    connect ( ok,     SIGNAL ( clicked () ), this, SLOT ( accept () ) );
    connect ( cancel, SIGNAL ( clicked () ), this, SLOT ( reject () ) );

    QHBoxLayout *layout ( new QHBoxLayout );
    
    topLayout->addLayout ( layout );
    
    layout->addStretch  ();
    layout->addWidget ( ok );
    layout->addWidget ( cancel );
  }

  this->setModal ( true );
  this->setWindowTitle ( "Connect to Database." );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the dialog.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::DB::Connection* ConnectToDatabase::exec ()
{
  Minerva::Core::DB::Connection::RefPtr connection ( 0x0 );

  this->_restoreConnections();

  if ( QDialog::Accepted == BaseClass::exec() )
  {
    connection = new Minerva::Core::DB::Connection;
    connection->hostname ( _hostname->text().toStdString () );
    connection->database ( _database->text().toStdString () );
    connection->username ( _username->text().toStdString () );
    connection->password ( _password->text().toStdString () );

    SavedConnection saved;
    saved.hostname = connection->hostname();
    saved.database = connection->database();
    saved.password = connection->password();
    saved.username = connection->username();
    saved.savePassword = false;
    _savedConnections.push_back ( saved );

    this->_saveConnections ();
  }

  return connection.release ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Restore saved connections.
//
///////////////////////////////////////////////////////////////////////////////

void ConnectToDatabase::_restoreConnections ()
{
  _savedConnections = Usul::Registry::Database::instance()[ Detail::KEY ].get < SavedConnections > ( SavedConnections () );

  for ( SavedConnections::const_iterator iter = _savedConnections.begin(); iter != _savedConnections.end(); ++iter )
  {
    std::string name ( iter->database + " on " + iter->hostname );
    _recentConnections->addItem ( name.c_str() );
  }

  _recentConnections->setCurrentIndex ( -1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save connections.
//
///////////////////////////////////////////////////////////////////////////////

void ConnectToDatabase::_saveConnections ()
{
  SavedConnections saved ( _savedConnections.begin(), std::unique ( _savedConnections.begin(), _savedConnections.end() ) );
  Usul::Registry::Database::instance()[ Detail::KEY ] = saved;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Recent connection has changed.
//
///////////////////////////////////////////////////////////////////////////////

void ConnectToDatabase::_recentConnectionChanged( int index )
{
  _hostname->setText ( "" );
  _database->setText ( "" );
  _username->setText ( "" );
  _password->setText ( "" );

  if ( index >= 0 && index < static_cast < int > ( _savedConnections.size() ) )
  {
    SavedConnection saved ( _savedConnections.at ( index ) );
    _hostname->setText ( saved.hostname.c_str() );
    _database->setText ( saved.database.c_str() );
    _username->setText ( saved.username.c_str() );
    _password->setText ( saved.password.c_str() );
  }
}
