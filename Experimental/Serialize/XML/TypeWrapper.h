
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper classes to operate on specific data types.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SERIALIZE_XML_SET_VALUE_H_
#define _SERIALIZE_XML_SET_VALUE_H_

#include "Serialize/XML/Factory.h"

#include "XmlTree/Node.h"

#include "Usul/Pointers/Pointers.h"

#include <sstream>


namespace Serialize {
namespace XML {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic types should not compile.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct TypeWrapper;


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to specialize built-in types.
//
///////////////////////////////////////////////////////////////////////////////

#define SERIALIZE_XML_DECLARE_TYPE_WRAPPER(TheType)\
template <> struct TypeWrapper < TheType >\
{\
  typedef TypeWrapper < TheType > ThisType;\
  static void addAtribute ( const std::string &name, const std::string &value, XmlTree::Node &node )\
  {\
  }\
  static const char *className ( const TheType &value )\
  {\
    return #TheType;\
  }\
  static TheType create ( const std::string &typeName )\
  {\
    return TheType();\
  }\
  static void deserialize ( const XmlTree::Node &node, TheType &value )\
  {\
    ThisType::set ( node.value(), value );\
  }\
  static void getAttribute ( const std::string &name, const XmlTree::Node &node, std::string &value )\
  {\
    value.clear();\
  }\
  static bool isValid ( const TheType &value )\
  {\
    return true;\
  }\
  static void serialize ( const TheType &value, XmlTree::Node &node )\
  {\
    std::ostringstream out;\
    out << value;\
    node.value ( out.str() );\
  }\
  static void set ( const std::string &s, TheType &value )\
  {\
    if ( false == s.empty() )\
    {\
      std::istringstream in ( s );\
      in >> value;\
    }\
  }\
}


///////////////////////////////////////////////////////////////////////////////
//
//  Specializations built-in types.
//
///////////////////////////////////////////////////////////////////////////////

SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( char  );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( short );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( int   );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( long  );

SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( unsigned char  );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( unsigned short );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( unsigned int   );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( unsigned long  );

SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( float );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( double );


///////////////////////////////////////////////////////////////////////////////
//
//  Specialization for strings.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct TypeWrapper < std::string >
{
  typedef TypeWrapper < std::string > ThisType;
  static void addAtribute ( const std::string &name, const std::string &value, XmlTree::Node &node )
  {
  }
  static const char *className ( const std::string &value )
  {
    return "std::string";
  }
  static std::string create ( const std::string &typeName )
  {
    return std::string();
  }
  static void deserialize ( const XmlTree::Node &node, std::string &value )
  {
    ThisType::set ( node.value(), value );
  }
  static void getAttribute ( const std::string &name, const XmlTree::Node &node, std::string &value )
  {
    value.clear();
  }
  static bool isValid ( const std::string &value )
  {
    return true;
  }
  static void set ( const std::string &s, std::string &value )
  {
    value = s;
  }
  static void serialize ( const std::string &value, XmlTree::Node &node )
  {
    node.value ( value );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Specialization for smart-pointers.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class C > struct TypeWrapper < Usul::Pointers::SmartPointer < T, C > >
{
  typedef Usul::Pointers::SmartPointer < T, C > PointerType;
  typedef typename PointerType::element_type ObjectType;

  static void addAtribute ( const std::string &name, const std::string &value, XmlTree::Node &node )
  {
    node.attributes()[name] = value;
  }
  static const char *className ( const PointerType &value )
  {
    return ( ( true == value.valid() ) ? value->className() : "" );
  }
  static ObjectType *create ( const std::string &typeName )
  {
    return ( dynamic_cast < ObjectType * > ( Factory::instance().create ( typeName ) ) );
  }
  static void deserialize ( const XmlTree::Node &node, PointerType &value )
  {
    if ( true == value.valid() )
    {
      value->deserialize ( node );
    }
  }
  static void getAttribute ( const std::string &name, const XmlTree::Node &node, std::string &value )
  {
    XmlTree::Node::Attributes::const_iterator i = node.attributes().find ( name );
    value.assign ( ( node.attributes().end() == i ) ? "" : i->second );
  }
  static bool isValid ( const PointerType &value )
  {
    return ( true == value.valid() );
  }
  static void serialize ( const PointerType &value, XmlTree::Node &parent )
  {
    if ( true == value.valid() )
    {
      value->serialize ( parent );
    }
  }
};


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_SET_VALUE_H_
