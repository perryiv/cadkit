
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


///////////////////////////////////////////////////////////////////////////////
//
//  Table names.
//
///////////////////////////////////////////////////////////////////////////////

const std::string NODE_TABLE_NAME ( "node_cache" );
const std::string NODE_TAGS_TABLE_NAME ( "tags" );

const std::string LINE_STRING_TABLE_NAME ( "line_string_cache" );
const std::string LINE_STRING_TAGS_TABLE_NAME ( "line_string_tags" );


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
const std::string GEOMETRY_COLUMN ( "geometry" );
const std::string NUM_NODES_COLUMN ( "num_nodes" );
const std::string NODE_IDS_COLUMN ( "node_ids" );


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

const std::string CREATE_LINE_STRING_TABLE 
  ( Usul::Strings::format ( 
    "CREATE TABLE IF NOT EXISTS ", LINE_STRING_TABLE_NAME, " ( id integer primary key autoincrement, ",
    KEY_COLUMN, " text not null, ",
    OBJECT_ID_COLUMN, " integer not null, ",
    DATE_COLUMN, " text not null, ",
    NUM_NODES_COLUMN, " integer not null, ",
    NODE_IDS_COLUMN, " blob not null, ",
    GEOMETRY_COLUMN, " blob not null )" ) );


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
    _connection->execute ( CREATE_LINE_STRING_TABLE );

    _connection->execute ( Usul::Strings::format ( "SELECT CreateSpatialIndex('", NODE_TABLE_NAME, "' , '", LOCACTION_COLUMN, "')" ) );
    _connection->execute ( Usul::Strings::format ( "SELECT CreateSpatialIndex('", LINE_STRING_TABLE_NAME, "' , '", GEOMETRY_COLUMN, "')" ) );

    this->_createTagsTable ( NODE_TAGS_TABLE_NAME );
    this->_createTagsTable ( LINE_STRING_TAGS_TABLE_NAME );
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
  //location[0] -= 180.0;
  //location[1] -= 90.0;

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
//  Create sql for creating a line.
//
///////////////////////////////////////////////////////////////////////////////

std::string Cache::_createLineText ( const LineString::Vertices& vertices )
{
  std::string sql ( "GeomFromText ( 'LINESTRING ( " );

  if ( vertices.size() > 0 )
  {
    for ( LineString::Vertices::const_iterator iter = vertices.begin(); iter != vertices.end() - 1; ++iter )
    {
      sql = Usul::Strings::format ( sql, (*iter)[0], " ", (*iter)[1], ", " );
    }

    sql = Usul::Strings::format ( sql, vertices.back()[0], " ", vertices.back()[1] );
  }
  
  sql = Usul::Strings::format ( sql, " )' )" );
  return sql;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create sql for creating a point.
//
///////////////////////////////////////////////////////////////////////////////

std::string Cache::_createPointText ( const Node::Location& location )
{
  return Usul::Strings::format ( "GeomFromText ( 'POINT ( ", location[0] /*+ 180.0*/, ' ', location[1] /*+ 90.0*/, " )' )" );
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
      extents.minLon() /*+ 180.0*/, ",", 
      extents.minLat() /*+ 90.0*/, ",", 
      extents.maxLon() /*+ 180.0*/, ",", 
      extents.maxLat() /*+ 90.0*/, " )" ) );
  return buildMBR;
}


SQL_LITE_WRAP_DEFINE_VECTOR_BINDER ( LineString::NodeIds );


///////////////////////////////////////////////////////////////////////////////
//
//  Add line data.
//
///////////////////////////////////////////////////////////////////////////////

