
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/Cache.h"

#include "Database/SQLite/Transaction.h"
#include "Tests/Database/SQLite/Common/Functions.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Convert/WellKnownBinary.h"
#include "Usul/Functions/SafeCall.h"

using namespace Minerva::Layers::OSM;

#define USE_WAY_EXTENTS 0

///////////////////////////////////////////////////////////////////////////////
//
//  Table names.
//
///////////////////////////////////////////////////////////////////////////////

const std::string NODE_TABLE_NAME ( "node_cache" );
const std::string NODE_TAGS_TABLE_NAME ( "tags" );

const std::string WAY_TABLE_NAME ( "way_cache" );
const std::string WAY_TAGS_TABLE_NAME ( "way_tags" );
const std::string WAY_NODES_TABLE_NAME ( "way_nodes" );


///////////////////////////////////////////////////////////////////////////////
//
//  Column names.
//
///////////////////////////////////////////////////////////////////////////////

const std::string KEY_COLUMN ( "key" );
const std::string VALUE_COLUMN ( "value" );
const std::string LOCACTION_COLUMN ( "location" );
const std::string OBJECT_ID_COLUMN ( "object_id" );
const std::string DATE_COLUMN ( "timestamp" );
const std::string TAGS_COLUMN ( "tags" );
const std::string NODE_ID_COLUMN ( "node_id" );
const std::string EXTENTS_COLUMN ( "extents" );


///////////////////////////////////////////////////////////////////////////////
//
//  SQL statements to create tables.
//
///////////////////////////////////////////////////////////////////////////////

const std::string CREATE_NODE_TABLE 
  ( Usul::Strings::format ( 
    "CREATE TABLE IF NOT EXISTS ", NODE_TABLE_NAME, " ( id integer primary key autoincrement, ",
    KEY_COLUMN, " text not null, ",
    LOCACTION_COLUMN, " blob not null, ",
    OBJECT_ID_COLUMN, " integer not null, ",
    DATE_COLUMN, " text not null )" ) );


#if USE_WAY_EXTENTS == 1
const std::string CREATE_WAY_TABLE 
  ( Usul::Strings::format ( 
    "CREATE TABLE IF NOT EXISTS ", WAY_TABLE_NAME, " ( id integer primary key autoincrement, ",
    KEY_COLUMN, " text not null, ",
    OBJECT_ID_COLUMN, " integer not null, ",
    DATE_COLUMN, " text not null, ",
    EXTENTS_COLUMN, " blob not null )" ) );
#else
const std::string CREATE_WAY_TABLE 
  ( Usul::Strings::format ( 
    "CREATE TABLE IF NOT EXISTS ", WAY_TABLE_NAME, " ( id integer primary key autoincrement, ",
    KEY_COLUMN, " text not null, ",
    OBJECT_ID_COLUMN, " integer not null, ",
    DATE_COLUMN, " text not null )" ) );
#endif

