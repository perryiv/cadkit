
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Internal class to provide sax xml parsing of line strings.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_LINE_STRING_PARSER_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_LINE_STRING_PARSER_H__

#include "Minerva/Layers/OSM/Common.h"
#include "Minerva/Layers/OSM/LineString.h"

#include "XmlTree/XercesString.h"

#include "Usul/Convert/Convert.h"

#include "xercesc/sax2/Attributes.hpp"
#include "xercesc/sax2/DefaultHandler.hpp"

#include <string>

namespace Minerva {
namespace Layers {
namespace OSM {
namespace Detail {

class LineStringParser : public xercesc::DefaultHandler
{
public:

  /// Typedefs.
  typedef xercesc::DefaultHandler BaseClass;
  typedef std::vector<LineString::RefPtr> Lines;
  typedef std::map<Node::IdType,Node::Location> LocationMap;
  typedef Minerva::Layers::OSM::Object OSMObject;
  typedef OSMObject::IdType IdType;
  typedef OSMObject::Date DateType;
  typedef OSMObject::Tags Tags;
  typedef LineString::NodeIds NodeIds;
  typedef LineString::Vertices Vertices;

  /// Constructor.
  LineStringParser ( Lines& lines );

  virtual ~LineStringParser();

  virtual void startElement ( 
    const XMLCh* const uri, 
    const XMLCh* const localname,  
    const XMLCh* const qname, 
    const xercesc::Attributes&	attributes );
    
  virtual void endElement (
    const XMLCh* const uri, 
    const XMLCh* const localname,
    const XMLCh* const qname );

private:

  template<class T>
  T _getAttribute ( const xercesc::Attributes& attributes, const std::string& key, const T& defaultValue = T() )
  {
    const XMLCh *value ( attributes.getValue ( XmlTree::fromNative ( key ).c_str() ) );
    if ( 0x0 != value )
    {
      return Usul::Convert::Type<std::string,T>::convert ( XmlTree::toNative ( value ) );
    }
    
    return defaultValue;
  }

  Lines& _lines;
  LocationMap _locations;

  IdType _currentId;
  DateType _currentDate;
  Tags _currentTags;
  Vertices _currentVertices;
  NodeIds _currentNodeIds;
};


}
}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_LINE_STRING_PARSER_H__
