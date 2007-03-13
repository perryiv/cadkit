
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

#ifndef _SERIALIZE_XML_SERIALIZE_FUNCTIONS_H_
#define _SERIALIZE_XML_SERIALIZE_FUNCTIONS_H_

#include "XmlTree/Document.h"
#include "XmlTree/XercesLife.h"


namespace Serialize {
namespace XML {


///////////////////////////////////////////////////////////////////////////////
//
//  Serialize the objects.
//
///////////////////////////////////////////////////////////////////////////////

template < class Itr > inline void serialize ( XmlTree::Document &document, Itr first, Itr last )
{
  typedef typename Itr::value_type ValueType;
  for ( Itr i = first; i != last; ++i )
  {
    const ValueType &value ( *i );
    if ( true == value.valid() )
    {
      XmlTree::Node::ValidRefPtr node ( new XmlTree::Node ( value->className() ) );
      document.children().push_back ( node );
      value->serialize ( *node );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Serialize the objects.
//
///////////////////////////////////////////////////////////////////////////////

template < class Itr > inline void serialize ( const std::string &file, const std::string &name, Itr first, Itr last )
{
  XmlTree::XercesLife life;
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->name ( name );
  serialize ( *document, first, last );
  document->write ( file );
}


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_SERIALIZE_FUNCTIONS_H_