void Cache::addLineData ( const std::string& key, const Extents& extents, const Lines& lines )
{
  if ( false == _connection.valid() )
    return;

  CadKit::Database::SQLite::Transaction<CadKit::Database::SQLite::Connection::RefPtr> transaction ( _connection );
  for ( Lines::const_iterator iter = lines.begin(); iter != lines.end(); ++iter )
  {
    LineString::RefPtr line ( *iter );
    if ( line.valid() )
    {
      LineString::IdType id ( line->id() );
      LineString::Date timestamp ( line->timestamp() );
      LineString::Tags tags ( line->tags() );
      
      const std::string geometry ( Cache::_createLineText ( line->vertices() ) );

      LineString::NodeIds ids ( line->ids() );
      unsigned int numNodes ( ids.size() );

      const std::string columns ( 
        Usul::Strings::format ( 
          KEY_COLUMN, ", ", 
          OBJECT_ID_COLUMN, ", ", 
          DATE_COLUMN, ", ", 
          NUM_NODES_COLUMN, ", ", 
          NODE_IDS_COLUMN, ", ", 
          GEOMETRY_COLUMN ) );

      const std::string values ( 
        Usul::Strings::format ( 
          "\"", key, "\", ", 
          id, ", ", 
          "\"", timestamp.toString(), "\", ", 
          numNodes, ", ", 
          "?", ", ", 
          "", geometry, "" ) );

      const std::string sql ( Usul::Strings::format ( 
        "INSERT INTO ", LINE_STRING_TABLE_NAME, 
        " ( ", columns, " ) values (",
        values, " )" ) );
      
      USUL_TRY_BLOCK
      {
        _connection->execute ( sql, ids );
      }
      USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "6077392060" );

      this->_addTags ( LINE_STRING_TAGS_TABLE_NAME, id, tags ); 
    } 
  }
  transaction.commit();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line data.
//
///////////////////////////////////////////////////////////////////////////////

void Cache::getLineData ( const std::string& key, const Extents& extents, Lines& lines ) const
{
  if ( false == _connection.valid() )
    return;

  const std::string columns ( 
    Usul::Strings::format ( 
      "asBinary(", GEOMETRY_COLUMN, "), ",
      OBJECT_ID_COLUMN, ", ", 
      DATE_COLUMN, ", ", 
      NUM_NODES_COLUMN, ", ", 
      NODE_IDS_COLUMN, " "  ) );

  const std::string sql ( Usul::Strings::format (
    "SELECT ", columns, "  FROM ", LINE_STRING_TABLE_NAME, 
    " WHERE \"", KEY_COLUMN, "\"=\"", key, "\"", 
    " AND MBROverlaps ( ", Cache::_createMBRText ( extents ), ", ", GEOMETRY_COLUMN, " )" ) );

  USUL_TRY_BLOCK
  {
    CadKit::Database::SQLite::Result::RefPtr result ( _connection->execute ( sql ) );
    if ( false == result.valid() )
      return;
    
    while ( result->prepareNextRow() )
    {
      std::string sId, sTimestamp, sNumNodes;

      typedef CadKit::Database::SQLite::Blob Blob;
      Blob geometry;
      Blob bNodeIds;

      *result >> geometry >> sId >> sTimestamp >> sNumNodes >> bNodeIds;

      LineString::IdType id ( Usul::Convert::Type<std::string,Node::IdType>::convert ( sId ) );
      LineString::Date timestamp ( sTimestamp );
      LineString::Tags tags;
      LineString::Vertices vertices ( Usul::Convert::Type<Blob,LineString::Vertices>::convert ( geometry ) );

      unsigned int numNodes ( Usul::Convert::Type<std::string,unsigned int>::convert ( sNumNodes ) );
      LineString::NodeIds nodeIds;

      if ( bNodeIds.size() == ( numNodes * sizeof ( OSMObject::IdType ) ) )
      {
        OSMObject::IdType *idPtr ( reinterpret_cast<OSMObject::IdType *> ( &bNodeIds[0] ) );
        nodeIds.assign ( idPtr, idPtr + numNodes );
      }
      
      this->_getTags ( LINE_STRING_TAGS_TABLE_NAME, id, tags );

      lines.push_back ( LineString::create ( id, timestamp, tags, vertices, nodeIds ) );
    }
  }
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "1740155461" )
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the line data exist in the cache?
//
///////////////////////////////////////////////////////////////////////////////

bool Cache::hasLineData ( const std::string& key, const Extents& extents ) const
{
  if ( false == _connection.valid() )
    return false;

  const std::string sql ( Usul::Strings::format (
    "SELECT * FROM ", LINE_STRING_TABLE_NAME, 
    " WHERE \"", KEY_COLUMN, "\"=\"", key, "\"", 
    " AND MBROverlaps ( ", Cache::_createMBRText ( extents ), ", ", GEOMETRY_COLUMN, " ) LIMIT 1;" ) );

  USUL_TRY_BLOCK
  {
    CadKit::Database::SQLite::Result::RefPtr result ( _connection->execute ( sql ) );
    return result->prepareNextRow();
  }
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "3950041208" );

  return false;
}
