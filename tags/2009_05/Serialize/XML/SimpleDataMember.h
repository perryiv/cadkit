
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
//  Class that wraps a simple data member.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SERIALIZE_XML_SIMPLE_DATA_MEMBER_CLASS_
#define _SERIALIZE_XML_SIMPLE_DATA_MEMBER_CLASS_

#include "Serialize/XML/MemberBase.h"
#include "Serialize/XML/TypeWrapper.h"

#include "Usul/Convert/Convert.h"


namespace Serialize {
namespace XML {


template < class T > class SimpleDataMember : public MemberBase 
{
public:

  USUL_DECLARE_REF_POINTERS ( SimpleDataMember );
  typedef MemberBase BaseClass;

  SimpleDataMember ( const std::string &name, T &value ) : BaseClass ( name ),
    _value ( value )
  {
  }

  virtual void serialize ( XmlTree::Node &parent ) const
  {
    T temp ( _value );

    // Make node with processed string.
    XmlTree::Node::ValidRefPtr node ( new XmlTree::Node ( this->name() ) );
    Serialize::XML::TypeWrapper<T>::serialize ( temp, *node );

    parent.children().push_back ( node );
  }

  virtual void deserialize ( const XmlTree::Node &node )
  {
    if ( this->name() == node.name() )
    {
      const std::string s ( node.value() );

      // Set value from string.
      Serialize::XML::TypeWrapper<T>::set ( s, _value );
    }
  }

protected:

  virtual ~SimpleDataMember()
  {
  }

private:

  T &_value;
};


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_SIMPLE_DATA_MEMBER_CLASS_
