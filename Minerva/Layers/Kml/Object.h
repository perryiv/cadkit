
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_OBJECT_H__
#define __MINERVA_LAYERS_KML_OBJECT_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

namespace XmlTree { class Node; }

namespace Minerva {
namespace Layers {
namespace Kml {

class Object : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( Object );
  
  Object();
  Object ( const XmlTree::Node& node );
  
  /// Get/set the id.
  const std::string&     objectId() const;
  void                   objectId( const std::string& );
  
  /// Get/set the target id.
  const std::string&     targetId() const;
  void                   targetId( const std::string& );
  
protected:
  
  virtual ~Object();
  
private:
  
  std::string _id;
  std::string _targetId;
};


}
}
}

#endif // __MINERVA_LAYERS_KML_OBJECT_H__
