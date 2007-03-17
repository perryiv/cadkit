
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
//  Class that wraps a std::vector and std::list data members.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SERIALIZE_XML_STD_SEQUENCE_DATA_MEMBER_CLASS_
#define _SERIALIZE_XML_STD_SEQUENCE_DATA_MEMBER_CLASS_

#include "Serialize/XML/MemberBase.h"


namespace Serialize {
namespace XML {


template < class T > class PointerSequenceMember : public MemberBase 
{
public:

  USUL_DECLARE_REF_POINTERS ( PointerSequenceMember );
  typedef MemberBase BaseClass;

  PointerSequenceMember ( const std::string &name, T &value ) : BaseClass ( name ),
    _value ( value )
  {
  }

  virtual void serialize ( XmlTree::Node &parent ) const
  {
    typedef typename T::const_iterator Itr;

    XmlTree::Node::ValidRefPtr theSequence ( new XmlTree::Node ( this->name() ) );
    parent.children().push_back ( theSequence.get() );
    theSequence->children().reserve ( _value.size() );

    for ( Itr i = _value.begin(); i != _value.end(); ++i )
    {
      if ( true == i->valid() )
      {
        XmlTree::Node::ValidRefPtr element ( new XmlTree::Node ( "element" ) );
        element->attributes()["TypeName"] = (*i)->className();
        theSequence->children().push_back ( element.get() );
        (*i)->serialize ( *element );
      }
    }
  }

  virtual void deserialize ( const XmlTree::Node &node )
  {
    typedef typename XmlTree::Node::Children::const_iterator Itr;
    typedef typename T::value_type PointerType;
    typedef typename PointerType::element_type ObjectType;

    if ( this->name() != node.name() )
      return;

    if ( true == node.children().empty() )
      return;

    _value.clear();
    // _value.reserve ( node.children().size() ); Doesn't work for lists.

    for ( Itr i = node.children().begin(); i != node.children().end(); ++i )
    {
      XmlTree::Node::RefPtr element ( i->get() );
      if ( true == element.valid() && element->name() == "element" )
      {
        XmlTree::Node::Attributes::const_iterator j = element->attributes().find ( "TypeName" );
        if ( element->attributes().end() != j )
        {
          const std::string typeName ( j->second );
          PointerType object ( dynamic_cast < ObjectType * > ( Factory::instance().create ( typeName ) ) );
          if ( true == object.valid() )
          {
            // Ask object to populate itself.
            object->deserialize ( *element );

            // Add to sequence.
            _value.push_back ( object );
          }
        }
      }
    }
  }

protected:

  virtual ~PointerSequenceMember()
  {
  }

private:

  T &_value;
};


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_STD_SEQUENCE_DATA_MEMBER_CLASS_
