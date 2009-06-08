
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

#include "XmlTree/Node.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Convert/Vector2.h"
#include "Usul/Convert/Vector3.h"
#include "Usul/Convert/Vector4.h"
#include "Usul/Convert/Matrix44.h"
#include "Usul/Factory/ObjectFactory.h"
#include "Usul/Interfaces/ISerialize.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/Atomic.h"
#include "Usul/Types/Types.h"

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
  static void replaceIllegalCharacters ( std::string &s )
  {
    // Illegal characters will be removed when the file is written to xml.
  }
  static std::string restoreIllegalCharacters ( const std::string &in )
  {
    // Illegal characters are restored when file is read.
    return in;
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
//  Specialization for query-pointers.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class C > struct TypeWrapper < Usul::Pointers::QueryPointer < T, C > >
{
  typedef Usul::Pointers::QueryPointer < T, C > PointerType;
  typedef typename PointerType::element_type ObjectType;
  typedef Usul::Factory::ObjectFactory Factory;

  static void addAtribute ( const std::string &name, const std::string &value, XmlTree::Node &node )
  {
    node.attributes()[name] = value;
  }
  static const char *className ( const PointerType &value )
  {
    Usul::Interfaces::ISerialize::QueryPtr iSerialize ( const_cast < typename PointerType::element_type* > ( value.get() ) );
    return ( ( true == iSerialize.valid() ) ? iSerialize->className() : "" );
  }
  static ObjectType *create ( const std::string &typeName )
  {
    Usul::Base::Referenced *referenced ( Factory::instance().create ( typeName ) );
    
    if ( 0x0 != referenced )
    {
      PointerType unknown ( referenced->asUnknown() );
      if ( unknown.valid() )
        return unknown.release();
    }
    
    ObjectType * object ( dynamic_cast < ObjectType* > ( referenced ) );
    return object;
  }
  static void deserialize ( const XmlTree::Node &node, PointerType &value )
  {
    Usul::Interfaces::ISerialize::QueryPtr iSerialize ( value );
    if ( true == iSerialize.valid() )
    {
      iSerialize->deserialize ( node );
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
    Usul::Interfaces::ISerialize::QueryPtr iSerialize ( const_cast < typename PointerType::element_type* > ( value.get() ) );
    if ( true == iSerialize.valid() )
    {
      iSerialize->serialize ( parent );
    }
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
  typedef Usul::Factory::ObjectFactory Factory;

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
  

///////////////////////////////////////////////////////////////////////////////
//
//  Specialization for strings.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> struct TypeWrapper < Usul::Threads::Atomic<T> >
{
  typedef T ValueType;
  typedef Usul::Threads::Atomic<T> AtomicType;
  typedef TypeWrapper < AtomicType > ThisType;

  static void addAtribute ( const std::string &name, const std::string &value, XmlTree::Node &node )
  {
  }
  static const char *className ( const std::string &value )
  {
    return "Usul::Threads::Atomic";
  }
  static ThisType create ( const std::string &typeName )
  {
    return ThisType();
  }
  static void deserialize ( const XmlTree::Node &node, AtomicType &value )
  {
    ThisType::set ( node.value(), value );
  }
  static void getAttribute ( const std::string &name, const XmlTree::Node &node, ThisType &value )
  {
  }
  static bool isValid ( const std::string &value )
  {
    return true;
  }
  static void set ( const std::string &s, AtomicType &value )
  {
    if ( false == s.empty() )
    {
      value.fetch_and_store ( Usul::Convert::Type<std::string,ValueType>::convert ( s ) );
    }
  }
  static void serialize ( const AtomicType &value, XmlTree::Node &node )
  {
    node.value ( Usul::Convert::Type<ValueType,std::string>::convert ( value ) );
  }
};


} // namespace Serialize
} // namespace XML


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to specialize built-in types.
//
///////////////////////////////////////////////////////////////////////////////

#define SERIALIZE_XML_DECLARE_TYPE_WRAPPER(TheType)\
namespace Serialize {\
namespace XML {\
template <> struct TypeWrapper < TheType >\
{\
  typedef TypeWrapper < TheType > ThisType;\
  typedef Usul::Factory::ObjectFactory Factory;\
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
    node.value ( Usul::Convert::Type<TheType,std::string>::convert ( value ) );\
  }\
  static void set ( const std::string &s, TheType &value )\
  {\
    if ( false == s.empty() )\
    {\
      value = Usul::Convert::Type<std::string,TheType>::convert ( s );\
    }\
  }\
};\
}\
}


///////////////////////////////////////////////////////////////////////////////
//
//  Specializations for built-in types.
//
///////////////////////////////////////////////////////////////////////////////

// These are the same as Usul::Types::IntXX below.
//SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( char  );
//SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( short );
//SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( int   );

SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Types::Int8 );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Types::Int16 );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Types::Int32 );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Types::Int64 );

SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( long  );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( bool  );

SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Types::Uint8  );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Types::Uint16 );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Types::Uint32 );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Types::Uint64 );

// These are the same as Usul::Types::FloatXX below.
//SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( float );
//SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( double );

SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Types::Float32 );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Types::Float64 );


/////////////////////////////////////////////////////////////////////////////
//
//  Declare known vector and matrix types.
//
/////////////////////////////////////////////////////////////////////////////

SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Math::Vec2d );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Math::Vec2f );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Math::Vec2ui );

SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Math::Vec3d );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Math::Vec3f );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Math::Vec3ui );

SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Math::Vec4d );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Math::Vec4f );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Math::Vec4ui );

SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Math::Matrix44d );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Math::Matrix44f );


///////////////////////////////////////////////////////////////////////////////
//
//  Macros to declare all plumbing for unknown vector and matrix types.
//  Use for osg::Vec3, osg::Matrix4, etc.
//
///////////////////////////////////////////////////////////////////////////////

#define SERIALIZE_XML_DECLARE_VECTOR_2_WRAPPER(the_type)\
  USUL_CONVERT_DEFINE_STRING_CONVERTER(the_type);\
  SERIALIZE_XML_DECLARE_TYPE_WRAPPER(the_type);

#define SERIALIZE_XML_DECLARE_VECTOR_3_WRAPPER(the_type)\
  USUL_CONVERT_DEFINE_STRING_CONVERTER(the_type);\
  SERIALIZE_XML_DECLARE_TYPE_WRAPPER(the_type);

#define SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER(the_type)\
  USUL_CONVERT_DEFINE_STRING_CONVERTER(the_type);\
  SERIALIZE_XML_DECLARE_TYPE_WRAPPER(the_type);

#define SERIALIZE_XML_DECLARE_MATRIX_4_4_WRAPPER(the_type)\
  USUL_CONVERT_DEFINE_STRING_CONVERTER(the_type);\
  SERIALIZE_XML_DECLARE_TYPE_WRAPPER(the_type);


///////////////////////////////////////////////////////////////////////////////
//
//  Macros to declare stream operators for simple vector types.
//
///////////////////////////////////////////////////////////////////////////////

#if 0

#define SERIALIZE_XML_DEFINE_STREAM_FUNCTIONS_VECTOR_4(the_type)\
inline std::ostream &operator << ( std::ostream &out, const the_type &v )\
{\
  Usul::Scope::Stream::MaxFloatPrecision<the_type::value_type> precision ( out );\
  out << v[0] << ' ' << v[1] << ' ' << v[2] << ' ' << v[3];\
  return out;\
}\
inline std::istream &operator >> ( std::istream &in, the_type &v )\
{\
  in >> v[0] >> v[1] >> v[2] >> v[3];\
  return in;\
}

#define SERIALIZE_XML_DEFINE_STREAM_FUNCTIONS_VECTOR_3(the_type)\
inline std::ostream &operator << ( std::ostream &out, const the_type &v )\
{\
  Usul::Scope::Stream::MaxFloatPrecision<the_type::value_type> precision ( out );\
  out << v[0] << ' ' << v[1] << ' ' << v[2];\
  return out;\
}\
inline std::istream &operator >> ( std::istream &in, the_type &v )\
{\
  in >> v[0] >> v[1] >> v[2];\
  return in;\
}

