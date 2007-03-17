
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

#ifndef _SERIALIZE_XML_SMART_POINTER_DATA_MEMBER_CLASS_
#define _SERIALIZE_XML_SMART_POINTER_DATA_MEMBER_CLASS_

#include "Serialize/XML/MemberBase.h"


namespace Serialize {
namespace XML {


template < class T > class SmartPointerMember : public MemberBase 
{
public:

  USUL_DECLARE_REF_POINTERS ( SmartPointerMember );
  typedef MemberBase BaseClass;

  SmartPointerMember ( const std::string &name, T value ) : BaseClass ( name ),
    _value ( value )
  {
  }

  virtual void serialize ( XmlTree::Node &parent ) const
  {
    XmlTree::Node::ValidRefPtr node ( new XmlTree::Node ( this->name() ) );
    parent.children().push_back ( node.get() );
    _value->serialize ( *node );
  }

  virtual void deserialize ( const XmlTree::Node &node )
  {
    if ( this->name() == node.name() )
    {
      _value->deserialize ( node );
    }
  }

protected:

  virtual ~SmartPointerMember()
  {
  }

private:

  T _value; // T is a smart-pointer, so a copy is ok.
};


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_SMART_POINTER_DATA_MEMBER_CLASS_
