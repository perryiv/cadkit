
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Create.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Functions/Guid.h"

#include "Usul/Interfaces/IDatabaseConnection.h"


class IUnknownTest : public Usul::Base::Referenced,
                     public Usul::Interfaces::IDatabaseConnection
{
public:
  USUL_DECLARE_QUERY_POINTERS ( IUnknownTest );

  IUnknownTest() :
  Usul::Base::Referenced(),
    _hostname( Usul::Functions::GUID::generate() ),
    _database( Usul::Functions::GUID::generate() ),
    _username( Usul::Functions::GUID::generate() ),
    _password( Usul::Functions::GUID::generate() )
  {
  }

  void ref()
  {
    Usul::Base::Referenced::ref();
  }

  void unref ( bool b = true )
  {
    Usul::Base::Referenced::unref( b );
  }
  
  Usul::Interfaces::IUnknown* queryInterface ( unsigned long iid )
  {
    switch ( iid )
    {
    case Usul::Interfaces::IUnknown::IID:
    case Usul::Interfaces::IDatabaseConnection::IID:
      return static_cast < Usul::Interfaces::IDatabaseConnection* > ( this );
    default:
      return 0x0;
    }
  }

protected:
  virtual ~IUnknownTest()
  {
  }

  /// Get/Set the hostname.
  virtual void                         hostname ( const std::string& h ) { _hostname = h; }
  virtual const std::string&           hostname () const { return _hostname; }

  /// Get/Set the database name.
  virtual void                         database ( const std::string& d ) { _database = d; }
  virtual const std::string&           database () const { return _database; }

  /// Get/Set the username
  virtual void                         username ( const std::string& u ) { _username = u; }
  virtual const std::string&           username () const { return _username; }

  /// Get/Set the password.
  virtual void                         password ( const std::string& p ) { _password = p; }
  virtual const std::string&           password () { return _password; }

  /// Connect.
  virtual void                         connect () {}
  
  /// Disconnect.
  virtual void                         disconnect () {}

private:
  std::string _hostname;
  std::string _database;
  std::string _username;
  std::string _password;

};


Usul::Interfaces::IUnknown* create ()
{
  return new IUnknownTest;
}
