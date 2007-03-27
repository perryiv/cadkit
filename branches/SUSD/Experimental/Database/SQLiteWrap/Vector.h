
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Vector class that uses SQLite for storage. Use for very large vectors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SQL_LITE_WRAP_VECTOR_H_
#define _SQL_LITE_WRAP_VECTOR_H_

#include "SQLiteWrap/Database.h"
#include "SQLiteWrap/Callback.h"
#include "SQLiteWrap/TypeTraits.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Types/Types.h"

#include <string>


namespace SQLiteWrap {


namespace Detail
{
  const std::string VECTOR_TABLE ( "vector_table" );
  const std::string INDEX_COLUMN ( "index_column" );
  const std::string DATA_COLUMN  ( "data_column" );
}


template < class ValueType_ > class Vector
{
public:


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef ValueType_ ValueType;
  typedef Usul::Types::Uint64 SizeType;
  typedef Vector<ValueType> ThisType;
  typedef SQLiteWrap::TypeTraits<ValueType> TypeTraits;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector() : _db ( 0x0 ), _value ( 0x0 )
  {
    this->_init();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector ( SizeType s ) : _db ( 0x0 ), _value ( 0x0 )
  {
    this->_init();
    this->resize ( s );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector ( const Vector &v ) : _db ( 0x0 ), _value ( 0x0 )
  {
    this->_init();
    this->assign ( v );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ~Vector()
  {
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &ThisType::_dropTable ), "2273851116" );
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &ThisType::_clean     ), "1671368534" );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Clear all rows of the database table.
  //
  /////////////////////////////////////////////////////////////////////////////

  void clear()
  {
    this->_dropTable();
    this->_createTable();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the value.
  //
  /////////////////////////////////////////////////////////////////////////////

  void get ( SizeType i, ValueType &v ) const
  {
    GetCallback::ValidRefPtr cb ( new GetCallback );
    _db->callback ( cb.get() );
    _db << "select " << Detail::DATA_COLUMN 
        << " from " << Detail::VECTOR_TABLE
        << " where " << Detail::INDEX_COLUMN << " = " << i;
    _db->execute();
    cb->get ( v );
    _db->callback ( 0x0 );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the value.
  //
  /////////////////////////////////////////////////////////////////////////////

  ValueType get ( SizeType i ) const
  {
    ValueType value = ValueType();
    this->get ( i, value );
    return value;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the value.
  //
  /////////////////////////////////////////////////////////////////////////////

  void set ( SizeType i, const ValueType &v )
  {
    _db << "update " << Detail::VECTOR_TABLE 
        << " set " << Detail::DATA_COLUMN << " = " << v
        << " where " << Detail::INDEX_COLUMN << " = " << i;
    _db->execute();  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Resize the vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  void resize ( SizeType s )
  {
    const SizeType current ( this->size() );
    if ( s > current )
    {
      const SizeType diff ( s - current );
      const ValueType temp = ValueType();
      for ( SizeType i = 0; i < diff; ++i )
      {
        this->append ( temp );
      }
    }
    else if ( s < current )
    {
      _db << "delete from " << Detail::VECTOR_TABLE 
          << " where " << Detail::INDEX_COLUMN << " >= " << s;
      _db->execute();
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  void assign ( const ThisType &v )
  {
    this->_init();
    this->append ( v );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  ThisType &operator = ( const ThisType &v )
  {
    this->assign ( v );
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Append.
  //
  /////////////////////////////////////////////////////////////////////////////

  void append ( const ThisType &v )
  {
    const SizeType size ( v.size() );
    ValueType temp = ValueType();
    for ( SizeType i = 0; i < size; ++i )
    {
      v.get ( i, temp );
      this->append ( temp );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Append.
  //
  /////////////////////////////////////////////////////////////////////////////

  void append ( const ValueType &v )
  {
    _db << "insert into " << Detail::VECTOR_TABLE 
        << " ( " << Detail::INDEX_COLUMN << ", " << Detail::DATA_COLUMN 
        << " ) values ( " << this->size() << ", " << v << " )";
    _db->execute();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the size of the vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  SizeType size() const
  {
    CountCallback::ValidRefPtr cb ( new CountCallback );
    _db->callback ( cb.get() );
    _db << "select count(*) from " << Detail::VECTOR_TABLE;
    _db->execute();
    SizeType count ( cb->count() );
    _db->callback ( 0x0 );
    return count;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Print the vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class OutputStream > void print ( OutputStream &out ) const
  {
    typedef PrintCallback<OutputStream> PrintCallback;
    PrintCallback::ValidRefPtr cb ( new PrintCallback ( out ) );
    _db->callback ( cb.get() );
    _db << "select * from " << Detail::VECTOR_TABLE;
    _db->execute();
    _db->callback ( 0x0 );
  }


private:


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Local database callback for getting values.
  //
  /////////////////////////////////////////////////////////////////////////////

  struct GetCallback : SQLiteWrap::Callback
  {
    USUL_DECLARE_REF_POINTERS ( GetCallback );
    GetCallback() : _value ( ValueType() )
    {
    }
    virtual void operator () ( const SQLiteWrap::Database &db, const std::string &name, const std::string &value )
    {
      std::istringstream in ( value );
      in >> _value;
    }
    void get ( ValueType &v ) const
    {
      v = _value;
    }
  private:
    ValueType _value;
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Local database callback for the row-count.
  //
  /////////////////////////////////////////////////////////////////////////////

  struct CountCallback : SQLiteWrap::Callback
  {
    USUL_DECLARE_REF_POINTERS ( CountCallback );
    CountCallback() : _count ( 0 )
    {
    }
    virtual void operator () ( const SQLiteWrap::Database &db, const std::string &name, const std::string &value )
    {
      std::istringstream in ( value );
      in >> _count;
    }
    const SizeType &count() const
    {
      return _count;
    }
  private:
    SizeType _count;
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Local database callback for printing.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class OutputStream > struct PrintCallback : SQLiteWrap::Callback
  {
    USUL_DECLARE_REF_POINTERS ( PrintCallback );
    PrintCallback ( OutputStream &out ) : _out ( out ), _index ( 0 )
    {
    }
    virtual void operator () ( const SQLiteWrap::Database &db, const std::string &name, const std::string &value )
    {
      if ( Detail::INDEX_COLUMN == name )
      {
        _index = ::atol ( value.c_str() );
      }
      else if ( Detail::DATA_COLUMN == name )
      {
        _out << '[' << _index << "] = " << value << '\n';
      }
    }
    private:
      OutputStream &_out;
      SizeType _index;
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Initialize this instance.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _init()
  {
    _db = new SQLiteWrap::Database;
    std::cout << "Opened database file: " << _db->file() << std::endl;
    _value = new ValueType();
    this->_createTable();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Create the table.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _createTable()
  {
    if ( true == _db.valid() )
    {
      _db << "create table " << Detail::VECTOR_TABLE 
          << " ( " << Detail::INDEX_COLUMN << " integer, " 
          << Detail::DATA_COLUMN << ' ' << TypeTraits::name() << " )";
      _db->execute();
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Drop the table.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _dropTable()
  {
    if ( true == _db.valid() )
    {
      _db << "drop table if exists " << Detail::VECTOR_TABLE;
      _db->execute();
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Clean up this instance.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _clean()
  {
    _db = 0x0;
    delete _value ; _value = 0x0;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Data members.
  //
  /////////////////////////////////////////////////////////////////////////////

  mutable SQLiteWrap::Database::ValidAccessRefPtr _db;
  ValueType *_value;
};


} // namespace SQLiteWrap


#endif // _SQL_LITE_WRAP_VECTOR_H_
