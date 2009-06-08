
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_NETWORK_LINK_H__
#define __MINERVA_LAYERS_KML_NETWORK_LINK_H__

#include "Minerva/Core/Data/Feature.h"
#include "Minerva/Layers/Kml/Link.h"

namespace Minerva {
namespace Layers {
namespace Kml {
  

class NetworkLink : public Minerva::Core::Data::Feature
{
public:
  // Typedefs.
  typedef Feature BaseClass;
  
  // Declare pointers.
  USUL_DECLARE_REF_POINTERS ( NetworkLink );
  
  NetworkLink();
  NetworkLink( const XmlTree::Node& );
  
  /// Get/Set the link.
  Link*       link() const;
  void        link( Link* );
  
protected:
  virtual ~NetworkLink();
  
private:
  Link::RefPtr _link;
};


}
}
}


#endif // __MINERVA_LAYERS_KML_NETWORK_LINK_H__
