
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
//  Abstract class that wraps a data member.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SERIALIZE_XML_MEMBER_BASE_CLASS_
#define _SERIALIZE_XML_MEMBER_BASE_CLASS_

#include "Serialize/XML/Export.h"

#include "XmlTree/Node.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>


namespace Serialize {
namespace XML {


class SERIALIZE_XML_EXPORT MemberBase : public Usul::Base::Referenced 
{
public:

  USUL_DECLARE_REF_POINTERS ( MemberBase );
  typedef Usul::Base::Referenced BaseClass;

  const std::string &name() const;

  virtual void serialize   ( XmlTree::Node &parent ) const;
  virtual void deserialize ( const XmlTree::Node &node );

protected:

  MemberBase ( const std::string &name );
  virtual ~MemberBase();

  std::string _name;
};


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_MEMBER_BASE_CLASS_
