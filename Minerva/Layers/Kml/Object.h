
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
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include <string>

namespace XmlTree { class Node; }

namespace Minerva {
namespace Layers {
namespace Kml {

class Object : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  
  USUL_DECLARE_REF_POINTERS ( Object );
  
  Object();
  Object ( const XmlTree::Node& node );
  
  /// Get the mutex.
  Mutex &                mutex() const;
  
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
  mutable Mutex _mutex;
};


}
}
}

#endif // __MINERVA_LAYERS_KML_OBJECT_H__
