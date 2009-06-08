
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
//  Map of data-member wrappers.
//
///////////////////////////////////////////////////////////////////////////////

#include "Serialize/XML/DataMemberMap.h"

#include "Usul/Trace/Trace.h"

using namespace Serialize::XML;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

DataMemberMap::DataMemberMap() : _map()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor
//
///////////////////////////////////////////////////////////////////////////////

DataMemberMap::DataMemberMap ( const DataMemberMap &m ) : _map ( m._map )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

DataMemberMap::~DataMemberMap()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Serialize the node.
//
///////////////////////////////////////////////////////////////////////////////

void DataMemberMap::serialize ( XmlTree::Node &parent ) const
{
  USUL_TRACE_SCOPE;

  for ( Map::const_iterator i = _map.begin(); i != _map.end(); ++i )
  {
    if ( true == i->second.valid() )
    {
      i->second->serialize ( parent );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize the node.
//
///////////////////////////////////////////////////////////////////////////////

void DataMemberMap::deserialize ( const XmlTree::Node &node )
{
  USUL_TRACE_SCOPE;

  typedef XmlTree::Node::Children::const_iterator Itr;
  for ( Itr i = node.children().begin(); i != node.children().end(); ++i )
  {
    XmlTree::Node::RefPtr member ( i->get() );
    if ( true == member.valid() )
    {
      Map::const_iterator j ( _map.find ( member->name() ) );
      if ( _map.end() != j )
      {
        Serialize::XML::MemberBase::RefPtr deserializer ( j->second );
        if ( true == deserializer.valid() )
        {
          deserializer->deserialize ( *member );
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a member.
//
///////////////////////////////////////////////////////////////////////////////

void DataMemberMap::addMember ( Serialize::XML::MemberBase *member )
{
  if ( 0x0 != member )
  {
    _map.insert ( Map::value_type ( member->name(), Serialize::XML::MemberBase::RefPtr ( member ) ) );
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Erase the data member with the given name.
//
///////////////////////////////////////////////////////////////////////////////

void DataMemberMap::erase ( const std::string& name )
{
  _map.erase ( name );
}
