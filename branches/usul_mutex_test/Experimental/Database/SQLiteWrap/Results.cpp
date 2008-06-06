
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
//  Results callback for SQLite database.
//
///////////////////////////////////////////////////////////////////////////////

#include "SQLiteWrap/Results.h"
#include "SQLiteWrap/Database.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"

#include <string>
#include <stdexcept>

using namespace SQLiteWrap;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Results::Results () : BaseClass(),
  _columns()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Results::~Results()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Results::_clean ), "4030095801" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean this instance. Should only be called from the destructor.
//
///////////////////////////////////////////////////////////////////////////////

void Results::_clean()
{
  _columns.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Callback function.
//
///////////////////////////////////////////////////////////////////////////////

void Results::operator () ( const Database &db, const std::string &name, const std::string &value )
{
  Guard guard ( this->mutex() );
  _columns[name].push_back ( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called right before a query.
//
///////////////////////////////////////////////////////////////////////////////

void Results::start ( const Database & )
{
  this->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear internal memory.
//
///////////////////////////////////////////////////////////////////////////////

void Results::clear()
{
  Guard guard ( this->mutex() );
  _columns.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the column.
//
///////////////////////////////////////////////////////////////////////////////

const Results::Column &Results::column ( const std::string &name ) const
{
  Guard guard ( this->mutex() );
  Columns::const_iterator i ( _columns.find ( name ) );
  if ( _columns.end() != i )
    return i->second;
  throw std::runtime_error ( std::string ( "Error 1782949017: Unknown column '" ) + name + std::string ( "'" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the column.
//
///////////////////////////////////////////////////////////////////////////////

void Results::column ( const std::string &name, Column &column ) const
{
  Guard guard ( this->mutex() );
  column = this->column ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the results.
//
///////////////////////////////////////////////////////////////////////////////

void Results::print ( std::ostream &out ) const
{
  Guard guard ( this->mutex() );

  // Handle empty state.
  if ( true == _columns.empty() )
    return;

  // Hold sorted column indices.
  typedef std::vector<unsigned int> Indices;
  Indices indices;
  indices.reserve ( _columns.size() );

  // Loop through columns...
  {
    for ( Columns::const_iterator i = _columns.begin(); i != _columns.end(); ++i )
    {
      // Print column name.
      out << i->first << ' ';
    }
    out << '\n';
  }

  // Loop through the rows.
  {
    const unsigned int numRows ( _columns.begin()->second.size() );
    for ( unsigned int i = 0; i < numRows; ++i )
    {
      // Loop through columns...
      {
        for ( Columns::const_iterator j = _columns.begin(); j != _columns.end(); ++j )
        {
          // Print value.
          const std::string &value ( j->second.at(i) );
          out << ( ( true == value.empty() ) ? "null" : value ) << ' ';
        }
        out << '\n';
      }
    }
  }
}
