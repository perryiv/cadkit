
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_TREE_PREDICATES_H_
#define _XML_TREE_PREDICATES_H_

#include "XmlTree/Node.h"

#include <string>


namespace XmlTree {


///////////////////////////////////////////////////////////////////////////////
//
//  Returns true if the given node has the attribute.
//
///////////////////////////////////////////////////////////////////////////////

struct HasAttribute
{
  typedef XmlTree::Node::Children::value_type NodePtr;
  typedef XmlTree::Node::Attributes Attributes;
  HasAttribute ( const std::string &name, const std::string &value ) : _name ( name ), _value ( value )
  {
  }
  bool operator () ( const NodePtr &node ) const
  {
    if ( false == node.valid() )
    {
      return false;
    }

    const Attributes &attr ( node->attributes() );
    Attributes::const_iterator i ( attr.find ( _name ) );
    if ( attr.end() == i )
    {
      return false;
    }
    else
    {
      const bool result ( i->second == _value );
      return result;
    }
  }
private:
  std::string _name;
  std::string _value;
};


} // namespace XmlTree


#endif // _XML_TREE_PREDICATES_H_
