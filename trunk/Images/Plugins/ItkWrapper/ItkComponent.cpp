
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

#include "Usul/Exceptions/Thrower.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Cast/Cast.h"

#include "itkImage.h"

#include <stdexcept>
#include <algorithm>
#include <iostream>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ItkComponent, ItkComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ItkComponent::ItkComponent() : BaseClass(),
  _dataUint8   (),
  _dataUint16  (),
  _dataUint32  (),
  _dataFloat32 (),
  _dataFloat64 (),
  _bytes       ( 0 ),
  _channels    ( 0 ),
  _floating    ( false ),
  _width       ( 0 ),
  _height      ( 0 )
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
  _dataUint8.clear();
  _dataUint16.clear();
  _dataUint32.clear();
  _dataFloat32.clear();
  _dataFloat64.clear();

  _bytes       = 0;
  _channels    = 0;
  _floating    = false;
  _width       = 0;
  _height      = 0;
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
  case Usul::Interfaces::IRead::IID:
    return static_cast < Usul::Interfaces::IRead*>(this);
  case Usul::Interfaces::IReadTIFF::IID:
    return static_cast < Usul::Interfaces::IReadTIFF*>(this);
  case Usul::Interfaces::IReadProperties::IID:
    return static_cast < Usul::Interfaces::IReadProperties*>(this);
  case Usul::Interfaces::IGetImageProperties::IID:
    return static_cast < Usul::Interfaces::IGetImageProperties*>(this);
  case Usul::Interfaces::IGetImageDataUint8::IID:
    return static_cast < Usul::Interfaces::IGetImageDataUint8*>(this);
  case Usul::Interfaces::IGetImageDataUint16::IID:
    return static_cast < Usul::Interfaces::IGetImageDataUint16*>(this);
  case Usul::Interfaces::IGetImageDataUint32::IID:
    return static_cast < Usul::Interfaces::IGetImageDataUint32*>(this);
  case Usul::Interfaces::IGetImageDataFloat32::IID:
    return static_cast < Usul::Interfaces::IGetImageDataFloat32*>(this);
  case Usul::Interfaces::IGetImageDataFloat64::IID:
    return static_cast < Usul::Interfaces::IGetImageDataFloat64*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of bytes per value.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int ItkComponent::getNumBytesPerValue() const
{
  return _bytes;
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
//  See if the pixel format is floating-point.
//
///////////////////////////////////////////////////////////////////////////////

bool ItkComponent::isValueFloatingPoint() const
{
  return _floating;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file properties.
//
///////////////////////////////////////////////////////////////////////////////

void ItkComponent::_readProperties ( TIFF *in, const std::string &filename )
{
  // Check input.
  USUL_ASSERT ( 0x0 != in );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file properties.
//
///////////////////////////////////////////////////////////////////////////////

void ItkComponent::readProperties ( const std::string &filename )
{
  // Initialize data members.
  this->_init();

  // Read properties.
  this->_readProperties ( in, filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the image.
//
///////////////////////////////////////////////////////////////////////////////

void ItkComponent::read ( const std::string &filename, Unknown *caller )
{
  // Initialize data members.
  this->_init();

  // Read properties.
  this->_readProperties ( in, filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image values.
//
///////////////////////////////////////////////////////////////////////////////

void ItkComponent::getImageValues ( DataUint8 &v ) const
{
  v.assign ( _dataUint8.begin(), _dataUint8.end() );
}
void ItkComponent::getImageValues ( DataUint16 &v ) const
{
  v.assign ( _dataUint16.begin(), _dataUint16.end() );
}
void ItkComponent::getImageValues ( DataUint32 &v ) const
{
  v.assign ( _dataUint32.begin(), _dataUint32.end() );
}
void ItkComponent::getImageValues ( DataFloat32 &v ) const
{
  v.assign ( _dataFloat32.begin(), _dataFloat32.end() );
}
void ItkComponent::getImageValues ( DataFloat64 &v ) const
{
  v.assign ( _dataFloat64.begin(), _dataFloat64.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image dimensions.
//
///////////////////////////////////////////////////////////////////////////////

void ItkComponent::getImageDimensions ( unsigned int &width, unsigned int &height, unsigned int &channels ) const
{
  width    = _width;
  height   = _height;
  channels = _channels;
}
