
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
//  Class that wraps a std::map data member.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SERIALIZE_XML_STD_VALUE_MAP_DATA_MEMBER_CLASS_
#define _SERIALIZE_XML_STD_VALUE_MAP_DATA_MEMBER_CLASS_

#include "Serialize/XML/MemberBase.h"

#include "Usul/Convert/Convert.h"

#include <map>


namespace Serialize {
namespace XML {


template < class T > class ValueMapMember : public MemberBase 
{
public:

  USUL_DECLARE_REF_POINTERS ( ValueMapMember );
  typedef MemberBase BaseClass;

  ValueMapMember ( const std::string &name, T &value ) : BaseClass ( name ),
    _value ( value )
  {
  }

  virtual void serialize ( XmlTree::Node &parent ) const
  {
    typedef typename T::const_iterator Itr;

    XmlTree::Node::ValidRefPtr thisMap ( new XmlTree::Node ( this->name() ) );
    parent.children().push_back ( thisMap.get() );
    thisMap->children().reserve ( _value.size() );

    for ( Itr i = _value.begin(); i != _value.end(); ++i )
    {
      XmlTree::Node::ValidRefPtr element ( new XmlTree::Node ( "element" ) );
      thisMap->children().push_back ( element.get() );
      element->children().reserve ( 2 );

      // Add key
      {
        XmlTree::Node::ValidRefPtr key ( new XmlTree::Node ( "key", i->first ) );
        element->children().push_back ( key.get() );
      }

      // Add value
      {
        XmlTree::Node::ValidRefPtr value ( new XmlTree::Node ( "value", i->second ) );
        element->children().push_back ( value.get() );
      }
    }
  }

  virtual void deserialize ( const XmlTree::Node &node )
  {
    typedef typename XmlTree::Node::Children::const_iterator Itr;
    typedef typename T::mapped_type MappedType;
    typedef typename T::key_type KeyType;

    if ( this->name() != node.name() )
      return;

    if ( true == node.children().empty() )
      return;

    _value.clear();

    for ( Itr i = node.children().begin(); i != node.children().end(); ++i )
    {
      XmlTree::Node::RefPtr element ( i->get() );
      if ( element->children().size() >= 2 )
      {
        XmlTree::Node::RefPtr elementKey   ( element->children()[0].get() );
        XmlTree::Node::RefPtr elementValue ( element->children()[1].get() );
        if ( true == elementKey.valid() && true == elementValue.valid() )
        {
          const std::string keyString ( elementKey->value() );
          if ( false == keyString.empty() )
          {
            const KeyType key ( Usul::Convert::Type<std::string,KeyType>::convert ( keyString ) );
            _value[key] = Usul::Convert::Type<std::string,MappedType>::convert ( elementValue->value() );
          }
        }
      }
    }
  }

protected:

  virtual ~ValueMapMember()
  {
  }

private:

  T &_value;
};


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_STD_VALUE_MAP_DATA_MEMBER_CLASS_
