
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

#ifndef _XML_TREE_NODE_H_
#define _XML_TREE_NODE_H_

#include "XmlTree/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Pointers/Pointers.h"

#include <string>
#include <map>
#include <vector>


namespace XmlTree {


class XML_TREE_EXPORT Node : public Usul::Base::Referenced
{
public:

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Node );

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef std::map < std::string, std::string > Attributes;
  typedef std::vector < Node::ValidRefPtr > Children;

  // Type id.
  USUL_DECLARE_TYPE_ID ( Node );

  // Construction
  Node ( const std::string &name, const std::string &value = std::string() );
  template < class T > Node ( const std::string &name, T value );

  // Append the child.
  void                    append ( Node * );
  Node::RefPtr            append ( const std::string &name, const std::string &value = std::string() );

  // Get the attribute. Returns empty string if it's not there.
  std::string             attribute ( const std::string & ) const;

  // Access the attributes.
  const Attributes &      attributes() const { return _attributes; }
  Attributes &            attributes()       { return _attributes; }

  // Get the i'th child (or grandchild, or any level of decendent)) at the 
  // given "path". If "createIfNeeded" is true, creates all necessary child 
  // nodes as needed. If false, returns NULL if child is not found. The path 
  // is much like a file path, but choose whatever deliminter you like. If 
  // there are no delimiters in the path, then the entire path is interpreted 
  // as the node-name. Throws an exception if it was asked to create but failed.
  Node *                  child ( unsigned int i, const std::string &path, const char delim = '/', bool createIfNeeded = true );

  // Get the first child by the name.
  Node::RefPtr            child ( const std::string &name, bool createIfNeeded );
  Node::RefPtr            child ( const std::string &name ) const;

  // Access the children.
  const Children &        children() const { return _children; }
  Children &              children()       { return _children; }

  // Clear the node.
  void                    clear();

  // Find all children with the given name. Pass true for "traverse" if you 
  // want to search all children, false if just immediate children.
  void                    find ( const std::string &name, bool traverse, Children & ) const;
  Children                find ( const std::string &name, bool traverse ) const;

  // Find all children with the given predicate. Pass true for "traverse" if you 
  // want to search all children, false if just immediate children.
  template< class Predicate >
  void                    findIf ( bool traverse, Children &, const Predicate& ) const;
  
  template< class Predicate >
  Children                findIf ( bool traverse, const Predicate& ) const;

  // Access the name.
  const std::string &     name() const { return _name; }
  void                    name ( const std::string &n );

  // Access the value.
  const std::string &     value() const { return _value; }
  void                    value ( const std::string &v );

protected:

  // Use reference counting.
  virtual ~Node();

  Node *                  _child ( unsigned int i, const std::string &name, bool createIfNeeded );

private:

  // Do not copy.
  Node ( const Node & );
  Node &operator = ( const Node & );

  std::string _name;
  std::string _value;
  Attributes _attributes;
  Children _children;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Template constructor.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > inline Node::Node ( const std::string &name, T value ) : BaseClass(),
  _name       ( name ),
  _value      ( Usul::Convert::Type<T,std::string>::convert ( value ) ),
  _attributes (),
  _children   ()
{
}
 
  
///////////////////////////////////////////////////////////////////////////////
//
//  Find all children with the given predicate. Pass true for "traverse" if you 
//  want to search all children, false if just immediate children.
//
///////////////////////////////////////////////////////////////////////////////

template < class Predicate >
void Node::findIf ( bool traverse, Children & all, const Predicate& pred ) const
{
  const Children &kids ( this->children() );
  for ( Children::const_iterator i = kids.begin(); i != kids.end(); ++i )
  {
    Node::RefPtr node ( const_cast<Node *> ( i->get() ) );
    if ( true == node.valid() )
    {
      if ( pred ( node->name() ) )
      {
        all.push_back ( node.get() );
      }
      
      if ( true == traverse )
      {
        node->findIf ( traverse, all, pred );
      }
    }
  }
}

  
///////////////////////////////////////////////////////////////////////////////
//
//  Find all children with the given predicate. Pass true for "traverse" if you 
//  want to search all children, false if just immediate children.
//
///////////////////////////////////////////////////////////////////////////////

template < class Predicate >
Node::Children Node::findIf ( bool traverse, const Predicate& pred ) const
{
  Children children;
  this->findIf ( traverse, children, pred );
  return children;
}


} // namespace XmlTree


#endif // _XML_TREE_NODE_H_
