
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
#include "Serialize/XML/TypeWrapper.h"


namespace Serialize {
namespace XML {


template < class T > class SequenceMember : public MemberBase 
{
public:

  USUL_DECLARE_REF_POINTERS ( SequenceMember );
  typedef MemberBase BaseClass;

  SequenceMember ( const std::string &name, T &value ) : BaseClass ( name ),
    _value ( value )
  {
  }

  virtual void serialize ( XmlTree::Node &parent ) const
  {
    typedef typename T::const_iterator Itr;
    typedef typename T::value_type ValueType;
    typedef Serialize::XML::TypeWrapper<ValueType> TypeWrapper;

    XmlTree::Node::ValidRefPtr theSequence ( new XmlTree::Node ( this->name() ) );
    parent.children().push_back ( theSequence.get() );
    theSequence->children().reserve ( _value.size() );

    for ( Itr i = _value.begin(); i != _value.end(); ++i )
    {
      if ( true == Serialize::XML::TypeWrapper<ValueType>::isValid ( *i ) )
      {
        XmlTree::Node::ValidRefPtr element ( new XmlTree::Node ( "element" ) );
        TypeWrapper::addAtribute ( "TypeName", TypeWrapper::className ( *i ), *element );
        theSequence->children().push_back ( element.get() );
        TypeWrapper::serialize ( *i, *element );
      }
    }
  }

  virtual void deserialize ( const XmlTree::Node &node )
  {
    typedef typename XmlTree::Node::Children::const_iterator Itr;
    typedef typename T::value_type ValueType;
    typedef Serialize::XML::TypeWrapper<ValueType> TypeWrapper;

    if ( this->name() != node.name() )
      return;

    if ( true == node.children().empty() )
      return;

    _value.clear();

    for ( Itr i = node.children().begin(); i != node.children().end(); ++i )
    {
      XmlTree::Node::RefPtr element ( i->get() );
      if ( true == element.valid() && element->name() == "element" )
      {
        std::string typeName;
        TypeWrapper::getAttribute ( "TypeName", *element, typeName );
        ValueType object ( TypeWrapper::create ( typeName ) );
        if ( true == TypeWrapper::isValid ( object ) )
        {
          // Ask object to populate itself.
          TypeWrapper::deserialize ( *element, object );

          // Add to sequence.
          _value.push_back ( object );
        }
      }
    }
  }

protected:

  virtual ~SequenceMember()
  {
  }

private:

  T &_value;
};


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_STD_SEQUENCE_DATA_MEMBER_CLASS_
