
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
//  Classes and functions for serialization.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SERIALIZE_XML_DESERIALIZE_FUNCTIONS_H_
#define _SERIALIZE_XML_DESERIALIZE_FUNCTIONS_H_

#include "XmlTree/Document.h"
#include "Serialize/XML/Factory.h"

#include "Usul/Predicates/FileExists.h"
#include "Usul/File/Temp.h"

namespace Serialize {
namespace XML {


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize the objects.
//
///////////////////////////////////////////////////////////////////////////////

template < class Container > inline void deserialize ( const XmlTree::Node &parent, Container &c )
{
  typedef typename Container::value_type PointerType;
  typedef typename PointerType::element_type ObjectType;
  typedef typename XmlTree::Node::Children::const_iterator Itr;
  for ( Itr i = parent.children().begin(); i != parent.children().end(); ++i )
  {
    XmlTree::Node::RefPtr node ( i->get() );
    PointerType object ( dynamic_cast < ObjectType * > ( Factory::instance().create ( node->name() ) ) );
    object->deserialize ( *node );
    c.push_back ( object );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize the objects.
//
///////////////////////////////////////////////////////////////////////////////

template < class Container > inline void deserialize ( const std::string &file, Container &c )
{
  if ( Usul::Predicates::FileExists::test ( file ) )
  {
    Usul::File::Temp temp;
    deserialize ( temp.name(), c );
  }
  else
  {
    XmlTree::XercesLife life;
    XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
    document->load ( file );
    deserialize ( *document, c );
  }
}


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_DESERIALIZE_FUNCTIONS_H_
