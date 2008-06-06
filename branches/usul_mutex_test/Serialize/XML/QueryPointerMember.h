
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
//  Class that wraps a pointer data member.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SERIALIZE_XML_QUERY_POINTER_DATA_MEMBER_CLASS_
#define _SERIALIZE_XML_QUERY_POINTER_DATA_MEMBER_CLASS_

#include "Serialize/XML/MemberBase.h"

#include "Usul/Factory/ObjectFactory.h"
#include "Usul/Interfaces/ISerialize.h"

namespace Serialize {
namespace XML {


template < class T > class QueryPointerMember : public MemberBase 
{
public:

  USUL_DECLARE_REF_POINTERS ( QueryPointerMember );
  typedef MemberBase BaseClass;
  typedef Usul::Factory::ObjectFactory Factory;

  // Need to be by reference so that the client can change it after this class it made.
  QueryPointerMember ( const std::string &name, T &value ) : BaseClass ( name ),
    _value ( value )
  {
  }

  virtual void serialize ( XmlTree::Node &parent ) const
  {
    // Handle null smart-pointer.
    if ( true == _value.valid() )
    {
      XmlTree::Node::ValidRefPtr node ( new XmlTree::Node ( this->name() ) );

      Usul::Interfaces::ISerialize::QueryPtr iSerialize ( _value );

      if( iSerialize.valid() )
      {
        node->attributes()["TypeName"] = iSerialize->className();
        parent.children().push_back ( node.get() );
        iSerialize->serialize ( *node );
      }
    }
  }

  virtual void deserialize ( const XmlTree::Node &node )
  {
    if ( this->name() == node.name() )
    {
      if ( false == _value.valid() )
      {
        XmlTree::Node::Attributes::const_iterator j = node.attributes().find ( "TypeName" );
        if ( node.attributes().end() != j )
        {
          _value = dynamic_cast < typename T::element_type * > ( Factory::instance().create ( j->second ) );
        }
      }

      if ( true == _value.valid() )
      {
        Usul::Interfaces::ISerialize::QueryPtr iSerialize ( _value );

        if( iSerialize.valid() )
        {
          iSerialize->deserialize ( node );
        }
      }
    }
  }

protected:

  virtual ~QueryPointerMember()
  {
  }

private:

  // Need to be by reference so that the client can change it after this class it made.
  T &_value;
};


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_QUERY_POINTER_DATA_MEMBER_CLASS_
