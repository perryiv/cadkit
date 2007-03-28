
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
//  Map of data-member wrappers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SERIALIZE_XML_DATA_MEMBER_MAP_CLASS_
#define _SERIALIZE_XML_DATA_MEMBER_MAP_CLASS_

#include "Serialize/XML/QueryPointerMember.h"
#include "Serialize/XML/SimpleDataMember.h"
#include "Serialize/XML/SmartPointerMember.h"
#include "Serialize/XML/PointerMapMember.h"
#include "Serialize/XML/SequenceMember.h"

#include "XmlTree/Node.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include <vector>
#include <list>

#include <string>
#include <map>


namespace Serialize {
namespace XML {


///////////////////////////////////////////////////////////////////////////////
//
//  Base creator class.
//
///////////////////////////////////////////////////////////////////////////////

class SERIALIZE_XML_EXPORT DataMemberMap
{
public:

  typedef std::map < std::string, Serialize::XML::MemberBase::RefPtr > Map;

  DataMemberMap();
  ~DataMemberMap();

  template < class T > void addMember ( const std::string &name, T &value )
  {
    this->_addMember ( new Serialize::XML::SimpleDataMember<T> ( name, value ) );
  }

  // Need to be by reference so that the client can change it after this class it made.
  template < class T, class C > void addMember ( const std::string &name, Usul::Pointers::QueryPointer<T,C> &value )
  {
    this->_addMember ( new Serialize::XML::QueryPointerMember<T, C> ( name, value ) );
  }


  // Need to be by reference so that the client can change it after this class it made.
  template < class T, class C > void addMember ( const std::string &name, Usul::Pointers::SmartPointer<T,C> &value )
  {
    typedef Usul::Pointers::SmartPointer<T,C> PointerType;
    this->_addMember ( new Serialize::XML::SmartPointerMember<PointerType> ( name, value ) );
  }

  template < class K, class V > void addMember ( const std::string &name, std::map<K,V> &value )
  {
    typedef std::map<K,V> MapType;
    this->_addMember ( new Serialize::XML::PointerMapMember<MapType> ( name, value ) );
  }

  template < class T > void addMember ( const std::string &name, std::vector<T> &value )
  {
    typedef std::vector<T> VectorType;
    this->_addMember ( new Serialize::XML::SequenceMember<VectorType> ( name, value ) );
  }

  template < class T > void addMember ( const std::string &name, std::list<T> &value )
  {
    typedef std::list<T> ListType;
    this->_addMember ( new Serialize::XML::SequenceMember<ListType> ( name, value ) );
  }

  void        serialize ( XmlTree::Node &parent ) const;
  void        deserialize ( const XmlTree::Node &node );

protected:

  void        _addMember ( Serialize::XML::MemberBase *member );

private:

  Map _map;
};


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_DATA_MEMBER_MAP_CLASS_
