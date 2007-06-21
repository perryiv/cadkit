
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_REGISTER_FACTORIES_H__
#define __MINERVA_CORE_REGISTER_FACTORIES_H__

#include "Minerva/Core/Layers/LineLayer.h"
#include "Minerva/Core/Layers/PolygonLayer.h"
#include "Minerva/Core/Layers/PointLayer.h"
#include "Minerva/Core/Layers/PointTimeLayer.h"
#include "Minerva/Core/Layers/PolygonTimeLayer.h"
#include "Minerva/Core/Functors/SingleColorFunctor.h"
#include "Minerva/Core/Functors/GradientColorFunctor.h"
#include "Minerva/Core/Commands/StartAnimation.h"
#include "Minerva/Core/Commands/StopAnimation.h"
#include "Minerva/Core/Commands/AnimationSpeed.h"

#include "Serialize/XML/TypeCreator.h"
#include "Serialize/XML/Factory.h"

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
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<Minerva::Core::Commands::StopAnimation> ( "StopAnimation" ) );
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<Minerva::Core::Commands::AnimationSpeed> ( "AnimationSpeed" ) );
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<Minerva::Core::Commands::StartAnimation> ( "StartAnimation" ) );
}

}
}


#endif // __MINERVA_CORE_REGISTER_FACTORIES_H__
