
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
#include "Usul/Pointers/Pointers.h"

#include <string>
#include <sstream>
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

  // Construction
  Node ( const std::string &name, const std::string &value = std::string() );
  template < class T > Node ( const std::string &name, T value );

  // Access the attributes.
  const Attributes &      attributes() const { return _attributes; }
  Attributes &            attributes()       { return _attributes; }

  // Get the i'th child (or grandchild, or any level of decendent)) at the 
  // given "path". Creates all necessary child nodes if needed. The path is 
  // much like a file path, but choose whatever deliminter you like. If there 
  // are no delimiters in the path, then the entire path is interpreted as 
  // the node-name.
  Node *                  child ( unsigned int i, const std::string &path, const char delim = '/' );

  // Access the children.
  const Children &        children() const { return _children; }
  Children &              children()       { return _children; }

  // Clear the document.
  void                    clear();

  // Access the name.
  const std::string &     name() const { return _name; }
  void                    name ( const std::string &n );

  // Access the value.
  const std::string &     value() const { return _value; }
  void                    value ( const std::string &v );

protected:

  // Use reference counting.
  virtual ~Node();

  Node *                  _child ( unsigned int i, const std::string &name );

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
  _value      (),
  _attributes (),
  _children   ()
{
  std::ostringstream out;
  out << value;
  _value = out.str();
}


} // namespace XmlTree


#endif // _XML_TREE_NODE_H_
