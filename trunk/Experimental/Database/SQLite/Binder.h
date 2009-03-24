
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
//  Generic binder type intentionally no defined.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct BinderTraits;


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-defined binder types. Make more as needed.
//
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for making a simple object binder.
//
///////////////////////////////////////////////////////////////////////////////

#define SQL_LITE_WRAP_DEFINE_SIMPLE_OBJECT_BINDER(object_type) \
namespace CadKit { namespace Database { namespace SQLite { \
template <> struct BinderTraits < object_type > \
{ \
  static Binder::RefPtr makeBinder ( const object_type &t ) \
  { \
    return Binder::makeBlobBinder ( &t, sizeof ( object_type ) ); \
  } \
}; } } }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for making a complex object binder that reports the size in bytes.
//
///////////////////////////////////////////////////////////////////////////////

#define SQL_LITE_WRAP_DEFINE_COMPLEX_OBJECT_BINDER(object_type) \
namespace CadKit { namespace Database { namespace SQLite { \
template <> struct BinderTraits < object_type > \
{ \
  static Binder::RefPtr makeBinder ( const object_type &t ) \
  { \
    return Binder::makeBlobBinder ( t.getBytes(), t.getNumBytes() ); \
  } \
}; } } }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for making a vector binder.
//
///////////////////////////////////////////////////////////////////////////////

#define SQL_LITE_WRAP_DEFINE_VECTOR_BINDER(vector_type) \
namespace CadKit { namespace Database { namespace SQLite { \
template <> struct BinderTraits < vector_type > \
{ \
  static Binder::RefPtr makeBinder ( const vector_type &v ) \
  { \
    typedef vector_type::value_type ValueType; \
    return Binder::makeBlobBinder ( &v[0], v.size() * sizeof ( ValueType ) ); \
  } \
}; } } }


} // namespace SQLite
} // namespace Database
} // namespace CadKit


///////////////////////////////////////////////////////////////////////////////
//
//  Declare some common vector types.
//
///////////////////////////////////////////////////////////////////////////////

SQL_LITE_WRAP_DEFINE_VECTOR_BINDER ( std::vector < unsigned short > );
SQL_LITE_WRAP_DEFINE_VECTOR_BINDER ( std::vector < unsigned int   > );
SQL_LITE_WRAP_DEFINE_VECTOR_BINDER ( std::vector < unsigned long  > );
SQL_LITE_WRAP_DEFINE_VECTOR_BINDER ( std::vector < char   > );
SQL_LITE_WRAP_DEFINE_VECTOR_BINDER ( std::vector < short  > );
SQL_LITE_WRAP_DEFINE_VECTOR_BINDER ( std::vector < int    > );
SQL_LITE_WRAP_DEFINE_VECTOR_BINDER ( std::vector < long   > );
SQL_LITE_WRAP_DEFINE_VECTOR_BINDER ( std::vector < double > );
SQL_LITE_WRAP_DEFINE_VECTOR_BINDER ( std::vector < float  > );


#endif // _SQL_LITE_WRAP_BINDER_H_