#define SERIALIZE_XML_DEFINE_STREAM_FUNCTIONS_VECTOR_2(the_type)\
inline std::ostream &operator << ( std::ostream &out, const the_type &v )\
{\
  Usul::Scope::Stream::MaxFloatPrecision<the_type::value_type> precision ( out );\
  out << v[0] << ' ' << v[1];\
  return out;\
}\
inline std::istream &operator >> ( std::istream &in, the_type &v )\
{\
  in >> v[0] >> v[1];\
  return in;\
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macros to declare stream operators for 4 by 4 matrix.
//
///////////////////////////////////////////////////////////////////////////////

#define SERIALIZE_XML_DEFINE_STREAM_FUNCTIONS_MATRIX_4_4(the_type)\
inline std::ostream &operator << ( std::ostream &out, const the_type &m )\
{\
  Usul::Scope::Stream::MaxFloatPrecision<the_type::value_type> precision ( out );\
\
  out << m( 0, 0 ) << ' ';\
  out << m( 1, 0 ) << ' ';\
  out << m( 2, 0 ) << ' ';\
  out << m( 3, 0 ) << ' ';\
  out << m( 0, 1 ) << ' ';\
  out << m( 1, 1 ) << ' ';\
  out << m( 2, 1 ) << ' ';\
  out << m( 3, 1 ) << ' ';\
  out << m( 0, 2 ) << ' ';\
  out << m( 1, 2 ) << ' ';\
  out << m( 2, 2 ) << ' ';\
  out << m( 3, 2 ) << ' ';\
  out << m( 0, 3 ) << ' ';\
  out << m( 1, 3 ) << ' ';\
  out << m( 2, 3 ) << ' ';\
  out << m( 3, 3 );\
\
  return out;\
}\
inline std::istream &operator >> ( std::istream &in, the_type &m )\
{\
  in >> m( 0, 0 ) >> m( 1, 0 ) >> m( 2, 0 )  >> m( 3, 0 )\
     >> m( 0, 1 ) >> m( 1, 1 ) >> m( 2, 1 )  >> m( 3, 1 )\
     >> m( 0, 2 ) >> m( 1, 2 ) >> m( 2, 2 )  >> m( 3, 2 )\
     >> m( 0, 3 ) >> m( 1, 3 ) >> m( 2, 3 )  >> m( 3, 3 );\
  return in;\
}

///////////////////////////////////////////////////////////////////////////////
//
//  Macros to declare all plumbing for matrix types.
//
///////////////////////////////////////////////////////////////////////////////

#define SERIALIZE_XML_DECLARE_MATRIX_4_4_WRAPPER(the_type)\
  SERIALIZE_XML_DEFINE_STREAM_FUNCTIONS_MATRIX_4_4(the_type);\
  SERIALIZE_XML_DECLARE_TYPE_WRAPPER(the_type);

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Specialization for container.
//
///////////////////////////////////////////////////////////////////////////////

#define SERIALIZE_XML_DECLARE_TYPE_WRAPPER_CONTAINER(the_container,the_type)\
namespace Serialize {\
namespace XML {\
template < > struct TypeWrapper < the_container < the_type > >\
{\
  typedef TypeWrapper < the_container < the_type > > ThisType;\
  static void addAtribute ( const std::string &name, const std::string &value, XmlTree::Node &node )\
  {\
  }\
  static const char *className ( const the_container < the_type > &value )\
  {\
    return #the_container;\
  }\
  static the_container<the_type> create ( const std::string &typeName )\
  {\
    return the_container<the_type>();\
  }\
  static void deserialize ( const XmlTree::Node &node, the_container < the_type >  &value )\
  {\
    typedef XmlTree::Node::Children::const_iterator Itr;\
    \
    value.clear();\
    \
    for ( Itr i = node.children().begin(); i != node.children().end(); ++i )\
    {\
      XmlTree::Node::RefPtr element ( i->get() );\
      if ( true == element.valid() && element->name() == "element" )\
      {\
        the_type object ( Usul::Convert::Type<std::string,the_type>::convert ( element->value() ) );\
        value.push_back ( object );\
      }\
    }\
  }\
  static void getAttribute ( const std::string &name, const XmlTree::Node &node, std::string &value )\
  {\
    value.clear();\
  }\
  static bool isValid ( const the_container < the_type >  &value )\
  {\
    return true;\
  }\
  static void set ( const std::string &s, the_container < the_type > &value )\
  {\
  }\
  static void serialize ( const the_container<the_type> &value, XmlTree::Node &node )\
  {\
    for ( the_container<the_type>::const_iterator iter = value.begin(); iter != value.end(); ++iter )\
    {\
      XmlTree::Node::RefPtr child ( new XmlTree::Node ( "element" ) );\
      child->value ( Usul::Convert::Type<the_type,std::string>::convert ( *iter ) );\
      node.append ( child.get() );\
    }\
  }\
};\
}\
}

#endif // _SERIALIZE_XML_SET_VALUE_H_
