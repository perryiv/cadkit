
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

#include "Minerva/Core/Layers/LineLayer.h"
#include "Minerva/Core/Layers/PolygonLayer.h"
#include "Minerva/Core/Layers/PointLayer.h"
#include "Minerva/Core/Layers/PointTimeLayer.h"
#include "Minerva/Core/Layers/PolygonTimeLayer.h"
#include "Minerva/Core/Functors/SingleColorFunctor.h"
#include "Minerva/Core/Functors/GradientColorFunctor.h"

#include "Serialize/XML/TypeCreator.h"
#include "Serialize/XML/Factory.h"
#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

namespace Minerva {
namespace Core {

///////////////////////////////////////////////////////////////////////////////
//
//  Register types that can be serialized.
//
///////////////////////////////////////////////////////////////////////////////

inline void registerFactories()
{
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<Minerva::Core::Layers::LineLayer> ( "LineLayer" ) );
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<Minerva::Core::Layers::PolygonLayer> ( "PolygonLayer" ) );
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<Minerva::Core::Layers::PointLayer> ( "PointLayer" ) );
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<Minerva::Core::Layers::PointTimeLayer> ( "PointTimeLayer" ) );
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<Minerva::Core::Layers::PolygonTimeLayer> ( "PolygonTimeLayer" ) );
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<Minerva::Core::Functors::SingleColorFunctor> ( "SingleColorFunctor" ) );
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<Minerva::Core::Functors::GradientColorFunctor> ( "GradientColorFunctor" ) );
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<Minerva::Core::DB::Connection> ( "Connection" ) );

}

///////////////////////////////////////////////////////////////////////////////
//
//  Serialize the layer.
//
///////////////////////////////////////////////////////////////////////////////

template < class T >
inline std::string serialize( T *layer )
{
  registerFactories();
  std::vector< Usul::Interfaces::ISerialize::QueryPtr > v;
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
  registerFactories();
  std::vector< Minerva::Core::Layers::Layer::RefPtr > v;

  //std::istringstream in ( xml );
  Serialize::XML::deserialize( xml, v );

  if( !v.empty() )
    return v.front().release();

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


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize the layer.
//
///////////////////////////////////////////////////////////////////////////////

inline void deserialize ( const std::string& xml, Minerva::Core::Layers::Layer *layer )
{
  registerFactories();

  Serialize::XML::deserialize( xml, *layer );
}

}
}

#endif // __MINERVA_CORE_SERIALIZE_H__
