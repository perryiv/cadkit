
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

#include "Serialize/XML/DataMember.h"

#include "XmlTree/Node.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

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
    this->_addMember ( new Serialize::XML::DataMember<T> ( name, value ) );
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
