
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

#include "Serialize/XML/PointerMapMember.h"
#include "Serialize/XML/QueryPointerMember.h"
#include "Serialize/XML/SequenceMember.h"
#include "Serialize/XML/SimpleDataMember.h"
#include "Serialize/XML/SmartPointerMember.h"
#include "Serialize/XML/ValueMapMember.h"

#include "XmlTree/Node.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <list>
#include <map>
#include <string>
#include <vector>


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
  DataMemberMap ( const DataMemberMap & );
  ~DataMemberMap();

  template < class T > void addMember ( const std::string &name, T &value )
  {
    this->addMember ( new Serialize::XML::SimpleDataMember<T> ( name, value ) );
  }

  // Need to be by reference so that the client can change it after this class it made.
  template < class T, class C > void addMember ( const std::string &name, Usul::Pointers::QueryPointer<T,C> &value )
  {
    typedef Usul::Pointers::QueryPointer<T,C> PointerType;
    this->addMember ( new Serialize::XML::QueryPointerMember<PointerType> ( name, value ) );
  }

  // Need to be by reference so that the client can change it after this class it made.
  template < class T, class C > void addMember ( const std::string &name, Usul::Pointers::SmartPointer<T,C> &value )
  {
    typedef Usul::Pointers::SmartPointer<T,C> PointerType;
    this->addMember ( new Serialize::XML::SmartPointerMember<PointerType> ( name, value ) );
  }

  template < class K, class V, class C > void addMember ( const std::string &name, std::map<K,V,C> &value )
  {
    typedef std::map<K,V,C> MapType;
    this->addMember ( new Serialize::XML::PointerMapMember<MapType> ( name, value ) );
  }

  void addMember ( const std::string &name, std::map<std::string,std::string> &value )
  {
    typedef std::map<std::string,std::string> MapType;
    this->addMember ( new Serialize::XML::ValueMapMember<MapType> ( name, value ) );
  }

  template < class T > void addMember ( const std::string &name, std::vector<T> &value )
  {
    typedef std::vector<T> VectorType;
    this->addMember ( new Serialize::XML::SequenceMember<VectorType> ( name, value ) );
  }
/*  
  template < class T > void addMember ( const std::string &name, std::vector<std::vector<T> > &value )
  {
    typedef std::vector<T> VectorType;
    this->addMember ( new Serialize::XML::SequenceMember<Serialize::XML::SequenceMember< < T > > ( name, value ) );
  }
*/
  template < class T > void addMember ( const std::string &name, std::list<T> &value )
  {
    typedef std::list<T> ListType;
    this->addMember ( new Serialize::XML::SequenceMember<ListType> ( name, value ) );
  }

  void        addMember ( Serialize::XML::MemberBase *member );
  
  // Erase the data member with the given name.
  void        erase ( const std::string& name );

  void        serialize ( XmlTree::Node &parent ) const;
  void        deserialize ( const XmlTree::Node &node );

private:

  // No assignment.
  DataMemberMap &operator = ( const DataMemberMap & );

  Map _map;
};


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_DATA_MEMBER_MAP_CLASS_
