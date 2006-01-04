
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

  // Get the i'th child with the given name. Create it if needed.
  Node *                  child ( unsigned int i, const std::string &name );

  // Access the children.
  const Children &        children() const { return _children; }
  Children &              children()       { return _children; }

  // Clear the document.
  void                    clear();

  // Access the name.
  const std::string &     name() const { return _name; }
  void                    name ( const std::string &n ) { _name = n; }

  // Access the value.
  const std::string &     value() const { return _value; }
  void                    value ( const std::string &v ) { _value = v; }

protected:

  // Use reference counting.
  virtual ~Node();

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
