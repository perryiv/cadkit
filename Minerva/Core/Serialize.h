
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_SERIALIZE_H__
#define __MINERVA_CORE_SERIALIZE_H__

// This needs to be included before any boost/serialization headers.
#include "boost/archive/xml_oarchive.hpp"
#include "boost/archive/xml_iarchive.hpp"

#include "boost/serialization/extended_type_info_typeid.hpp"

#include "boost/serialization/serialization.hpp"
#include "boost/serialization/base_object.hpp"
#include "boost/serialization/nvp.hpp"
#include "boost/serialization/version.hpp"
#include "boost/serialization/map.hpp"

#include "Usul/Pointers/BoostSerialize.h"

#include "Minerva/Core/Layers/LineLayer.h"
#include "Minerva/Core/Layers/PolygonLayer.h"
#include "Minerva/Core/Layers/PointLayer.h"
#include "Minerva/Core/Layers/PointTimeLayer.h"
#include "Minerva/Core/Layers/RLayer.h"
#include "Minerva/Core/Layers/PolygonTimeLayer.h"
#include "Minerva/Core/Functors/SingleColorFunctor.h"
#include "Minerva/Core/Functors/GradientColorFunctor.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

namespace Minerva {
namespace Core {


///////////////////////////////////////////////////////////////////////////////
//
//  Register types that can be serialized.
//
///////////////////////////////////////////////////////////////////////////////

template < class Archive > void registerTypes( Archive& a )
{
  // Register types.
  a.register_type<Minerva::Core::Layers::LineLayer>();
  a.register_type<Minerva::Core::Layers::PolygonLayer>();
  a.register_type<Minerva::Core::Layers::PointLayer>();
  a.register_type<Minerva::Core::Layers::PointTimeLayer>();
  a.register_type<Minerva::Core::Layers::RLayer>();
  a.register_type<Minerva::Core::Functors::SingleColorFunctor>();
  a.register_type<Minerva::Core::Functors::GradientColorFunctor>();
  a.register_type<Minerva::Core::Layers::PolygonTimeLayer>();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Serialize the layer.
//
///////////////////////////////////////////////////////////////////////////////

inline std::string serialize( Minerva::Core::Layers::Layer *layer )
{
  std::vector< Minerva::Core::Layers::Layer::RefPtr > v;
  v.push_back( layer );

  std::string contents;
  Serialize::XML::serialize( "Layers", v.begin(), v.end(), contents );

  // Create the archive.
  /*std::ostringstream os;
  boost::archive::xml_oarchive oa ( os );

  registerTypes( oa );

  oa << boost::serialization::make_nvp( "Layer", layer );
  return os.str();*/

  return contents;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize the layer.
//
///////////////////////////////////////////////////////////////////////////////

inline Minerva::Core::Layers::Layer * deserialize ( const std::string& xml )
{
  std::vector< Minerva::Core::Layers::Layer::RefPtr > v;

  Serialize::XML::deserialize( xml, v );

  if( !v.empty() )
    return v.front().get();

  return 0x0;

  // Create the archive.
  //std::istringstream in ( xml );
  //boost::archive::xml_iarchive ia ( in );

  //registerTypes( ia );

  //// Deserialize.
  //Minerva::Core::Layers::Layer *layer ( 0x0 );
  //ia >> boost::serialization::make_nvp( "Layer", layer );
  //return layer;
}


}
}

#endif // __MINERVA_CORE_SERIALIZE_H__
