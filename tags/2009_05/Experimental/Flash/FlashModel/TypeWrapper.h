
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FLASH_MODEL_TYPE_WRAPPER_H__
#define __FLASH_MODEL_TYPE_WRAPPER_H__

#include "Usul/Math/Vector4.h"

#include <vector>

///////////////////////////////////////////////////////////////////////////////
//
//  Specialization for std::vector<Usul::Math::Vec4uc>.
//
///////////////////////////////////////////////////////////////////////////////

namespace Serialize {
namespace XML {

  
template < > struct TypeWrapper < std::vector < Usul::Math::Vec4uc > >
{
  typedef TypeWrapper < std::vector < Usul::Math::Vec4uc > > ThisType;
  static void addAtribute ( const std::string &name, const std::string &value, XmlTree::Node &node )
  {
  }
  static const char *className ( const std::vector < Usul::Math::Vec4uc > &value )
  {
    return "std::vector<Usul::Math::Vec4uc>";
  }
  static std::vector<Usul::Math::Vec4uc> create ( const std::string &typeName )
  {
    return std::vector<Usul::Math::Vec4uc>();
  }
  static void deserialize ( const XmlTree::Node &node, std::vector < Usul::Math::Vec4uc >  &value )
  {
    typedef XmlTree::Node::Children::const_iterator Itr;

    value.clear();
    
    for ( Itr i = node.children().begin(); i != node.children().end(); ++i )
    {
      XmlTree::Node::RefPtr element ( i->get() );
      if ( true == element.valid() && element->name() == "element" )
      {
        Usul::Math::Vec4uc v;
    
        unsigned short a ( 0 ), b ( 0 ), c ( 0 ), d ( 0 );
        std::istringstream in ( element->value() );
        in >> a >> b >> c >> d;
        v.set ( static_cast<unsigned char> ( a ), static_cast<unsigned char> ( b ), static_cast<unsigned char> ( c ), static_cast<unsigned char> ( d ) );

        value.push_back ( v );
      }
    }
  }
  static void getAttribute ( const std::string &name, const XmlTree::Node &node, std::string &value )
  {
    value.clear();
  }
  static bool isValid ( const std::vector < Usul::Math::Vec4uc >  &value )
  {
    return true;
  }
  static void set ( const std::string &s, std::vector < Usul::Math::Vec4uc > &value )
  {
  }
  static void serialize ( const std::vector<Usul::Math::Vec4uc> &value, XmlTree::Node &node )
  {
    for ( std::vector<Usul::Math::Vec4uc>::const_iterator iter = value.begin(); iter != value.end(); ++iter )
    {
      XmlTree::Node::RefPtr child ( new XmlTree::Node ( "element" ) );\
    
      Usul::Math::Vec4uc v ( *iter );
    
      std::ostringstream out;
      out << static_cast<unsigned short> ( v[0] ); out << ' ';
      out << static_cast<unsigned short> ( v[1] ); out << ' ';
      out << static_cast<unsigned short> ( v[2] ); out << ' ';
      out << static_cast<unsigned short> ( v[3] );
      
      child->value ( out.str() );
      node.append ( child.get() );
    }
  }
};
  
  
}
}

#endif // __FLASH_MODEL_TYPE_WRAPPER_H__
