
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A node in an xml tree.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_NODE_CLASS_H_
#define _XML_NODE_CLASS_H_

#include <list>
#include <algorithm>


namespace XML {


template
<
  class StringType, 
  class ErrorPolicyType, 
  bool CREATE_MISSING_CHILDREN
>
class Node
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Internal class to handle reference counting.
  //
  /////////////////////////////////////////////////////////////////////////////

  class Pointer
  {
  public:
    Pointer() : _n ( 0x0 ){}
    Pointer ( Node *n ) : _n ( n ) { if ( _n ) _n->ref(); }
    Pointer ( const Pointer &p ) : _n ( p._n ) { if ( _n ) _n->ref(); }
    Pointer &operator = ( const Pointer &p ) { _n = p._n; if ( _n ) _n->ref(); return *this; }
    ~Pointer() { if ( _n ) _n->unref(); }
    const Node *operator -> () const { check ( _n ); return _n; }
    Node *      operator -> ()       { check ( _n ); return _n; }
    operator Node *()             { return _n; }
    operator const Node *() const { return _n; }
    bool valid() const { return ( 0x0 != _n ); }
    static void check ( const Node *n )
    {
      if ( !n )
      {
        // We don't want the error policy to be a data member because the 
        // nodes should be as small as possible. So declare this inside 
        // the if-statement.
        ErrorPolicy() ( false );
      }
    }
  private:
    Node *_n;
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef std::list < Pointer > Children;
  typedef StringType String;
  typedef typename String::value_type Char;
  typedef typename Children::iterator Iterator;
  typedef typename Children::const_iterator ConstIterator;
  typedef typename Children::size_type ChildIndex;
  typedef ErrorPolicyType ErrorPolicy;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Predicate to test for an equal name.
  //
  /////////////////////////////////////////////////////////////////////////////

  struct EqualName
  {
    EqualName ( const StringType &name ) : _name ( name ){}
    bool operator() ( const Pointer &p ) { return ( _name == p->getName() ); }
  private:
    StringType _name;
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Predicate to test for the i'th equal name.
  //
  /////////////////////////////////////////////////////////////////////////////

