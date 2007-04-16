
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

#ifndef _SERIALIZE_XML_STD_MAP_DATA_MEMBER_CLASS_
#define _SERIALIZE_XML_STD_MAP_DATA_MEMBER_CLASS_

#include "Serialize/XML/MemberBase.h"


namespace Serialize {
namespace XML {


template < class T > class PointerMapMember : public MemberBase 
{
public:

  USUL_DECLARE_REF_POINTERS ( PointerMapMember );
  typedef MemberBase BaseClass;

  PointerMapMember ( const std::string &name, T &value ) : BaseClass ( name ),
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

      // Add value (the smart-pointer)
      {
        typedef Serialize::XML::TypeWrapper< typename T::mapped_type > TypeWrapper;
        XmlTree::Node::ValidRefPtr value ( new XmlTree::Node ( "value" ) );
        TypeWrapper::addAtribute ( "TypeName", TypeWrapper::className ( i->second ), *value );
        element->children().push_back ( value.get() );
        TypeWrapper::serialize ( i->second, *value );
      }
    }
  }

  virtual void deserialize ( const XmlTree::Node &node )
  {
    typedef typename XmlTree::Node::Children::const_iterator Itr;
    typedef typename T::value_type ValueType;
    typedef typename T::mapped_type PointerType;
    typedef typename PointerType::element_type ObjectType;
    typedef Serialize::XML::TypeWrapper< PointerType > TypeWrapper;

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
          if ( elementKey->name() == "key" && elementValue->name() == "value" && elementValue->children().size() > 0 )
          {
            const std::string keyString ( elementKey->value() );
            if ( false == keyString.empty() )
            {
              std::string typeName ( "" );
              TypeWrapper::getAttribute ( "TypeName", *elementValue, typeName );
              PointerType object ( TypeWrapper::create ( typeName ) );
              if ( true == object.valid() )
              {
                // Ask object to populate itself.
                TypeWrapper::deserialize ( *elementValue, object );

                // Erase existing value so that insert statement always works.
                _value.erase ( keyString );

                // Use insert statement instead of [] operator because the 
                // smart-pointer may be a ValidRefPtr.
                _value.insert ( ValueType ( keyString, object ) );
              }
            }
          }
        }
      }
    }
  }

protected:

  virtual ~PointerMapMember()
  {
  }

private:

  T &_value;
};


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_STD_MAP_DATA_MEMBER_CLASS_
