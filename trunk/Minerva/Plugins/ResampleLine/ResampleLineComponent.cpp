
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/ResampleLine/ResampleLineComponent.h"
#include "Minerva/Plugins/ResampleLine/Resample.h"

#include "Usul/Components/Factory.h"

USUL_DECLARE_COMPONENT_FACTORY ( ResampleLineComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ResampleLineComponent, ResampleLineComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

ResampleLineComponent::ResampleLineComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

ResampleLineComponent::~ResampleLineComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ResampleLineComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IFitLineTerrain::IID:
    return static_cast < Usul::Interfaces::IFitLineTerrain*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IPlugin implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string ResampleLineComponent::getPluginName() const 
{
  return "Resample Line";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resample line to fit over terrain.
//
///////////////////////////////////////////////////////////////////////////////

void ResampleLineComponent::resample ( const Vertices& input, Vertices& output, unsigned int maximumDepth, Usul::Interfaces::IUnknown *caller )
{
  Detail::resample( input, output, maximumDepth, caller );
}
