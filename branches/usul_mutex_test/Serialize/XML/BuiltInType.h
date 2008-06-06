///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SERIALIZE_XML_BUILT_IN_TYPE_H__
#define __SERIALIZE_XML_BUILT_IN_TYPE_H__

#include "XmlTree/Node.h"

#include <sstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to declare serialization and deserialization functions.
//
///////////////////////////////////////////////////////////////////////////////

#define SERIALIZE_XML_DECLARE_FUNCTIONS(TheType)\
namespace Serialize {\
namespace Xml {\
inline void serialize ( const std::string& name, XmlTree::Node &parent, const TheType& value )\
{\
  std::ostringstream out; \
  out << value;\
\
  parent.children().push_back ( new XmlTree::Node ( name, out.str() ) );\
}\
\
inline void deserialize ( const std::string& name, const XmlTree::Node &node, TheType& value )\
{\
  if ( name == node.name() )\
  {\
    std::istringstream in ( node.value() );\
    in >> value;\
  }\
}\
\
}\
}\


///////////////////////////////////////////////////////////////////////////////
//
//  Specializations built-in types.
//
///////////////////////////////////////////////////////////////////////////////

SERIALIZE_XML_DECLARE_FUNCTIONS ( char  );
SERIALIZE_XML_DECLARE_FUNCTIONS ( short );
SERIALIZE_XML_DECLARE_FUNCTIONS ( int   );
SERIALIZE_XML_DECLARE_FUNCTIONS ( long  );
SERIALIZE_XML_DECLARE_FUNCTIONS ( bool  );

SERIALIZE_XML_DECLARE_FUNCTIONS ( unsigned char  );
SERIALIZE_XML_DECLARE_FUNCTIONS ( unsigned short );
SERIALIZE_XML_DECLARE_FUNCTIONS ( unsigned int   );
SERIALIZE_XML_DECLARE_FUNCTIONS ( unsigned long  );

SERIALIZE_XML_DECLARE_FUNCTIONS ( float );
SERIALIZE_XML_DECLARE_FUNCTIONS ( double );


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

#define SERIALIZE_XML_DECLARE_VECTOR_4_FUNCTIONS(the_type)\
  SERIALIZE_XML_DEFINE_STREAM_FUNCTIONS_VECTOR_4(the_type);\
  SERIALIZE_XML_DECLARE_FUNCTIONS(the_type);

#define SERIALIZE_XML_DECLARE_VECTOR_3_FUNCTIONS(the_type)\
  SERIALIZE_XML_DEFINE_STREAM_FUNCTIONS_VECTOR_3(the_type);\
  SERIALIZE_XML_DECLARE_FUNCTIONS(the_type);

#define SERIALIZE_XML_DECLARE_VECTOR_2_FUNCTIONS(the_type)\
  SERIALIZE_XML_DEFINE_STREAM_FUNCTIONS_VECTOR_2(the_type);\
  SERIALIZE_XML_DECLARE_FUNCTIONS(the_type);


/////////////////////////////////////////////////////////////////////////////
//
//  Declare vector types.
//
/////////////////////////////////////////////////////////////////////////////

SERIALIZE_XML_DECLARE_VECTOR_4_FUNCTIONS ( Usul::Math::Vec4d );
SERIALIZE_XML_DECLARE_VECTOR_4_FUNCTIONS ( Usul::Math::Vec4f );

SERIALIZE_XML_DECLARE_VECTOR_3_FUNCTIONS ( Usul::Math::Vec3d );
SERIALIZE_XML_DECLARE_VECTOR_3_FUNCTIONS ( Usul::Math::Vec3f );

SERIALIZE_XML_DECLARE_VECTOR_2_FUNCTIONS ( Usul::Math::Vec2d );
SERIALIZE_XML_DECLARE_VECTOR_2_FUNCTIONS ( Usul::Math::Vec2f );


#endif // __SERIALIZE_XML_BUILT_IN_TYPE_H__
