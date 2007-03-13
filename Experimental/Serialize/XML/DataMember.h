
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
//  Class that wraps a data member.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SERIALIZE_XML_DATA_MEMBER_CLASS_
#define _SERIALIZE_XML_DATA_MEMBER_CLASS_

#include "Serialize/XML/MemberBase.h"
#include "Serialize/XML/SetValue.h"


namespace Serialize {
namespace XML {


template < class T > class DataMember : public MemberBase 
{
public:

  USUL_DECLARE_REF_POINTERS ( DataMember );
  typedef MemberBase BaseClass;

  DataMember ( const std::string &name, T &value ) : BaseClass ( name ),
    _value ( value )
  {
  }

  virtual void serialize ( XmlTree::Node &parent ) const
  {
    parent.children().push_back ( new XmlTree::Node ( this->name(), this->_getValue() ) );
  }

  virtual void deserialize ( const XmlTree::Node &node )
  {
    if ( this->name() == node.name() )
    {
      this->_setValue ( node.value() );
    }
  }

protected:

  virtual ~DataMember()
  {
  }

private:

  void _setValue ( const std::string &s )
  {
    Serialize::XML::SetValue<T>::set ( s, _value );
  }

  const T &_getValue() const
  {
    return _value;
  }

  T &_value;
};


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_DATA_MEMBER_CLASS_
