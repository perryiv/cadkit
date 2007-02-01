
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "PatentLayerGlue.h"

#include "Usul/Pointers/Pointers.h"
#include "Threads/OpenThreads/Mutex.h"
#include "Usul/Threads/Mutex.h"

using namespace DT::Minerva::Glue;

PatentLayerGlue::PatentLayerGlue() : BaseClass(),
_layer ( 0x0 )
{
  Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );
  _layer = new ::Minerva::Layers::PatentLayer;
  Usul::Pointers::reference ( _layer );
}

PatentLayerGlue::~PatentLayerGlue()
{
  Usul::Pointers::unreference( _layer );
  _layer = 0x0;
}

PatentLayerGlue::!PatentLayerGlue()
{
  Usul::Pointers::unreference( _layer );
  _layer = 0x0;
}

::Minerva::Layers::Layer* PatentLayerGlue::layer()
{
  return _layer;
}
