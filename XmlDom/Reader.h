
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  An xml reader.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_READER_CLASS_H_
#define _XML_READER_CLASS_H_

#include <stack>


namespace XML {


template
<
  class NodeType,
  class ErrorPolicyType,
  class NodeCallbackType,
  class TrimPolicyType
>
class Reader
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef NodeType Node;
  typedef typename Node::Pointer Pointer;
  typedef typename Node::String String;
  typedef ErrorPolicyType ErrorPolicy;
  typedef NodeCallbackType NodeCallback;
  typedef TrimPolicyType TrimPolicy;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  explicit Reader() :
    _root(),
    _parents(),
    _errorPolicy(),
    _callback(),
    _trimPolicy()
  {
    // Empty.
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor. Takes bi-directional iterators.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Bi > Reader ( const Bi &first, const Bi &last ) :
    _root(),
    _parents(),
    _errorPolicy(),
    _callback(),
    _trimPolicy()
  {
    this->read ( first, last );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Reader ( const Reader &r ) :
    _root ( r._root ),
    _parents ( r._parents ),
    _errorPolicy ( r._errorPolicy ),
    _callback ( r._callback ),
    _trimPolicy ( r._trimPolicy )
  {
    // Empty.
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ~Reader()
  {
    // Should be true.
    _errorPolicy ( _parents.empty() );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  Reader &operator = ( const Reader &n )
  {
    _root = r._root;
    _parents = r._parents;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the root node.
  //
  /////////////////////////////////////////////////////////////////////////////

  Pointer getRoot() const
  {
    return _root;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Construct the node tree from the bi-directional iterators.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Bi > void read ( Bi start, const Bi &stop )
  {
    _errorPolicy ( start != stop );

    // Skip the header.
    start = this->_skipHeader ( start, stop );

    // Call the other one.
    start = this->_read ( start, stop );

    // Should be true.
    _errorPolicy ( start == stop );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access to the error policy.
  //
  /////////////////////////////////////////////////////////////////////////////

  const ErrorPolicyType &getErrorPolicy() const
  {
    return _errorPolicy;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access to the error policy.
  //
  /////////////////////////////////////////////////////////////////////////////

  ErrorPolicyType &getErrorPolicy()
  {
    return _errorPolicy;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access to the trim policy.
  //
  /////////////////////////////////////////////////////////////////////////////

  const TrimPolicyType &getTrimPolicy() const
  {
    return _trimPolicy;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access to the error policy.
  //
  /////////////////////////////////////////////////////////////////////////////

  TrimPolicyType &getTrimPolicy()
  {
    return _trimPolicy;
  }


protected:

  typedef std::stack<Pointer> ParentStack;
  Pointer _root;
  ParentStack _parents;
  ErrorPolicy _errorPolicy;
  NodeCallback _callback;
  TrimPolicy _trimPolicy;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the current parent.
  //
  /////////////////////////////////////////////////////////////////////////////

  Pointer _getParent() const
  {
    _errorPolicy ( !_parents.empty() );
    return _parents.top();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Push the parent.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _pushParent ( Pointer parent )
  {
    _errorPolicy ( _root.valid() );
    _parents.push ( parent );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Pop the parent.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _popParent()
  {
    _errorPolicy ( !_parents.empty() );
    _parents.pop();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Skip the header.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Bi > Bi _skipHeader ( Bi start, const Bi &stop ) const
  {
    _errorPolicy ( start != stop );

    // TODO.

    // Return the new starting position.
    return start;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Append the node.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _append ( Pointer child )
  {
    _errorPolicy ( child.valid() );

    // The first time assign the root.
    if ( !_root )
    {
      _errorPolicy ( _parents.empty() );
      _root = child;
    }

    // Otherwise, add it to the current parent.
    else
    {
      this->_getParent()->appendChild ( child );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called at the end of the name.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _end ( const String &name )
  {
    _errorPolicy ( !name.empty() );

    // Call the callback.
    _callback.endNode ( name );

    // Done with the current parent.
    this->_popParent();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called at the start of the name.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Bi > void _start ( const String &name, const Bi &start, const Bi &stop )
  {
    _errorPolicy ( !name.empty() );

    // Find the value.
    Bi end = std::find ( start, stop, '<' );

    // Grab the string.
    String value ( start, end );

    // Lose leading and trailing white space if we are supposed to.
    _trimPolicy ( value );

    // Call the callback.
    _callback.startNode ( name, value );

    // Make a new node.
    Pointer node ( new Node ( name, value ) );

    // Add it to our tree.
    this->_append ( node );

    // This node becomes the new parent. If there are no children
    // then it will be popped in "_end()".
    this->_pushParent ( node );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Called when a name is read.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Bi > void _notify ( const String &name, const Bi &start, const Bi &stop )
  {
    _errorPolicy ( !name.empty() );

    if ( '/' == name[0] )
      this->_end ( String ( name.begin() + 1, name.end() ) );
    else
      this->_start ( name, start, stop );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Read from the bi-directional iterators.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Bi > Bi _read ( Bi start, const Bi &stop )
  {
    _errorPolicy ( start != stop );

    while ( start != stop )
    {
      start = this->_readNode ( start, stop );
    }

    return stop;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Read from the bi-directional iterators.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Bi > Bi _readNode ( Bi start, const Bi &stop )
  {
    // See if we are at the end.
    if ( start == stop )
      return stop;

    // Find the start of the next name.
    Bi i0 = std::find ( start, stop, '<' );

    // Did we get to the end?
    if ( i0 == stop )
      return stop;

    // Find the end of the next name.
    std::advance ( i0, 1 );
    Bi i1 = std::find ( i0, stop, '>' );

    // Grab the name.
    String name ( i0, i1 );

    // Look for the end of this name.
    String s ( String ( "</" ) + name + String ( ">" ) );
    std::advance ( i1, 1 );
    Bi i2 = std::search ( i1, stop, s.begin(), s.end() );

    // Notify the client.
    this->_notify ( name, i1, i2 );

    // Call this function recursively.
    return this->_readNode ( i1, i2 );
  }
};


}; // namespace XML


#endif // _XML_READER_CLASS_H_
