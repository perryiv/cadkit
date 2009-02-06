
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
  Transaction ( ConnectionPtr c ) : _c ( c )
  {
    if ( true == _c.valid() )
    {
      _c->execute ( "begin transaction" );
    }
  }

  ~Transaction()
  {
    ConnectionPtr c ( _c );
    _c = 0x0;

    if ( true == c.valid() )
    {
      c->execute ( "rollback" );
    }
  }

  void commit()
  {
    ConnectionPtr c ( _c );
    _c = 0x0;

    if ( true == c.valid() )
    {
      c->execute ( "commit" );
    }
  }

private:

  ConnectionPtr _c;
};


} // namespace SQLite
} // namespace Database
} // namespace CadKit


#endif // _SQL_LITE_WRAP_TRANSACTION_CLASS_H_
