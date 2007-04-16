
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SERIALIZE_XML_SMART_POINTER_H__
#define __SERIALIZE_XML_SMART_POINTER_H__

#include "XmlTree/Node.h"


namespace Serialize {
namespace XML {


template < class T, class C >
inline void serialize ( const std::string& name, XmlTree::Node &parent, Usul::Pointers::QueryPointer< T, C >& value )
{
  // Handle null smart-pointer.
  if ( true == value.valid() )
  {
    XmlTree::Node::ValidRefPtr node ( new XmlTree::Node ( name ) );

    Usul::Interfaces::ISerialize::QueryPtr iSerialize ( value );

    if( iSerialize.valid() )
    {
      node->attributes()["TypeName"] = iSerialize->className();
      parent.children().push_back ( node.get() );
      iSerialize->serialize ( *node );
    }
  }
}


template < class T, class C >
inline void deserialize ( const std::string& name, const XmlTree::Node &node, Usul::Pointers::QueryPointer< T, C >& value )
{
  if ( name == node.name() )
  {
    if ( false == value.valid() )
    {
      XmlTree::Node::Attributes::const_iterator j = node.attributes().find ( "TypeName" );
      if ( node.attributes().end() != j )
      {
        value = dynamic_cast < typename T * > ( Factory::instance().create ( j->second ) );
      }
    }

    if ( true == value.valid() )
    {
      Usul::Interfaces::ISerialize::QueryPtr iSerialize ( value );

      if( iSerialize.valid() )
      {
        iSerialize->deserialize ( node );
      }
    }
  }
}


template < class T, class C >
inline void serialize ( const std::string& name, XmlTree::Node &parent, const Usul::Pointers::SmartPointer< T, C >& value )
{
  // Handle null smart-pointer.
  if ( true == value.valid() )
  {
    XmlTree::Node::ValidRefPtr node ( new XmlTree::Node ( name ) );
    node->attributes()["TypeName"] = value->className();
    parent.children().push_back ( node.get() );
    value->serialize ( *node );
  }
}

template < class T, class C >
inline void deserialize ( const std::string& name, const XmlTree::Node &node, Usul::Pointers::SmartPointer< T, C >& value )
{
  typedef Usul::Pointers::SmartPointer< T, C > PointerType;
  if ( name == node.name() )
  {
    if ( false == value.valid() )
    {
      XmlTree::Node::Attributes::const_iterator j = node.attributes().find ( "TypeName" );
      if ( node.attributes().end() != j )
      {
        value = dynamic_cast < typename PointerType::element_type * > ( Factory::instance().create ( j->second ) );
      }
    }

    if ( true == value.valid() )
    {
      value->deserialize ( node );
    }
  }
}

}
}

#endif // __SERIALIZE_XML_SMART_POINTER_H__
