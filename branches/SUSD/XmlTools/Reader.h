
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  XML reader class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_TOOLS_READ_H_
#define _XML_TOOLS_READ_H_

#include "XmlTools/Error.h"

#include "cppdom/cppdom.h"
#include "cppdom/shared_ptr.h"

#include <fstream>
#include <string>


namespace XML {


template
<
  class CallbackType_,
  class CallbackPolicy_,
  class ErrorPolicy_
>
class Reader
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef CallbackType_ CallbackType;
  typedef CallbackPolicy_ CallbackPolicy;
  typedef ErrorPolicy_ ErrorPolicy;
  typedef std::map<std::string,CallbackType> Callbacks;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Possible flags.
  //
  /////////////////////////////////////////////////////////////////////////////

  enum
  {
    USE_HIERARCHY = 0x00000001
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructors.
  //
  /////////////////////////////////////////////////////////////////////////////

  Reader() : 
    _startCBs(), 
    _finishCBs(), 
    _error(),
    _hierarchy(),
    _name(),
    _value(),
    _attributes ( 0x0 ),
    _flags ( Reader::USE_HIERARCHY )
  {
  }
  Reader ( const Reader &r ) : 
    _startCBs   ( r._startCBs ),
    _finishCBs  ( r._finishCBs ),
    _error      ( r._error ),
    _hierarchy  ( r._hierarchy ),
    _name       ( r._name ),
    _value      ( r._value ),
    _attributes ( r._attributes ),
    _flags      ( r._flags )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Read the xml file and call any appropriate callbacks.
  //
  /////////////////////////////////////////////////////////////////////////////

  void read ( const std::string &filename )
  {
    std::ifstream in ( filename.c_str() );      // find file of name "filename"
    ErrorPolicy ( 1071099975, !!in );
    this->read ( in );                          // send this file to overloaded read function below
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Read the xml file and call any appropriate callbacks.
  //
  /////////////////////////////////////////////////////////////////////////////

  void read ( std::istream &in )
  {
    // Check input.
    ErrorPolicy ( 1071099976, !!in );

    // Initialize the data members.
    this->_init();

    // Declare a new document.
    cppdom::ContextPtr context ( new cppdom::Context );
    ErrorPolicy ( 1071099977, 0x0 != context.get() );
    cppdom::Document document ( context );

    // Read the file.
    try { document.load ( in, context ); }

    // Catch exceptions from cppdom.
    catch ( const cppdom::Error &e )
    {
      _error.error ( e );
      _error.context ( context );
      ErrorPolicy ( 1071100499, false );
      return;
    }

    // A well-formed xml document has only one root node. However, cppdom 
    // puts the contents of the document under its own root node. Therefore, 
    // the document node should only have one child, and it should be the 
    // root from the file.
    cppdom::NodeList &children = document.getChildren();
    ErrorPolicy ( 1071116734, 1 == children.size() );

    // Traverse the tree.
    std::string hierarchy;
    this->_traverse ( hierarchy, *(children.front()) );

    // Initialize the data members again because they point to the document 
    // which is about to go out of scope.
    this->_init();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Flag for which callback list to put the callback.
  //
  /////////////////////////////////////////////////////////////////////////////

  enum WhichCallback
  {
    NODE_START,
    NODE_FINISH
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Add a callback to be called when the given hierarchy is current.
  //
  /////////////////////////////////////////////////////////////////////////////

  void add ( WhichCallback which, const std::string &hierarchy, const CallbackType &callback )
  {
    switch ( which )
    {
    case NODE_START:
      _startCBs[hierarchy] = callback;
      break;
    case NODE_FINISH:
      _finishCBs[hierarchy] = callback;
      break;
    default:
      ErrorPolicy ( 1071199269, false );
      break;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the last error, if any.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Error &error() const
  {
    return _error;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the hierarchy.
  //
  /////////////////////////////////////////////////////////////////////////////

  const std::string &hierarchy() const
  {
    return _hierarchy;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the name.
  //
  /////////////////////////////////////////////////////////////////////////////

  const std::string &name() const
  {
    return _name;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the value.
  //
  /////////////////////////////////////////////////////////////////////////////

  const std::string &value() const
  {
    return _value;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the attribute map.
  //
  /////////////////////////////////////////////////////////////////////////////

  const cppdom::Attributes *attributes() const
  {
    return _attributes;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the flags.
  //
  /////////////////////////////////////////////////////////////////////////////

  unsigned int flags() const
  {
    return _flags;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the flags.
  //
  /////////////////////////////////////////////////////////////////////////////

  void flags ( unsigned int f )
  {
    _flags = f;
  }


protected:


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Call the callback if there is one.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _notify ( const std::string &hierarchy, const std::string &name, 
                 Callbacks &callbacks, cppdom::Node &node )
  {
    // See if there is a callback.
    typename Callbacks::iterator i = callbacks.find ( hierarchy );
    if ( callbacks.end() == i )
      return;

    // Set the data members.
    _hierarchy = hierarchy;
    _name = name;
    _value = node.getCdata();
    _attributes = &(node.getAttrMap());

    // Call the callback.
    CallbackType &cb = i->second;
    CallbackPolicy::call ( *this, cb );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Traverse the tree.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _traverse ( const std::string &hierarchy, cppdom::Node &node )
  {
    // Get the name of this node.
    std::string name ( node.getName() );
    ErrorPolicy ( 1071116174, false == name.empty() );

    // String containing the current hierarchy.
    std::string ch;

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
      ErrorPolicy ( 1071119099, 0x0 != node.get() );
      if ( cppdom::Node::xml_nt_cdata != node->getType() )
        this->_traverse ( ch, *node );
    }

    // Call any finish-callbacks if we are supposed to.
    this->_notify ( ch, name, _finishCBs, node );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Initialize (some of) the members.
  //
  /////////////////////////////////////////////////////////////////////////////

  void _init()
  {
    _hierarchy  = "";
    _name       = "";
    _value      = "";
    _attributes = 0x0;
  }


private:

  Callbacks _startCBs;
  Callbacks _finishCBs;
  Error _error;
  std::string _hierarchy;
  std::string _name;
  std::string _value;
  cppdom::Attributes *_attributes;
  unsigned int _flags;
};


}; // namespace XML


#endif // _XML_TOOLS_READ_H_
