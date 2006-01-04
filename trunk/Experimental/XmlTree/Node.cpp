
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

#include "Usul/Errors/Assert.h"
#include "Usul/Math/MinMax.h"

#include <algorithm>
#include <functional>

using namespace XmlTree;


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
//  Predicate class to check for same name.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct SameName
  {
    SameName ( const std::string &n ) : _n ( n ){}
    bool operator () ( const Node::ValidRefPtr &node )
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
      return ( _t1 ( node ) && _t2 ( node ) );
    }
  private:
    T1 _t1;
    T2 _t2;
  };
  template < class T1, class T2 > And < T1, T2 > and ( const T1 &t1, const T2 &t2 )
  {
    return ( And < T1, T2 > ( t1, t2 ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the i'th child with the given name. Create it if needed.
//
///////////////////////////////////////////////////////////////////////////////

XmlTree::Node *Node::child ( unsigned int which, const std::string &name )
{
  USUL_ASSERT ( false == name.empty() );

  // See if we can quickly find the child.
  {
    Children::iterator c ( std::find_if ( _children.begin(), _children.end(), Helper::and ( Helper::SameName ( name ), Helper::SameIndex ( which ) ) ) );
    if ( _children.end() != c )
      return c->get();
  }

  // If we get to here then we have to add children.
  {
    const unsigned int existing ( std::count_if ( _children.begin(), _children.end(), Helper::SameName ( name ) ) );
    USUL_ASSERT ( which >= existing );
    _children.reserve ( _children.size() + ( which + 1 - existing ) );
    for ( unsigned int i = _children.size(); i < _children.capacity(); ++i )
      _children.push_back ( new Node ( name ) );
  }

  // Look again.
  {
    Children::iterator c ( std::find_if ( _children.begin(), _children.end(), Helper::and ( Helper::SameName ( name ), Helper::SameIndex ( which ) ) ) );
    if ( _children.end() != c )
      return c->get();
  }

  // If we get to here then something went wrong.
  throw std::runtime_error ( "Error 1232649218: Failed to add new child node" );
}
