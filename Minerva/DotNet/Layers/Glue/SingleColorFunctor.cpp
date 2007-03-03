
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "SingleColorFunctor.h"

#include "Minerva/Core/Functors/SingleColorFunctor.h"

using namespace DT::Minerva::Glue;

SingleColorFunctor::SingleColorFunctor() : _color ( nullptr )
{
}

Minerva::Core::Functors::BaseColorFunctor * SingleColorFunctor::colorFunctor()
{
  ::Minerva::Core::Functors::SingleColorFunctor::RefPtr functor ( new ::Minerva::Core::Functors::SingleColorFunctor );
  functor->color( this->toOsgColor( _color ) );

  return functor.release();
}
