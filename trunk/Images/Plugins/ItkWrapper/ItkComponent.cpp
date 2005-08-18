
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

#include "ITK/Code/Common/itkImage.h"
#include "ITK/Code/Common/itkImageRegionIterator.h"
#include "ITK/Code/IO/itkImageFileReader.h"

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
  _integer     ( true ),
  _rows        ( 0 ),
  _columns     ( 0 ),
  _layers      ( 1 )
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
  _integer     = true;
  _rows        = 0;
  _columns     = 0;
  _layers      = 1;
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
  case Usul::Interfaces::IReadImage::IID:
    return static_cast < Usul::Interfaces::IReadImage*>(this);
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
//  See if the scalar values are integers.
//
///////////////////////////////////////////////////////////////////////////////

bool ItkComponent::isValueInteger() const
{
  return _integer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file properties.
//
///////////////////////////////////////////////////////////////////////////////

void ItkComponent::readProperties ( const std::string &filename )
{
  // For now...
  this->read ( filename, 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the image.
//
///////////////////////////////////////////////////////////////////////////////

void ItkComponent::read ( const std::string &filename, Unknown *caller )
{
  // Open as 8-bit rgba.
  typedef unsigned char PixelType;
  const unsigned int DIMENSION ( 3 ); // Volume
  typedef itk::Image < PixelType, DIMENSION > ImageType;
  typedef ImageType::PixelContainer::Element Buffer;
  typedef itk::ImageFileReader < ImageType > ReaderType;

  // Initialize data members.
  this->_init();

  // Open the file.
  ReaderType::Pointer reader ( ReaderType::New() );
  reader->SetFileName ( filename.c_str() );
  reader->Update();

  // Get the image.
  ImageType::Pointer image ( reader->GetOutput() );

  // Set these members.
  _bytes    = sizeof ( PixelType );
  _channels = DIMENSION;
  _integer  = true;
  _rows     = image->GetBufferedRegion().GetSize()[0];
  _columns  = ( image->GetBufferedRegion().GetSize().GetSizeDimension() > 1 ) ? image->GetBufferedRegion().GetSize()[1] : 1;
  _layers   = ( image->GetBufferedRegion().GetSize().GetSizeDimension() > 2 ) ? image->GetBufferedRegion().GetSize()[2] : 1;

  // Should be true.
  const unsigned int numPixels ( image->GetPixelContainer()->Size() );
  if ( ( _rows * _columns * _layers ) != numPixels )
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 3339041398: Number of pixel values (", numPixels, 
        ") is not the same as rows (", _rows, 
        ") times columns (", _columns, 
        ") times layers (", _layers, ")" );

  // Make room.
  _dataUint8.reserve ( numPixels * _channels );

  // Append the values to our container.
  typedef itk::ImageRegionConstIterator < ImageType > ConstIteratorType;
  ConstIteratorType itr ( image, image->GetLargestPossibleRegion() );
  for ( itr.GoToBegin(); !itr.IsAtEnd(); ++itr )
  {
    ImageType::PixelType pixel ( itr.Value() );
    _dataUint8.push_back ( pixel );
  }

  // Should be true.
  USUL_ASSERT ( ( numPixels * _channels ) == _dataUint8.size() );
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

void ItkComponent::getImageDimensions ( unsigned int &rows, unsigned int &columns, unsigned int &layers, unsigned int &channels ) const
{
  rows     = _rows;
  columns  = _columns;
  layers   = _layers;
  channels = _channels;
}
