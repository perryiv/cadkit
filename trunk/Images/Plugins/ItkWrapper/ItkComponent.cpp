
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "ItkComponent.h"

#include "Usul/Errors/Assert.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ItkComponent, ItkComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ItkComponent::ItkComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ItkComponent::~ItkComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize data members.
//
///////////////////////////////////////////////////////////////////////////////

void ItkComponent::_init()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ItkComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IImageToGrayScaleUint8::IID:
    return static_cast < Usul::Interfaces::IImageToGrayScaleUint8*>(this);
  case Usul::Interfaces::IImageToGrayScaleUint16::IID:
    return static_cast < Usul::Interfaces::IImageToGrayScaleUint16*>(this);
  case Usul::Interfaces::IImageToGrayScaleUint32::IID:
    return static_cast < Usul::Interfaces::IImageToGrayScaleUint32*>(this);
  case Usul::Interfaces::IImageToGrayScaleFloat32::IID:
    return static_cast < Usul::Interfaces::IImageToGrayScaleFloat32*>(this);
  case Usul::Interfaces::IImageToGrayScaleFloat64::IID:
    return static_cast < Usul::Interfaces::IImageToGrayScaleFloat64*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the name of the plugin.
//
///////////////////////////////////////////////////////////////////////////////

std::string ItkComponent::getPluginName() const
{
  return "ITK Wrapper";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image values.
//
///////////////////////////////////////////////////////////////////////////////

void ItkComponent::toGrayScale ( unsigned int channels, DataUint8 &v ) const
{
  USUL_ASSERT ( 0 ); // TODO
}
void ItkComponent::toGrayScale ( unsigned int channels, DataUint16 &v ) const
{
  USUL_ASSERT ( 0 ); // TODO
}
void ItkComponent::toGrayScale ( unsigned int channels, DataUint32 &v ) const
{
  USUL_ASSERT ( 0 ); // TODO
}
void ItkComponent::toGrayScale ( unsigned int channels, DataFloat32 &v ) const
{
  USUL_ASSERT ( 0 ); // TODO
}
void ItkComponent::toGrayScale ( unsigned int channels, DataFloat64 &v ) const
{
  USUL_ASSERT ( 0 ); // TODO
}
