
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
#include "XmlTree/XercesLife.h"

#include "Usul/Factory/ObjectFactory.h"
#include "Usul/File/Temp.h"
#include "Usul/Predicates/FileExists.h"


namespace Serialize {
namespace XML {


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize the objects.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > inline void deserialize ( const XmlTree::Node &parent, std::vector< T > & c )
{
  typedef std::vector< T > Container;
  typedef typename Container::value_type PointerType;
  typedef typename PointerType::element_type ObjectType;
  typedef typename XmlTree::Node::Children::const_iterator Itr;
  typedef Usul::Factory::ObjectFactory Factory;
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
//  Deserialize single object.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > inline void deserialize ( const XmlTree::Node& parent, T& t )
{
  if( !parent.children().empty() )
  {
    XmlTree::Node::RefPtr node ( parent.children().begin()->get() );
    t.deserialize ( *node );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize the objects.
//
//  When the XML content is in a string (rather than a file), a temporary 
//  file is dropped and then read. See this link for a better way:
//  http://mail-archives.apache.org/mod_mbox/xerces-c-dev/200203.mbox/%3C31D2D4F05067D2119A8500805FC7819358EEC8@DBSRV1%3E
//
///////////////////////////////////////////////////////////////////////////////

template < class Container > inline void deserialize ( const std::string &file, Container &c )
{
  // If the string is not a file name...
  if ( false == Usul::Predicates::FileExists::test ( file ) )
  {
    // Make temporary file.
    Usul::File::Temp temp;

    // Assume string contains XML content and write it to file.
    temp.stream() << file;

    // Close the stream. The destructor will still remove the file.
    temp.close();

    // Call this function with the file name.
    deserialize ( temp.name(), c );
  }

  // The string is a file name.
  else
  {
    XmlTree::XercesLife life;
    XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
    document->load ( file );
    deserialize ( *document, c );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize the objects.
//
///////////////////////////////////////////////////////////////////////////////

template < class Container > inline void deserialize ( const std::istream &stream, Container &c )
{
  XmlTree::XercesLife life;
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->load ( stream );
  deserialize ( *document, c );
}


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_DESERIALIZE_FUNCTIONS_H_
