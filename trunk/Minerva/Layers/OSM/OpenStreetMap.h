
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_H__

#include "Minerva/Core/Data/Container.h"

#include "Usul/Interfaces/IRead.h"
#include "Usul/Types/Types.h"

#include <vector>

namespace XmlTree { class Node; }

namespace Minerva {
namespace Layers {
namespace OSM {
  
class OpenStreetMap : public Minerva::Core::Data::Container,
                      public Usul::Interfaces::IRead
{
public:
  
  /// Typedefs.
  typedef Minerva::Core::Data::Container             BaseClass;
  
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( OpenStreetMap );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  OpenStreetMap();
  
  // Read the file.
  virtual void                read ( const std::string &filename, Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );
  
  // Deserialize.
  virtual void                deserialize( const XmlTree::Node &node );
  
  /// Serialize.
  virtual void                serialize ( XmlTree::Node &parent ) const;
  
protected:

  virtual ~OpenStreetMap();
  
  // Add a node.
  void                        _addNode ( const XmlTree::Node& node );
  
  // Add a way.
  void                        _addWay ( const XmlTree::Node& node );

  // Read.
  void                        _read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress );
  
  // Set the bounds.
  void                        _setBounds ( const XmlTree::Node& node );
  
private:

  class Node : public Usul::Base::Referenced
  {
  public:
    typedef Usul::Base::Referenced BaseClass;
    
    USUL_DECLARE_REF_POINTERS ( Node );
    
    Node();
    
  protected:
    
    virtual ~Node();
    
  private:
    Usul::Types::Uint64 _id;
    Usul::Math::Vec2d _location;
  };
  
  class Way : public Usul::Base::Referenced
  {
  public:
    
    typedef Usul::Base::Referenced BaseClass;
    
    USUL_DECLARE_REF_POINTERS ( Way );
    
    Way();
    
  protected:
    
    virtual ~Way();
  };
  
  std::string _filename;
  
  SERIALIZE_XML_CLASS_NAME ( OpenStreetMap );
};
  
}
}
}


#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_H__
