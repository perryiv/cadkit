
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenStreetMap layer that reads from .osm file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_FILE_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_FILE_H__

#include "Minerva/Core/Data/Container.h"

#include "Usul/Interfaces/IRead.h"


namespace Minerva {
namespace Layers {
namespace OSM {

class OpenStreetMapFile : public Minerva::Core::Data::Container,
                          public Usul::Interfaces::IRead
{
public:
  
  /// Typedefs.
  typedef Minerva::Core::Data::Container BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( OpenStreetMapFile );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  OpenStreetMapFile();
  
  // Read the file.
  virtual void                read ( const std::string &filename, Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );

  // Deserialize.
  virtual void                deserialize ( const XmlTree::Node &node );

  /// Serialize.
  virtual void                serialize ( XmlTree::Node &parent ) const;

protected:

  virtual ~OpenStreetMapFile();

  // Read.
  void                        _read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress );

  // Set the bounds.
  void                        _setBounds ( const XmlTree::Node& node );

private:
  
  std::string _filename;

  SERIALIZE_XML_CLASS_NAME ( OpenStreetMapFile );
};

}
}
}


#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_FILE_H__