const std::string CREATE_WAY_NODES_TABLE 
  ( Usul::Strings::format ( 
    "CREATE TABLE IF NOT EXISTS ", WAY_NODES_TABLE_NAME, " ( id integer primary key autoincrement, ",
    OBJECT_ID_COLUMN, " integer not null, ",
    NODE_ID_COLUMN, " text not null )" ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Cache::Cache ( CadKit::Database::SQLite::Connection *connection ) : BaseClass(),
  _connection ( connection )
{
  // Make the tables.
  if ( _connection.valid() )
  {
    _connection->execute ( CREATE_NODE_TABLE );
    _connection->execute ( CREATE_WAY_TABLE );
    _connection->execute ( CREATE_WAY_NODES_TABLE );

    this->_createTagsTable ( NODE_TAGS_TABLE_NAME );
    this->_createTagsTable ( WAY_TAGS_TABLE_NAME );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Cache::~Cache()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the node data.
//
///////////////////////////////////////////////////////////////////////////////

void Cache::addNodeData ( const std::string& key, const Extents& extents, const Nodes& nodes )
{
  if ( false == _connection.valid() )
    return;

  CadKit::Database::SQLite::Transaction<CadKit::Database::SQLite::Connection::RefPtr> transaction ( _connection );
  this->_addNodeData ( key, extents, nodes );
  transaction.commit();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the node data.
//
///////////////////////////////////////////////////////////////////////////////

void Cache::_addNodeData ( const std::string& key, const Extents& extents, const Nodes& nodes )
{
  for ( Nodes::const_iterator iter = nodes.begin(); iter != nodes.end(); ++iter )
  {
    OSMNodePtr node ( *iter );
    if ( node.valid() )
    {
      Node::IdType id ( node->id() );
      Node::Date timestamp ( node->timestamp() );
      Node::Tags tags ( node->tags() );
      Node::Location location ( node->location() );

      const std::string sql ( Usul::Strings::format ( 
        "INSERT INTO ", NODE_TABLE_NAME, 
        " ( ", KEY_COLUMN, ", ", LOCACTION_COLUMN, ", ", OBJECT_ID_COLUMN, ", ", DATE_COLUMN, " ) values (",
        " \"", key, "\", ",
        " ", Cache::_createPointText ( location ), ",", 
        " \"", id, "\", \"", timestamp.toString(), "\" )" ) );
      
      USUL_TRY_BLOCK
      {
        _connection->execute ( sql );
      }
      USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "2633836923" );

      this->_addTags ( NODE_TAGS_TABLE_NAME, id, tags );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the node data.
//
///////////////////////////////////////////////////////////////////////////////

void Cache::getNodeData ( const std::string& key, const Extents& extents, Nodes& nodes ) const
{
  if ( false == _connection.valid() )
    return;

  const std::string sql ( Usul::Strings::format (
    "SELECT asBinary(", LOCACTION_COLUMN, "), ", OBJECT_ID_COLUMN, ",", DATE_COLUMN, "  FROM ", NODE_TABLE_NAME, 
    " WHERE \"", KEY_COLUMN, "\"=\"", key, "\"", 
    " AND MBRContains ( ", Cache::_createMBRText ( extents ), ", ", LOCACTION_COLUMN, " )" ) );

  USUL_TRY_BLOCK
  {
    CadKit::Database::SQLite::Result::RefPtr result ( _connection->execute ( sql ) );
    if ( false == result.valid() )
      return;
    
    while ( result->prepareNextRow() )
    {
      nodes.push_back ( Cache::_createNode ( *result ) );
    }
  }
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "2175538793" )
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a node from the result.
//
///////////////////////////////////////////////////////////////////////////////

Node* Cache::_createNode ( CadKit::Database::SQLite::Result& result ) const
{
  std::string sId, sTimestamp;

  typedef CadKit::Database::SQLite::Blob Blob;
  Blob geometry;

  result >> geometry >> sId >> sTimestamp;

  Node::IdType id ( Usul::Convert::Type<std::string,Node::IdType>::convert ( sId ) );
  Node::Date timestamp ( sTimestamp );
  Node::Tags tags;
  Node::Location location ( Usul::Convert::Type<Blob,Node::Location>::convert ( geometry ) );
  location[0] -= 180.0;
  location[1] -= 90.0;

  this->_getTags ( NODE_TAGS_TABLE_NAME, id, tags );

  return Node::create ( id, location, timestamp, tags );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the node data exist in the cache?
//
///////////////////////////////////////////////////////////////////////////////

bool Cache::hasNodeData ( const std::string& key, const Extents& extents ) const
{
  if ( false == _connection.valid() )
    return false;

  const std::string sql ( Usul::Strings::format (
    "SELECT * FROM ", NODE_TABLE_NAME, 
    " WHERE \"", KEY_COLUMN, "\"=\"", key, "\"", 
    " AND MBRContains ( ", Cache::_createMBRText ( extents ), ", ", LOCACTION_COLUMN, " ) LIMIT 1;" ) );

  try
  {
    CadKit::Database::SQLite::Result::RefPtr result ( _connection->execute ( sql ) );
    return result->prepareNextRow();
  }
  catch ( ... )
  {
    // Eat all exceptions.  If an error happened, then we don't have the data in the cache.
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the table to store the tags.
//
///////////////////////////////////////////////////////////////////////////////

void Cache::_createTagsTable ( const std::string& tableName )
{
  if ( false == _connection.valid() )
    return;

  const std::string sql
    ( Usul::Strings::format ( 
      "CREATE TABLE IF NOT EXISTS ", tableName, " ( ",
      OBJECT_ID_COLUMN, " integer, ",
      KEY_COLUMN, " text, ",
      VALUE_COLUMN, " text )" ) );

  // Make the table.
  _connection->execute ( sql );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the tags for the id.
//
///////////////////////////////////////////////////////////////////////////////

void Cache::_addTags ( const std::string& tableName, OSMObject::IdType id, const OSMObject::Tags& tags )
{
  if ( false == _connection.valid() )
    return;

  // NO transaction here!  Nested transactions cause a crash.

  for ( Node::Tags::const_iterator iter = tags.begin(); iter != tags.end(); ++iter )
  {
    const std::string key ( iter->first );
    const std::string value ( iter->second );
    const std::string sql ( Usul::Strings::format ( 
      "INSERT INTO ", tableName, 
      " ( ", OBJECT_ID_COLUMN, ", ", KEY_COLUMN, ", ", VALUE_COLUMN, " ) values (",
      " \"", id, "\", ",
      " \"", key, "\",",
      " \"", value, "\" )" ) );
      
    USUL_TRY_BLOCK
    {
      _connection->execute ( sql );
    }
    USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "3992404744" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tags for the id.
//
///////////////////////////////////////////////////////////////////////////////

void Cache::_getTags ( const std::string& tableName, OSMObject::IdType id, OSMObject::Tags& tags ) const
{
  if ( false == _connection.valid() )
    return;

  const std::string sql ( 
    Usul::Strings::format ( 
    "SELECT ", KEY_COLUMN, ", ", VALUE_COLUMN, 
    " FROM ", tableName, 
    " WHERE \"", OBJECT_ID_COLUMN, "\"=\"", id, "\"" ) );

  USUL_TRY_BLOCK
  {
    CadKit::Database::SQLite::Result::RefPtr result ( _connection->execute ( sql ) );
    if ( false == result.valid() )
      return;
    
    while ( result->prepareNextRow() )
    {
      std::string key, value;

      *result >> key >> value;
      tags.insert ( std::make_pair ( key, value ) );
    }
  }
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "2033756500" )
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cache the way data.
//
///////////////////////////////////////////////////////////////////////////////

void Cache::addWayData ( const std::string& key, const Extents& extents, const Ways& ways )
{
  if ( false == _connection.valid() )
    return;

  CadKit::Database::SQLite::Transaction<CadKit::Database::SQLite::Connection::RefPtr> transaction ( _connection );
  for ( Ways::const_iterator iter = ways.begin(); iter != ways.end(); ++iter )
  {
    OSMWayPtr way ( *iter );
    if ( way.valid() )
    {
      USUL_TRY_BLOCK
      {
        this->_addWay ( key, extents, *way );
      }
      USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "2450737445" );
    }
  }
  transaction.commit();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a single way.
//
///////////////////////////////////////////////////////////////////////////////

void Cache::_addWay ( const std::string& key, const Extents& extents, const Way& way )
{
  Way::IdType id ( way.id() );
  Way::Date timestamp ( way.timestamp() );
  Way::Tags tags ( way.tags() );
  Nodes nodes ( way.nodes() );

#if USE_WAY_EXTENTS == 1
  const std::string sql ( Usul::Strings::format ( 
    "INSERT INTO ", WAY_TABLE_NAME, 
    " ( ", KEY_COLUMN, ", ", OBJECT_ID_COLUMN, ", ", DATE_COLUMN, ", ", EXTENTS_COLUMN, " ) values (",
    " \"", key, "\", ",
    " \"", id, "\", ",
    " \"", timestamp.toString(), "\", "
    " ", Cache::_createMBRText ( way.extents() ), " )" ) );
#else
  const std::string sql ( Usul::Strings::format ( 
    "INSERT INTO ", WAY_TABLE_NAME, 
    " ( ", KEY_COLUMN, ", ", OBJECT_ID_COLUMN, ", ", DATE_COLUMN, " ) values (",
    " \"", key, "\", ",
    " \"", id, "\", ",
    " \"", timestamp.toString(), "\" )" ) );
#endif

  _connection->execute ( sql );

  this->_addNodeData ( key, extents, nodes );

  for ( Nodes::const_iterator iter = nodes.begin(); iter != nodes.end(); ++iter )
  {
    OSMNodePtr node ( *iter );
    if ( node.valid() )
    {
      const std::string sql2 ( Usul::Strings::format ( 
        "INSERT INTO ", WAY_NODES_TABLE_NAME, 
        " ( ", OBJECT_ID_COLUMN, ", ", NODE_ID_COLUMN, " ) values (",
        " \"", id, "\", ",
        " \"", node->id(), "\" )" ) );

      _connection->execute ( sql2 );
    }
  }

  this->_addTags ( WAY_TAGS_TABLE_NAME, id, tags );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the way data.
//
///////////////////////////////////////////////////////////////////////////////

void Cache::getWayData ( const std::string& key, const Extents& extents, Ways& ways ) const
{
  if ( false == _connection.valid() )
    return;

#if USE_WAY_EXTENTS == 0
  const std::string innerSql ( Usul::Strings::format (
    "SELECT ", OBJECT_ID_COLUMN, " FROM ", NODE_TABLE_NAME,
    " WHERE \"", KEY_COLUMN, "\"=\"", key, "\"",
    " AND MBRContains ( ", Cache::_createMBRText ( extents ), ", ", LOCACTION_COLUMN, " )" ) );

  const std::string sql ( Usul::Strings::format (
    "SELECT DISTINCT ", OBJECT_ID_COLUMN, " FROM ", WAY_NODES_TABLE_NAME,
    " WHERE \"", NODE_ID_COLUMN, "\" IN (", innerSql, " );" ) );
#else

  const std::string sql ( Usul::Strings::format (
    "SELECT ", OBJECT_ID_COLUMN, " FROM ", WAY_TABLE_NAME,
    " WHERE \"", KEY_COLUMN, "\"=\"", key, "\"",
    " AND MBRContains ( ", Cache::_createMBRText ( extents ), ", ", EXTENTS_COLUMN, " )" ) );

#endif

  USUL_TRY_BLOCK
  {
    CadKit::Database::SQLite::Result::RefPtr result ( _connection->execute ( sql ) );
    while ( result->prepareNextRow() )
    {
      std::string sId;
      *result >> sId;

      Way::IdType id ( Usul::Convert::Type<std::string,Way::IdType>::convert ( sId ) );

      Way::RefPtr way ( this->_createWay ( id ) );
      if ( way.valid() )
      {
        ways.push_back ( way );
      }
    }
  }
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "2699100931" )
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a way.
//
///////////////////////////////////////////////////////////////////////////////

Way* Cache::_createWay ( Way::IdType id ) const
{
  Way::Date timestamp;
  Way::Tags tags;
  Nodes nodes;

  {
    const std::string sql ( Usul::Strings::format ( 
      "SELECT ", DATE_COLUMN,  " FROM ", WAY_TABLE_NAME, 
      " WHERE ", OBJECT_ID_COLUMN, "=", id ) );

    CadKit::Database::SQLite::Result::RefPtr result ( _connection->execute ( sql ) );
    if ( result->prepareNextRow() )
    {
      std::string date;
      *result >> date;

      timestamp = Way::Date ( date );
    }
  }

  this->_getTags ( WAY_TAGS_TABLE_NAME, id, tags );

  {
    const std::string innerSql ( Usul::Strings::format (
      "SELECT ", NODE_ID_COLUMN, " FROM ", WAY_NODES_TABLE_NAME,
      " WHERE \"", OBJECT_ID_COLUMN, "\"=\"", id, "\" " ) );

    const std::string sql ( Usul::Strings::format (
      "SELECT asBinary(", LOCACTION_COLUMN, "), ", OBJECT_ID_COLUMN, ",", DATE_COLUMN, "  FROM ", NODE_TABLE_NAME, 
      " WHERE \"", OBJECT_ID_COLUMN , "\" IN (", innerSql, ") " ) );

    CadKit::Database::SQLite::Result::RefPtr result ( _connection->execute ( sql ) );
    while ( result->prepareNextRow() )
    {
      nodes.push_back ( Cache::_createNode ( *result ) );
    }
  }

  return Way::create ( id, timestamp, tags, nodes );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the cache contain way data?
//
///////////////////////////////////////////////////////////////////////////////

bool Cache::hasWayData ( const std::string& key, const Extents& extents ) const
{
  if ( false == _connection.valid() )
    return false;

#if USE_WAY_EXTENTS == 0
  const std::string innerSql ( Usul::Strings::format (
    "SELECT ", OBJECT_ID_COLUMN, " FROM ", NODE_TABLE_NAME,
    " WHERE \"", KEY_COLUMN, "\"=\"", key, "\"",
    " AND MBRContains ( ", Cache::_createMBRText ( extents ), ", ", LOCACTION_COLUMN, " )" ) );

  const std::string sql ( Usul::Strings::format (
    "SELECT * FROM ", WAY_NODES_TABLE_NAME,
    " WHERE \"", NODE_ID_COLUMN, "\" IN (", innerSql, " ) LIMIT 1;" ) );
#else

  const std::string sql ( Usul::Strings::format (
    "SELECT ", OBJECT_ID_COLUMN, " FROM ", WAY_TABLE_NAME,
    " WHERE \"", KEY_COLUMN, "\"=\"", key, "\"",
    " AND MBRContains ( ", Cache::_createMBRText ( extents ), ", ", EXTENTS_COLUMN, " ) LIMIT 1" ) );

#endif
  try
  {
    CadKit::Database::SQLite::Result::RefPtr result ( _connection->execute ( sql ) );
    return result->prepareNextRow();
  }
  catch ( ... )
  {
    // Eat all exceptions.  If an error happened, then we don't have the data in the cache.
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create sql for creating a point.
//
///////////////////////////////////////////////////////////////////////////////

std::string Cache::_createPointText ( const Node::Location& location )
{
  return Usul::Strings::format ( "GeomFromText ( 'POINT ( ", location[0] + 180.0, ' ', location[1] + 90.0, " )' )" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create sql for creating a MBR.
//
///////////////////////////////////////////////////////////////////////////////

std::string Cache::_createMBRText ( const Extents& extents )
{
  const std::string buildMBR ( 
    Usul::Strings::format ( "BuildMBR ( ", 
      extents.minLon() + 180.0, ",", 
      extents.minLat() + 90.0, ",", 
      extents.maxLon() + 180.0, ",", 
      extents.maxLat() + 90.0, " )" ) );
  return buildMBR;
}
