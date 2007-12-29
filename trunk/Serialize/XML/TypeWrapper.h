
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
#include "XmlTree/ReplaceIllegalCharacters.h"

#include "Usul/Factory/ObjectFactory.h"
#include "Usul/Interfaces/ISerialize.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Types/Types.h"

#include <sstream>
#include <iomanip>

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
    XmlTree::replaceIllegalCharacters ( s );
  }
  static std::string restoreIllegalCharacters ( const std::string &in )
  {
    std::string out ( in );
    XmlTree::restoreIllegalCharacters ( out );
    return out;
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
    ObjectType * object ( dynamic_cast < ObjectType* > ( Factory::instance().create ( typeName ) ) );
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
  static void replaceIllegalCharacters ( TheType &in )\
  {\
  }\
  static std::string restoreIllegalCharacters ( const std::string &in )\
  {\
    return in;\
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
};\
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
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( bool  );

SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Types::Uint8  );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Types::Uint16 );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Types::Uint32 );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Types::Uint64 );

SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( float );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( double );


///////////////////////////////////////////////////////////////////////////////
//
//  Macros to declare stream operators for simple vector types.
//
///////////////////////////////////////////////////////////////////////////////

#define SERIALIZE_XML_DEFINE_STREAM_FUNCTIONS_VECTOR_4(the_type)\
inline std::ostream &operator << ( std::ostream &out, const the_type &v )\
{\
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
//  Macros to declare all plumbing for simple vector types.
//
///////////////////////////////////////////////////////////////////////////////

#define SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER(the_type)\
  SERIALIZE_XML_DEFINE_STREAM_FUNCTIONS_VECTOR_4(the_type);\
  SERIALIZE_XML_DECLARE_TYPE_WRAPPER(the_type);

#define SERIALIZE_XML_DECLARE_VECTOR_3_WRAPPER(the_type)\
  SERIALIZE_XML_DEFINE_STREAM_FUNCTIONS_VECTOR_3(the_type);\
  SERIALIZE_XML_DECLARE_TYPE_WRAPPER(the_type);

#define SERIALIZE_XML_DECLARE_VECTOR_2_WRAPPER(the_type)\
  SERIALIZE_XML_DEFINE_STREAM_FUNCTIONS_VECTOR_2(the_type);\
  SERIALIZE_XML_DECLARE_TYPE_WRAPPER(the_type);


/////////////////////////////////////////////////////////////////////////////
//
//  Declare vector types.
//
/////////////////////////////////////////////////////////////////////////////

SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER ( Usul::Math::Vec4d );
SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER ( Usul::Math::Vec4f );
SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER ( Usul::Math::Vec4ui );

SERIALIZE_XML_DECLARE_VECTOR_3_WRAPPER ( Usul::Math::Vec3d );
SERIALIZE_XML_DECLARE_VECTOR_3_WRAPPER ( Usul::Math::Vec3f );
SERIALIZE_XML_DECLARE_VECTOR_3_WRAPPER ( Usul::Math::Vec3ui );

SERIALIZE_XML_DECLARE_VECTOR_2_WRAPPER ( Usul::Math::Vec2d );
SERIALIZE_XML_DECLARE_VECTOR_2_WRAPPER ( Usul::Math::Vec2f );
SERIALIZE_XML_DECLARE_VECTOR_2_WRAPPER ( Usul::Math::Vec2ui );


///////////////////////////////////////////////////////////////////////////////
//
//  Macros to declare stream operators for 4 by 4 matrix.
//
///////////////////////////////////////////////////////////////////////////////

#define SERIALIZE_XML_DEFINE_STREAM_FUNCTIONS_MATRIX_4_4(the_type)\
inline std::ostream &operator << ( std::ostream &out, const the_type &m )\
{\
  int w ( 20 );\
  int p ( 4 );\
\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 0, 0 );\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 1, 0 );\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 2, 0 );\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 3, 0 );\
  out << std::endl;\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 0, 1 );\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 1, 1 );\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 2, 1 );\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 3, 1 );\
  out << std::endl;\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 0, 2 );\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 1, 2 );\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 2, 2 );\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 3, 2 );\
  out << std::endl;\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 0, 3 );\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 1, 3 );\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 2, 3 );\
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m( 3, 3 );\
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

#endif // _SERIALIZE_XML_SET_VALUE_H_
