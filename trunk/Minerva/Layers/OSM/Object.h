
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_OBJECT_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_OBJECT_H__

#include "Minerva/Layers/OSM/Export.h"

#include "Minerva/Core/Animate/Date.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Types/Types.h"

#include <map>

namespace Minerva {
namespace Layers {
namespace OSM {
  
  
class MINERVA_OSM_EXPORT Object : public Usul::Base::Referenced
{
public:
  
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Types::Uint64 IdType;
  typedef Minerva::Core::Animate::Date Date;
  typedef std::map<std::string,std::string> Tags;
  
  USUL_DECLARE_REF_POINTERS ( Object );

  // Get the id.
  IdType              id() const;

  // Get a tag.
  template<typename T>
  T                   tag ( const std::string& key ) const;

  // Get all the tags.
  const Tags&         tags() const;

  // Get the timestamp.
  const Date&         timestamp() const;

protected:
  
  Object();
  Object ( IdType id, const Date& timestamp, const Tags& tags );
  virtual ~Object();
  
private:

  IdType _id;
  Tags _tags;
  Date _timestamp;
};


// Get a tag.
template<typename T>
T Object::tag ( const std::string& key ) const
{
  Tags::const_iterator iter ( _tags.find ( key ) );
  if ( _tags.end() != iter )
    return Usul::Convert::Type<Tags::key_type,T>::convert ( iter->second );
  return T();
}

}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_OBJECT_H__
