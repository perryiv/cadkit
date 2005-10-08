
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  XML-tree visitor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_DOM_VISITOR_CLASS_H_
#define _XML_DOM_VISITOR_CLASS_H_

#include <map>


namespace XML {


template
<
  class PolicyType, 
  class NodeType, 
  class CallbackType
>
class Visitor
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef PolicyType                    Policy;
  typedef NodeType                      Node;
  typedef CallbackType                  NodeCB;
  typedef typename Policy::String       String;
  typedef typename Policy::ErrorPolicy  ErrorPolicy;
  typedef typename Policy::FilterPolicy FilterPolicy;
  typedef std::map < String, NodeCB >   Callbacks;
  typedef typename Node::Children       Children;
  typedef typename Node::ChildItr       ChildItr;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Visitor() : 
    _startCBs   (),
    _endCBs     (),
    _hierarchy  (),
    _errorPolicy()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Visitor ( const Visitor &r ) : 
    _startCBs    ( r._startCBs ),
    _endCBs      ( r._endCBs ),
    _hierarchy   ( r._hierarchy ),
    _errorPolicy ( r._errorPolicy )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Visit the node.
  //
  /////////////////////////////////////////////////////////////////////////////

  void operator () ( Node &node )
  {
    // Get the name of this node.
    String name ( node.name() );
    _errorPolicy ( 1379014409u, false == name.empty() );

    // Add the name of this node to current hierarchy.
    String ch ( _hierarchy );
    if ( !_hierarchy.empty() )
      _hierarchy += '/';
    _hierarchy += name;

    // Call any start-callbacks if we are supposed to.
    this->_notify ( _startCBs, node );

    // Call this function for all the children.
    for ( ChildItr i = node.begin(); i != node.end(); ++i )
    {
      Node *node = *i;
      _errorPolicy ( 3944967299u, 0x0 != node );
      (*this) ( *node );
    }

    // Call any finish-callbacks if we are supposed to.
    this->_notify ( _endCBs, node );

    // Restore the hierarchy.
    _hierarchy = ch;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Read the xml file and call any appropriate callbacks.
  //
  /////////////////////////////////////////////////////////////////////////////
#if 0
  void read ( std::istream &in )
  {
    // Check input.
    _errorPolicy ( 1071099976, !!in );

    // Initialize the data members.
    this->_init();

    // Declare a new document.
    cppdom::ContextPtr context ( new cppdom::Context );
    _errorPolicy ( 1071099977, 0x0 != context.get() );
    cppdom::Document document ( context );

    // Read the file.
    try { document.load ( in, context ); }

    // Catch exceptions from cppdom.
    catch ( const cppdom::Error &e )
    {
      _error.error ( e );
      _error.context ( context );
      _errorPolicy ( 1071100499, false );
      return;
    }

    // A well-formed xml document has only one root node. However, cppdom 
    // puts the contents of the document under its own root node. Therefore, 
    // the document node should only have one child, and it should be the 
    // root from the file.
    cppdom::NodeList &children = document.getChildren();
    _errorPolicy ( 1071116734, 1 == children.size() );

    // Traverse the tree.
    String hierarchy;
    this->_traverse ( hierarchy, *(children.front()) );

    // Initialize the data members again because they point to the document 
    // which is about to go out of scope.
    this->_init();
  }
#endif

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Flag for which callback list to put the callback.
  //
  /////////////////////////////////////////////////////////////////////////////

  enum WhichCallback
  {
    NODE_START,
    NODE_END
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Add a callback to be called when the given hierarchy is current.
  //
  /////////////////////////////////////////////////////////////////////////////

  void add ( WhichCallback which, const String &hierarchy, const NodeCB &callback )
  {
    switch ( which )
    {
    case NODE_START:
      _startCBs[hierarchy] = callback;
      break;
    case NODE_END:
      _endCBs[hierarchy] = callback;
      break;
    default:
      _errorPolicy ( 2080602521u, false );
      break;
    }
  }


protected:


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Call the callback if there is one.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _notify ( Callbacks &callbacks, Node &node )
  {
    // See if there is a callback.
    typename Callbacks::iterator i = callbacks.find ( _hierarchy );
    if ( callbacks.end() == i )
      return;

    // Call the callback. See Filter.h before you change the order.
    NodeCB &cb = i->second;
    FilterPolicy::call ( cb, node.name(), node.value(), *this, node, _hierarchy );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Traverse the tree.
  //
  /////////////////////////////////////////////////////////////////////////////
#if 0
  void _traverse ( const String &hierarchy, Node &node )
  {
    // Get the name of this node.
    String name ( node.getName() );
    _errorPolicy ( 1071116174, false == name.empty() );

    // String containing the current hierarchy.
    String ch;

    // If the flag says, add the name of this node to current hierarchy.
    if ( ( this->flags() & USE_HIERARCHY ) == USE_HIERARCHY )
    {
      ch = hierarchy;
      if ( !ch.empty() )
        ch += '/';
      ch += name;
    }

    // Otherwise, the name is the hierarchy.
    else
      ch = name;

    // Call any start-callbacks if we are supposed to.
    this->_notify ( ch, name, _startCBs, node );

    // Get the list of children.
    cppdom::NodeList &kids = node.getChildren();

    // Call this function for all the children that are not "cdata".
    for ( cppdom::NodeList::iterator i = kids.begin(); i != kids.end(); ++i )
    {
      cppdom::NodePtr &node = *i;
      _errorPolicy ( 1071119099, 0x0 != node.get() );
      if ( cppdom::xml_nt_cdata != node->getType() )
        this->_traverse ( ch, *node );
    }

    // Call any finish-callbacks if we are supposed to.
    this->_notify ( ch, name, _endCBs, node );
  }
#endif

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Initialize (some of) the members.
  //
  /////////////////////////////////////////////////////////////////////////////

  void clear()
  {
    _hierarchy.clear();
  }


private:

  Callbacks _startCBs;
  Callbacks _endCBs;
  String _hierarchy;
  ErrorPolicy _errorPolicy;
};


}; // namespace XML


#endif // _XML_DOM_VISITOR_CLASS_H_
