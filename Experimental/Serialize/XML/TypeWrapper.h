
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
#include "XmlTree/ReplaceIllegalCharacters.h"

#include "Usul/Pointers/Pointers.h"
#include "Usul/Types/Types.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector2.h"

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

SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( unsigned char  );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( unsigned short );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( unsigned int   );
SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( unsigned long  );

//SERIALIZE_XML_DECLARE_TYPE_WRAPPER ( Usul::Types::Uint32 );

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

SERIALIZE_XML_DECLARE_VECTOR_3_WRAPPER ( Usul::Math::Vec3d );
SERIALIZE_XML_DECLARE_VECTOR_3_WRAPPER ( Usul::Math::Vec3f );

SERIALIZE_XML_DECLARE_VECTOR_2_WRAPPER ( Usul::Math::Vec2d );
SERIALIZE_XML_DECLARE_VECTOR_2_WRAPPER ( Usul::Math::Vec2f );


#endif // _SERIALIZE_XML_SET_VALUE_H_
