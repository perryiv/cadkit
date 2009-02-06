
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base binder class and supporting makers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SQL_LITE_WRAP_BINDER_H_
#define _SQL_LITE_WRAP_BINDER_H_

#include "Database/SQLite/Export.h"
#include "Database/SQLite/Types.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

struct sqlite3_stmt;
struct sqlite3;


namespace CadKit {
namespace Database {
namespace SQLite {


///////////////////////////////////////////////////////////////////////////////
//
//  Base binder class.
//
///////////////////////////////////////////////////////////////////////////////

struct SQL_LITE_WRAP_EXPORT Binder : public Usul::Base::Referenced
{
  typedef Usul::Base::Referenced BaseClass;
  USUL_DECLARE_TYPE_ID ( Binder );
  USUL_DECLARE_REF_POINTERS ( Binder );

  // Overload to bind the data.
  virtual void      bind ( const std::string &sql, unsigned int which, sqlite3_stmt *, sqlite3 * );

  // Static methods for creating binders.
  static RefPtr     makeBlobBinder ( const void *, unsigned int );
  static RefPtr     makeBlobBinder ( const Blob & );
  static RefPtr     makeTextBinder ( const std::string & );

protected:

  Binder(){}
  virtual ~Binder(){}

private:

  Binder ( const Binder & );
  Binder &operator = ( const Binder & );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Blob binder class.
//
///////////////////////////////////////////////////////////////////////////////

struct SQL_LITE_WRAP_EXPORT BlobBinder : public Binder
{
  typedef Binder BaseClass;
  USUL_DECLARE_TYPE_ID ( BlobBinder );
  USUL_DECLARE_REF_POINTERS ( BlobBinder );
  virtual void bind ( const std::string &sql, unsigned int which, sqlite3_stmt *, sqlite3 * );
  BlobBinder ( const void *b, unsigned int size ) : _blob ( b ), _size ( size ){}
protected:
  virtual ~BlobBinder(){}
private:
  BlobBinder ( const BlobBinder & );
  BlobBinder &operator = ( const BlobBinder & );
  const void * _blob;
  unsigned int _size;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Text binder class.
//
///////////////////////////////////////////////////////////////////////////////

struct SQL_LITE_WRAP_EXPORT TextBinder : public Binder
{
  typedef Binder BaseClass;
  USUL_DECLARE_TYPE_ID ( TextBinder );
  USUL_DECLARE_REF_POINTERS ( TextBinder );
  virtual void bind ( const std::string &sql, unsigned int which, sqlite3_stmt *, sqlite3 * );
  TextBinder ( const std::string &t ) : _text ( t ){}
protected:
  virtual ~TextBinder(){}
private:
  TextBinder ( const TextBinder & );
  TextBinder &operator = ( const TextBinder & );
  const std::string &_text;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Template classes for making binders.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct BinderTraits
{
  static Binder::RefPtr makeBinder ( const T &t )
  {
    // Interpret the bytes of type T as a blob. If T contains pointers or 
    // other complex derived types then this packing will likely be unreliable.
    return Binder::makeBlobBinder ( &t, sizeof ( T ) );
  }
};
template <> struct BinderTraits < CadKit::Database::SQLite::Blob >
{
  static Binder::RefPtr makeBinder ( const CadKit::Database::SQLite::Blob &blob )
  {
    return Binder::makeBlobBinder ( blob );
  }
};
template <> struct BinderTraits < std::string >
{
  static Binder::RefPtr makeBinder ( const std::string &text )
  {
    return Binder::makeTextBinder ( text );
  }
};


} // namespace SQLite
} // namespace Database
} // namespace CadKit


#endif // _SQL_LITE_WRAP_BINDER_H_
