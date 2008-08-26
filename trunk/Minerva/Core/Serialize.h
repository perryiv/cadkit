
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_SERIALIZE_H__
#define __MINERVA_CORE_SERIALIZE_H__

#include "Minerva/Core/Data/Container.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

namespace Minerva {
namespace Core {


///////////////////////////////////////////////////////////////////////////////
//
//  Serialize the layer.
//
///////////////////////////////////////////////////////////////////////////////

template < class T >
inline std::string serialize( T *layer )
{
  std::vector< Usul::Interfaces::ISerialize::QueryPtr > v;
  v.push_back( layer );

  std::string contents;
  Serialize::XML::serialize( "Layers", v.begin(), v.end(), contents );

  return contents;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize the layer.
//
///////////////////////////////////////////////////////////////////////////////

inline Minerva::Core::Data::Container * deserialize ( const std::string& xml )
{
  std::vector< Minerva::Core::Data::Container::RefPtr > v;

  Serialize::XML::deserialize( xml, v );

  if( !v.empty() )
    return v.front().release();

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize the layer.
//
///////////////////////////////////////////////////////////////////////////////

inline void deserialize ( const std::string& xml, Minerva::Core::Data::Container *layer )
{
  Serialize::XML::deserialize( xml, *layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize the layer.
//
///////////////////////////////////////////////////////////////////////////////

inline Usul::Interfaces::IUnknown* deserializeCommand ( const std::string& xml )
{
  std::vector< Usul::Interfaces::ISerialize::QueryPtr > v;

  Serialize::XML::deserialize( xml, v );

  if( !v.empty() )
  {
    Usul::Interfaces::IUnknown::QueryPtr unknown ( v.front() );  // Ref count = 2
    v.clear();                                                   // Ref count 2 -> 1
    return unknown.release();                                    // Ref count 1->0
  }

  return 0x0;
}


}
}

#endif // __MINERVA_CORE_SERIALIZE_H__
