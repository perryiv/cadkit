
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Generic visitor class and helper function.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_GENERIC_VISITOR_H_
#define _OSG_TOOLS_GENERIC_VISITOR_H_

#include "osg/NodeVisitor"
#include "osg/ref_ptr"


namespace OsgTools {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic visitor class.
//
///////////////////////////////////////////////////////////////////////////////

template 
<
  class NodeType_, 
  class Operation_
>
struct Visitor : public osg::NodeVisitor
{
  typedef osg::NodeVisitor BaseClass;
  typedef Operation_ operation_type;
  typedef NodeType_ node_type;
  typedef Visitor<node_type,operation_type> this_type;
  typedef osg::ref_ptr<this_type> Ptr;

  Visitor ( const operation_type &op, TraversalMode mode = BaseClass::TRAVERSE_ALL_CHILDREN ) : 
    BaseClass ( mode ), 
    _op ( op )
  {
    // Always hit every node.
    const unsigned int all ( 0xffffffff );
    this->setTraversalMask    ( all );
    this->setNodeMaskOverride ( all );
  }

  virtual ~Visitor()
  {
  }

  virtual void apply ( node_type &node )
  {
    _op ( &node );
    BaseClass::apply ( node );
  }

  const operation_type &  op() const { return _op; }
  operation_type &        op()       { return _op; }

protected:

  Visitor ( const Visitor &v );             // Disable copy.
  Visitor &operator = ( const Visitor &v ); // Disable copy.

private:

  operation_type _op;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class to make the visitor.
//
///////////////////////////////////////////////////////////////////////////////

template < class NodeType_ > struct MakeVisitor
{
  template < class Operation_ >
  static Visitor<NodeType_,Operation_> *make ( const Operation_ &op )
  {
    return new Visitor<NodeType_,Operation_> ( op );
  }
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_GENERIC_VISITOR_H_
