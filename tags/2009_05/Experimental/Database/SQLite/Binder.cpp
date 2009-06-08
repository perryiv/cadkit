
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base binder class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Database/SQLite/Binder.h"
#include "Database/SQLite/Internal.h"

#include "Usul/Exceptions/Exception.h"

#include "boost/bind.hpp"

using namespace CadKit::Database::SQLite;

USUL_IMPLEMENT_TYPE_ID ( Binder );
USUL_IMPLEMENT_TYPE_ID ( BlobBinder );
USUL_IMPLEMENT_TYPE_ID ( TextBinder );


///////////////////////////////////////////////////////////////////////////////
//
//  Empty binder.
//
///////////////////////////////////////////////////////////////////////////////

void Binder::bind ( const std::string &, unsigned int, sqlite3_stmt *, sqlite3 * )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Blob binder. See http://www.sqlite.org/cvstrac/wiki?p=BlobExample
//
///////////////////////////////////////////////////////////////////////////////

void BlobBinder::bind ( const std::string &sql, unsigned int which, sqlite3_stmt *statement, sqlite3 *database )
{
  if ( ( 0x0 == statement ) || ( 0x0 == database ) )
    return;

  Helper::call ( boost::bind<int> ( ::sqlite3_bind_blob, statement, which + 1, _blob, _size, SQLITE_STATIC ), 
    "Error 2401614108: Failed to bind blob data", sql, database );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Text binder. See http://www.sqlite.org/cvstrac/wiki?p=BlobExample
//
///////////////////////////////////////////////////////////////////////////////

void TextBinder::bind ( const std::string &sql, unsigned int which, sqlite3_stmt *statement, sqlite3 *database )
{
  if ( ( 0x0 == statement ) || ( 0x0 == database ) )
    return;

  const char *text ( ( false == _text.empty() ) ? _text.c_str() : "" );
  Helper::call ( boost::bind<int> ( ::sqlite3_bind_text, statement, which + 1, text, _text.size(), SQLITE_STATIC ),
    "Error 2735904180: Failed to bind text data", sql, database );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a blob binder.
//
///////////////////////////////////////////////////////////////////////////////

Binder::RefPtr Binder::makeBlobBinder ( const void *data, unsigned int size )
{
  return Binder::RefPtr ( new BlobBinder ( data, size ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a blob binder.
//
///////////////////////////////////////////////////////////////////////////////

Binder::RefPtr Binder::makeBlobBinder ( const Blob &blob )
{
  const void *data        ( ( false == blob.empty() ) ? &blob[0] : 0x0 );
  const unsigned int size ( ( false == blob.empty() ) ? blob.size() : 0 );
  return Binder::makeBlobBinder ( data, size );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a text binder.
//
///////////////////////////////////////////////////////////////////////////////

Binder::RefPtr Binder::makeTextBinder ( const std::string &text )
{
  return Binder::RefPtr ( new TextBinder ( text ) );
}
