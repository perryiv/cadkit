
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
#include "Usul/Interfaces/ISerialize.h"

#include "Serialize/XML/Macros.h"
#include "Serialize/XML/Factory.h"
#include "Serialize/XML/TypeCreator.h"

class IUnknownTest : public Usul::Base::Referenced,
                     public Usul::Interfaces::IDatabaseConnection,
                     public Usul::Interfaces::ISerialize
{
public:
  USUL_DECLARE_QUERY_POINTERS ( IUnknownTest );

  IUnknownTest() :
  Usul::Base::Referenced(),
    _hostname( Usul::Functions::generateGUID() ),
    _database( Usul::Functions::generateGUID() ),
    _username( Usul::Functions::generateGUID() ),
    _password( Usul::Functions::generateGUID() ),
    _dataMemberMap()
  {
    SERIALIZE_XML_ADD_MEMBER ( _hostname );
    SERIALIZE_XML_ADD_MEMBER ( _database );
    SERIALIZE_XML_ADD_MEMBER ( _username );
    SERIALIZE_XML_ADD_MEMBER ( _password );
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
    case Usul::Interfaces::ISerialize::IID:
      return static_cast < Usul::Interfaces::ISerialize* > ( this );
    default:
      return 0x0;
    }
  }

protected:
  virtual ~IUnknownTest() { }

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

  Serialize::XML::DataMemberMap _dataMemberMap;
  SERIALIZE_XML_DEFINE_MEMBERS ( IUnknownTest );
};


Usul::Interfaces::IUnknown* create ()
{
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<IUnknownTest> ( "IUnknownTest" ) );
  Usul::Interfaces::IUnknown::QueryPtr unknown ( new IUnknownTest );
  return unknown.release();
}