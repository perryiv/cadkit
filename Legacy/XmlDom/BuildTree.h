
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Callback class that builds a node-tree.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_CALLBACK_BUILD_TREE_H_
#define _XML_CALLBACK_BUILD_TREE_H_

#include "XmlDom/Pointer.h"

#include <list>


namespace XML {
namespace Callback {


template < class NodeType > class BuildTree
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef NodeType Node;
  typedef typename Node::String String;
  typedef XML::Pointer < Node > Pointer;
  typedef std::list < Pointer > Parents;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  explicit BuildTree() : _parents(), _root ( 0x0 )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  BuildTree ( const BuildTree &cb ) : _parents ( cb._parents ), _root ( cb._root )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  BuildTree &operator = ( const BuildTree &cb )
  {
    _parents = cb._parents;
    _root    = cb._root;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called when the start of a node is found.
  //
  /////////////////////////////////////////////////////////////////////////////

  void start ( const String &name, const String &value )
  {
    // Make a new node.
    Pointer node ( new Node ( name, value ) );

    // Assign the root if this is the first time.
    if ( !_root.valid() )
      _root = node;

    // If there is a parent then append this node to it.
    if ( !_parents.empty() )
      _parents.back()->append ( node.get() );

    // Push the new node onto the parent stack.
    _parents.push_back ( node );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called when the end of a node is found.
  //
  /////////////////////////////////////////////////////////////////////////////

  void end ( const String &name )
  {
    // Pop the current child.
    if ( !_parents.empty() )
      _parents.pop_back();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Clear any accumulated state.
  //
  /////////////////////////////////////////////////////////////////////////////

  void clear()
  {
    while ( !_parents.empty() )
      _parents.pop_back();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the root.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Node *root() const
  {
    return _root.get();
  }
  Node *root()
  {
    return _root.get();
  }

protected:

  Parents _parents;
  Pointer _root;
};


}; // namespace Callback
}; // namespace XML


#endif // _XML_CALLBACK_BUILD_TREE_H_
