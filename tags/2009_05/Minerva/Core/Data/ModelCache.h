
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Cache osg::Node base on key (usually filename or href).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DATA_MODEL_CACHE_H__
#define __MINERVA_CORE_DATA_MODEL_CACHE_H__

#include "Minerva/Core/Export.h"

#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Mutex.h"

#include "osg/Node"

#include <map>

namespace Minerva {
namespace Core {
namespace Data {


class MINERVA_EXPORT ModelCache
{
public:
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef osg::ref_ptr<osg::Node> NodePtr;
  typedef std::map<std::string,NodePtr> Cache;
  
  static ModelCache& instance();
  
  /// Construction/Destruction.
  ModelCache();
  ~ModelCache();
  
  /// Add the model.
  void               addModel ( const std::string& key, osg::Node* node );
  
  /// Clear the cache.
  void               clear();
  
  /// Is the model cached?
  bool               hasModel ( const std::string& key ) const;
  
  /// Get the model.
  osg::Node*         model ( const std::string& key ) const;
  
  /// Remove the model.
  void               removeModel ( const std::string& key );
  
  /// Get the mutex.
  Mutex&             mutex() const;
  
private:
  
  mutable Mutex _mutex;
  Cache _cache;
  
  static ModelCache *_instance;
};

      
}
}
}

#endif // __MINERVA_CORE_DATA_MODEL_CACHE_H__
