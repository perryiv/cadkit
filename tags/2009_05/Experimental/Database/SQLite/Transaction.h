
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for managing transactions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SQL_LITE_WRAP_TRANSACTION_CLASS_H_
#define _SQL_LITE_WRAP_TRANSACTION_CLASS_H_


namespace CadKit {
namespace Database {
namespace SQLite {


template < class ConnectionPtr > struct Transaction
{
  typedef typename ConnectionPtr::element_type Connection;
  typedef typename Connection::Mutex Mutex;
  typedef typename Connection::Guard Guard;

private:

  static ConnectionPtr _checkConnection ( ConnectionPtr c )
  {
    if ( false == c.valid() )
      throw std::invalid_argument ( "Error 3653216415: Cannot have transaction with null connection" );
    return c;
  }

  void _finalize ( const std::string &command )
  {
    ConnectionPtr c ( _c );
    _c = 0x0;

    if ( true == c.valid() )
    {
      c->execute ( command );
    }
  }

public:

  // Lock because it's real easy to get nested transations with multiple threads.
  Transaction ( ConnectionPtr c ) : _c ( _checkConnection ( c ) ), _guard ( _checkConnection ( c )->mutex() )
  {
    _c->execute ( "begin transaction" );
  }

  ~Transaction()
  {
    this->_finalize ( "rollback" );
  }

  void commit()
  {
    this->_finalize ( "commit" );
  }

private:

  ConnectionPtr _c;
  Guard _guard;
};


} // namespace SQLite
} // namespace Database
} // namespace CadKit


#endif // _SQL_LITE_WRAP_TRANSACTION_CLASS_H_
