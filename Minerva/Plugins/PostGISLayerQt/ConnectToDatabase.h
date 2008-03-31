
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CONNECT_TO_DATABASE_H__
#define __CONNECT_TO_DATABASE_H__

#include "Minerva/DataSources/PG/Connection.h"

#include "QtGui/QDialog"

class QLineEdit;
class QComboBox;

class ConnectToDatabase : public QDialog
{
  Q_OBJECT;
public:
  struct SavedConnection
  {
    std::string hostname;
    std::string database;
    std::string username;
    std::string password;
    bool savePassword;

    std::string name () const
    {
      return ( database + " on " + hostname );
    }

    bool operator== ( const SavedConnection &rhs ) const
    {
      return ( hostname == rhs.hostname && database == rhs.database );
    }

    bool operator < ( const SavedConnection& rhs ) const
    {
      return this->name() < rhs.name();
    }
  };

  typedef QDialog BaseClass;
  typedef Minerva::DataSources::PG::Connection Connection;
  typedef std::vector < SavedConnection > SavedConnections;

  ConnectToDatabase ( QWidget *parent );
  
  // Show the dialog.  Connection may be null.
  Connection*   exec ();

protected:
  void _ensureUniqueConnections();
  void _restoreConnections();
  void _populateComboBox ();
  void _saveConnections();

protected slots:
  void _recentConnectionChanged( int index );

private:

  QComboBox *_recentConnections;
  QLineEdit *_hostname;
  QLineEdit *_database;
  QLineEdit *_username;
  QLineEdit *_password;
  SavedConnections _savedConnections;
};


#endif // __CONNECT_TO_DATABASE_H__
