
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "SingleColorFunctor.h"

#include "Minerva/Layers/Functors/SingleColorFunctor.h"

using namespace DT::Minerva::Glue;

SingleColorFunctor::SingleColorFunctor() : _color ( nullptr )
{
}

Minerva::Layers::Functors::BaseColorFunctor * SingleColorFunctor::colorFunctor()
{
  ::Minerva::Layers::Functors::SingleColorFunctor::RefPtr functor ( new ::Minerva::Layers::Functors::SingleColorFunctor );
  functor->color( this->toOsgColor( _color ) );

  return functor.release();
}
