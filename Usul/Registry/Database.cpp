
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Registry database class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Registry/Database.h"
#include "Usul/Registry/Node.h"
#include "Usul/Registry/Visitor.h"
#include "Usul/Trace/Trace.h"

using namespace Usul::Registry;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member(s).
//
///////////////////////////////////////////////////////////////////////////////

Database *Database::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Database::Database() : 
  _mutex ( new Mutex() ),
  _root  ( new Node() )
{
  USUL_TRACE_SCOPE;
  _root->name ( "registry" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Database::~Database()
{
  USUL_TRACE_SCOPE;
  _root = 0x0;
  delete _mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the single instance. Not thread safe allocation!
//
///////////////////////////////////////////////////////////////////////////////

Database &Database::instance()
{
  USUL_TRACE_SCOPE_STATIC;
  if ( 0x0 == _instance )
    _instance = new Database;
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the single instance. Not thread safe!
//
///////////////////////////////////////////////////////////////////////////////

void Database::destroy()
{
  USUL_TRACE_SCOPE_STATIC;
  if ( 0x0 != _instance )
  {
    delete _instance;
    _instance = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the mutex.
//
///////////////////////////////////////////////////////////////////////////////

Database::Mutex &Database::mutex() const
{
  USUL_TRACE_SCOPE;
  return *_mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the database.
//
///////////////////////////////////////////////////////////////////////////////

void Database::clear()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _root->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Database::accept ( Usul::Registry::Visitor *v )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != v )
  {
    v->visit ( this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Operator to return the child with the name. Creates child nodes as needed.
//
///////////////////////////////////////////////////////////////////////////////

Node &Database::operator [] ( const std::string &name )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return (*_root)[name];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the root.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Registry::Node *Database::root()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the root.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Registry::Node *Database::root() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _root.get();
}
