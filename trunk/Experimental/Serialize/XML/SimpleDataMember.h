
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
#include "Serialize/XML/SetValue.h"


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
    parent.children().push_back ( new XmlTree::Node ( this->name(), _value ) );
  }

  virtual void deserialize ( const XmlTree::Node &node )
  {
    if ( this->name() == node.name() )
    {
      Serialize::XML::SetValue<T>::set ( node.value(), _value );
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
