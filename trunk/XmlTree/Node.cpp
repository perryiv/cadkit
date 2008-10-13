
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  XML node class.
//
///////////////////////////////////////////////////////////////////////////////

#include "XmlTree/Node.h"

#include "Usul/Math/MinMax.h"
#include "Usul/Strings/Format.h"
#include "Usul/Strings/Split.h"

#include <algorithm>
#include <functional>
#include <list>

using namespace XmlTree;

USUL_IMPLEMENT_TYPE_ID ( Node );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Node::Node ( const std::string &name, const std::string &value ) : BaseClass(),
  _name       ( name  ),
  _value      ( value ),
  _attributes (),
  _children   ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Node::~Node()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void Node::clear()
{
  _value.clear();
  _children.clear();
  _attributes.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set name.
//
///////////////////////////////////////////////////////////////////////////////

void Node::name ( const std::string &n )
{
  _name = n;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set value.
//
///////////////////////////////////////////////////////////////////////////////

void Node::value ( const std::string &v )
{
  _value = v;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return child at path. If requested, creates them as needed.
//
///////////////////////////////////////////////////////////////////////////////

Node *Node::child ( unsigned int which, const std::string &path, const char delim, bool createIfNeeded )
{
  // Empty path is an error.
  if ( true == path.empty() )
    throw std::runtime_error ( "Error 2832818132: Empty node-path given" );

  // Split the path.
  typedef std::list < std::string > Parts;
  Parts parts;
  Usul::Strings::split ( path, delim, false, parts );

  // Should be true.
  if ( true == parts.empty() )
    throw std::runtime_error ( "Error 2941769360: Failed to split node-path: " + path );

  // We special-case the last one.
  const std::string last ( parts.back() );
  parts.pop_back();

  // Loop through the parts and get the node. At this point we always ask for 
  // the first child with the given name.
  XmlTree::Node::RefPtr node ( this );
  for ( Parts::const_iterator i = parts.begin(); i != parts.end(); ++i )
  {
    node = node->_child ( 0, *i, createIfNeeded );
    if ( false == node.valid() )
      return 0x0;
  }

  // The last one we make the i'th child.
  node = node->_child ( which, last, createIfNeeded );

  // Return what we have, which may be null.
  return node.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate class to check for same name.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct SameName
  {
    SameName ( const std::string &n ) : _n ( n ){}
    bool operator () ( const Node::ValidRefPtr &node ) const
    {
      return ( _n == node->name() );
    }
  private:
    std::string _n;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate class to check for index.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct SameIndex
  {
    SameIndex ( unsigned int i ) : _index ( i ), _count ( 0 ){}
    bool operator () ( const Node::ValidRefPtr & )
    {
      return ( _index == _count++ ); // Do not use prefix.
    }
  private:
    unsigned int _index;
    unsigned int _count;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Composite predicate class.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T1, class T2 > struct And
  {
    And ( const T1 &t1, const T2 &t2 ) : _t1 ( t1 ), _t2 ( t2 ){}
    bool operator () ( const Node::ValidRefPtr &node )
    {
      bool b1 ( _t1 ( node ) );
      bool b2 ( _t2 ( node ) );
      return b1 && b2;
    }
  private:
    T1 _t1;
    T2 _t2;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the i'th child with the given name. Create it if needed.
//
///////////////////////////////////////////////////////////////////////////////

XmlTree::Node *Node::_child ( unsigned int which, const std::string &name, bool createIfNeeded )
{
  if ( true == name.empty() )
    throw std::runtime_error ( "Error 1803369517: Empty node name given" );

  // See if we can quickly find the child.
  {
    Helper::SameName sn ( name );
    Helper::SameIndex si ( which );
    Helper::And < Helper::SameName, Helper::SameIndex > predicate ( sn, si );
    Children::iterator c ( std::find_if ( _children.begin(), _children.end(), predicate ) );
    if ( _children.end() != c )
      return c->get();
  }

  // If we are not supposed to create then punt.
  if ( false == createIfNeeded )
    return 0x0;

  // If we get to here then we have to add children.
  {
    const unsigned int existing ( std::count_if ( _children.begin(), _children.end(), Helper::SameName ( name ) ) );
    if ( which < existing )
      throw std::runtime_error ( Usul::Strings::format ( "Error 4256284815: Failed to add new child, which = ", which, ", existing = ", existing ) );
    _children.reserve ( _children.size() + ( which + 1 - existing ) );
    for ( unsigned int i = _children.size(); i < _children.capacity(); ++i )
      _children.push_back ( new Node ( name ) );
  }

  // Look again.
  {
    Helper::SameName sn ( name );
    Helper::SameIndex si ( which );
    Helper::And < Helper::SameName, Helper::SameIndex > predicate ( sn, si );
    Children::iterator c ( std::find_if ( _children.begin(), _children.end(), predicate ) );
    if ( _children.end() != c )
      return c->get();
  }

  // If we get to here then something went wrong.
  throw std::runtime_error ( "Error 1232649218: Failed to add new child node" );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Helper predicate to check name of node.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct NameCompare
  {
    NameCompare ( const std::string& name ) : _name ( name )
    {
    }
    
    bool operator() ( const std::string& name ) const
    {
      return name == _name;
    }
    
  private:
    std::string _name;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all children with the given name. Pass true for "traverse" if you 
//  want to search all children, false if just immediate children.
//
///////////////////////////////////////////////////////////////////////////////

void Node::find ( const std::string &name, bool traverse, Children &all ) const
{
  this->findIf ( traverse, all, Helper::NameCompare ( name ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all children with the given name. Pass true for "traverse" if you 
//  want to search all children, false if just immediate children.
//
///////////////////////////////////////////////////////////////////////////////

Node::Children Node::find ( const std::string &name, bool traverse ) const
{
  Children children;
  this->find ( name, traverse, children );
  return children;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the attribute. Returns empty string if it's not there.
//
///////////////////////////////////////////////////////////////////////////////

std::string Node::attribute ( const std::string &name ) const
{
  Attributes::const_iterator i ( _attributes.find ( name ) );
  return ( ( _attributes.end() == i ) ? std::string() : i->second );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the node.
//
///////////////////////////////////////////////////////////////////////////////

void Node::append ( Node *node )
{
  if ( 0x0 != node )
  {
    _children.push_back ( Node::ValidRefPtr ( node ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the node.
//
///////////////////////////////////////////////////////////////////////////////

Node::RefPtr Node::append ( const std::string &name, const std::string &value )
{
  Node::RefPtr node ( new Node ( name, value ) );
  this->append ( node.get() );
  return node;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the first child by the name.
//
///////////////////////////////////////////////////////////////////////////////

Node::RefPtr Node::child ( const std::string &name, bool createIfNeeded )
{
  Node::RefPtr node ( this->child ( name ) );
  if ( false == node.valid() && true == createIfNeeded )
  {
    node = this->append ( name );
  }
  return node;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the first child by the name.
//
///////////////////////////////////////////////////////////////////////////////

Node::RefPtr Node::child ( const std::string &name ) const
{
  Children kids ( this->find ( name, false ) );
  if ( false == kids.empty() )
  {
    return kids.front().get();
  }

  return 0x0;
}