  struct IthEqualName
  {
    IthEqualName ( const StringType &name, ChildIndex i ) : _name ( name ), _i ( i ), _count ( 0 ){}
    bool operator() ( const Pointer &p )
    {
      if ( _name == p->getName() )
      {
        if ( _i == _count++ )
        {
          return true;
        }
      }
      return false;
    }
  private:
    StringType _name;
    ChildIndex _i;
    ChildIndex _count;
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Node ( const String &name = String(), const String &value = String(), const String &attributes = String() ) :
    _children(),
    _name ( name ),
    _value ( value ),
    _attributes ( attributes ),
    _refCount ( 0 )
  {
    // Empty.
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Node ( const Node &n ) :
    _children   ( n._children ),
    _name       ( n._name ),
    _value      ( n._value ),
    _attributes ( n._attributes ),
    _refCount   ( 0 )
  {
    // Empty.
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  Node &operator = ( const Node &n )
  {
    _children   = n._children;
    _name       = n._name;
    _value      = n._value;
    _attributes = n._attributes;

    // Note: _refCount is unchanged.

    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Append the new child.
  //
  /////////////////////////////////////////////////////////////////////////////

  void appendChild ( Node *n )
  {
    Pointer::check ( n );
    _children.push_back ( Pointer ( n ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the i'th child node.
  //
  /////////////////////////////////////////////////////////////////////////////

  Pointer getChild ( ChildIndex i ) const
  {
    if ( i >= 0 && i < _children.size() ) 
      return this->_getMissingChild();

    ConstIterator it = _children.begin();
    std::advance ( it, i );
    return *i;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the i'th child by name.
  //
  /////////////////////////////////////////////////////////////////////////////

  Pointer getChild ( ChildIndex i, const StringType &name ) const
  {
    ConstIterator it = std::find_if ( this->begin(), this->end(), IthEqualName ( name, i ) );
    return ( it == this->end() ) ? this->_getMissingChild() : *it;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the child node by the given path.
  //
  /////////////////////////////////////////////////////////////////////////////

  Pointer getChild ( const StringType &path, Char delim ) const
  {
    // Get the child, which may be null.
    Pointer child = this->_getChild ( path, delim );

    // Only return null if we are supposed to.
    return ( child.valid() ) ? child : this->_getMissingChild();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the number of children.
  //
  /////////////////////////////////////////////////////////////////////////////

  ChildIndex getNumChildren() const
  {
    return _children.size();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the number of attributes.
  //
  /////////////////////////////////////////////////////////////////////////////

  unsigned int getNumAttributes() const
  {
    ErrorPolicy() ( false ); // TODO.
    return 0;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the name of this node.
  //
  /////////////////////////////////////////////////////////////////////////////

  const StringType &getName() const
  {
    return _name;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the value, which should be empty if there are children.
  //
  /////////////////////////////////////////////////////////////////////////////

  const StringType &getValue() const
  {
    return _value;
  }

  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the i'th attribute.
  //
  /////////////////////////////////////////////////////////////////////////////

  StringType getAttribute ( unsigned int i )
  {
    ErrorPolicy() ( false ); // TODO.
  }

  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the attribute by name.
  //
  /////////////////////////////////////////////////////////////////////////////

  StringType getAttribute ( const StringType &attribName )
  {
    ErrorPolicy() ( false ); // TODO.
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the name.
  //
  /////////////////////////////////////////////////////////////////////////////

  void setName ( const StringType &name )
  {
    _name = name;
  }

  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the value.
  //
  /////////////////////////////////////////////////////////////////////////////

  void setValue ( const StringType &value )
  {
    _value = value;
  }

  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the attribute. This will add a new attribute of the given attribute 
  //  name does not exist.
  //
  /////////////////////////////////////////////////////////////////////////////

  void setAttribute ( const StringType &attribName, const StringType &attribValue )
  {
    ErrorPolicy() ( false ); // TODO.
  }

  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Reference this node.
  //
  /////////////////////////////////////////////////////////////////////////////

  void ref()
  {
    ++_refCount;
  }

  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Unreference this node.
  //
  /////////////////////////////////////////////////////////////////////////////

  void unref()
  {
    ErrorPolicy() ( _refCount > 0 );

    if ( 0 == ( --_refCount ) )
      delete this;
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  ///  Return the first iterator.
  ///
  /////////////////////////////////////////////////////////////////////////////

  Iterator begin()
  {
    return _children.begin();
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  ///  Return the first iterator.
  ///
  /////////////////////////////////////////////////////////////////////////////

  ConstIterator begin() const
  {
    return _children.begin();
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  ///  Return one past the last iterator.
  ///
  /////////////////////////////////////////////////////////////////////////////

  Iterator end()
  {
    return _children.end();
  }


  /////////////////////////////////////////////////////////////////////////////
  ///
  ///  Return one past the last iterator.
  ///
  /////////////////////////////////////////////////////////////////////////////

  ConstIterator end() const
  {
    return _children.end();
  }

  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Small functor to accept a visitor.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Visitor > struct Accept
  {
    Accept ( const Visitor &visitor ) : _visitor ( visitor ){}
    void operator() ( Pointer p ) { p->accept ( _visitor ); }
  private:
    Visitor _visitor;
  };

  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Accept the visitor.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Visitor > void accept ( Visitor visitor )
  {
    // Visit this node.
    visitor ( Pointer ( this ) );

    // Visit all the children.
    std::for_each ( this->begin(), this->end(), Accept<Visitor> ( visitor ) );
  }


protected:

  Children _children;
  StringType _name;
  StringType _value;
  StringType _attributes;
  unsigned int _refCount;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ~Node()
  {
    // Empty.
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the child node by the given path.
  //
  /////////////////////////////////////////////////////////////////////////////

  Pointer _getChild ( const StringType &path, Char delim ) const
  {
    typedef typename StringType::const_iterator SI;

    // Look for the first name in the path.
    SI last = std::find ( path.begin(), path.end(), delim );

    // Make the name.
    String name ( path.begin(), last );

    // If we're already at the end of the path-string...
    if ( last == path.end() )
    {
      // Get the first child by this name. Do not call getName(), we have 
      // to return an invalid node if we didn't find one.
      ConstIterator it = std::find_if ( this->begin(), this->end(), IthEqualName ( name, 0 ) );
      return ( it == this->end() ) ? Pointer ( 0x0 ) : *it;
    }

    // Increment now.
    ++last;

    // Loop through the children.
    for ( ConstIterator i = _children.begin(); i != _children.end(); ++i )
    {
      // Get the child.
      Pointer child ( *i );

      // Do the names match?
      if ( name == child->getName() )
      {
        // Call this function recursively with the rest of the path-string.
        String temp ( last, path.end() );
        child = child->_getChild ( temp, delim );

        // Did we find it?
        if ( child.valid() )
        {
          return child;
        }
      }
    }

    // We didn't find anything.
    return Pointer ( 0x0 );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Small functor to create a new node.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < bool create > struct MissingChildrenPolicy;
  template <> struct MissingChildrenPolicy < true >
  {
    Pointer operator ()()
    {
      return Pointer ( new Node );
    }
  };
  template <> struct MissingChildrenPolicy < false >
  {
    Pointer operator ()()
    {
      return Pointer ( 0x0 );
    }
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the missing child. Either creates a new node or returns null.
  //
  /////////////////////////////////////////////////////////////////////////////

  Pointer _getMissingChild() const
  {
    return MissingChildrenPolicy<CREATE_MISSING_CHILDREN>()();
  }
};


}; // namespace XML


#endif // _XML_NODE_CLASS_H_
