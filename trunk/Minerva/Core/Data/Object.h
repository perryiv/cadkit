
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DATA_OBJECT_H__
#define __MINERVA_CORE_DATA_OBJECT_H__

#include "Minerva/Core/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/ISerialize.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "Serialize/XML/Macros.h"

#include <string>

namespace Minerva {
namespace Core {
namespace Data {

class MINERVA_EXPORT Object : public Usul::Base::Referenced,
                              public Usul::Interfaces::ISerialize
{
public:
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef std::string ObjectID;

  USUL_DECLARE_TYPE_ID ( Object );
  USUL_DECLARE_QUERY_POINTERS ( Object );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  /// Get the mutex.
  Mutex &                mutex() const;
  
  /// Get/set the id.
  const ObjectID&        objectId() const;
  void                   objectId( const ObjectID& );
  
  /// Get/set the target id.
  const std::string&     targetId() const;
  void                   targetId( const std::string& );
  
protected:
  
  Object();
  Object ( const Object& rhs );
  virtual ~Object();
  
private:
  
  ObjectID _id;
  std::string _targetId;
  mutable Mutex _mutex;
  SERIALIZE_XML_DEFINE_MEMBERS ( Object );
  SERIALIZE_XML_DEFINE_MAP;
};


}
}
}

#endif // __MINERVA_CORE_DATA_OBJECT_H__
