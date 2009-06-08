
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

#include "TiffComponent.h"

#include "Usul/Exceptions/Thrower.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Cast/Cast.h"

#include "tiffio.h"

#include <stdexcept>
#include <algorithm>
#include <iostream>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TiffComponent, TiffComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class to close a file.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct CloseTiffFile
  {
    CloseTiffFile ( ::TIFF *file ) : _file ( file )
    {
    }
    ~CloseTiffFile()
    {
      if ( _file )
        ::TIFFClose ( _file );
    }
  private:
    ::TIFF *_file;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class to delete memory.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct FreeTiffBuffer
  {
    FreeTiffBuffer ( void *buf ) : _buf ( buf )
    {
    }
    ~FreeTiffBuffer()
    {
      if ( _buf )
        ::_TIFFfree ( _buf );
    }
  private:
    void *_buf;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class to set and reset the error and warning handlers.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct SetErrorHandlers
  {
    SetErrorHandlers() : 
      _error   ( ::TIFFSetErrorHandler   ( &SetErrorHandlers::_errorHandler ) ),
      _warning ( ::TIFFSetWarningHandler ( &SetErrorHandlers::_warningHandler ) )
    {
    }
    ~SetErrorHandlers()
    {
      ::TIFFSetErrorHandler   ( _error   );
      ::TIFFSetWarningHandler ( _warning );
    }
  protected:
    static void _errorHandler ( const char *module, const char *format, va_list args )
    {
      char message[16384];
      ::vsprintf ( message, format, args );
      std::cout << "Error 1421868931: In " << module << ", message = " << message << std::endl;
    }
    static void _warningHandler ( const char *module, const char *format, va_list args )
    {
      char message[16384];
      ::vsprintf ( message, format, args );
      std::cout << "Warning 2974278689: In " << module << ", message = " << message << std::endl;
    }
  private:
    TIFFErrorHandler _error;
    TIFFErrorHandler _warning;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TiffComponent::TiffComponent() : BaseClass(),
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

TiffComponent::~TiffComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize data members.
//
///////////////////////////////////////////////////////////////////////////////

void TiffComponent::_init()
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

Usul::Interfaces::IUnknown *TiffComponent::queryInterface ( unsigned long iid )
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

unsigned int TiffComponent::getNumBytesPerValue() const
{
  return _bytes;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the name of the plugin.
//
///////////////////////////////////////////////////////////////////////////////

std::string TiffComponent::getPluginName() const
{
  return "TIFF IO";
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the scalar values are integers.
//
///////////////////////////////////////////////////////////////////////////////

bool TiffComponent::isValueInteger() const
{
  return _integer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file properties.
//
///////////////////////////////////////////////////////////////////////////////

void TiffComponent::_readProperties ( TIFF *in, const std::string &filename )
{
  // Check input.
  USUL_ASSERT ( 0x0 != in );

  // Get the info.
  ::TIFFGetField ( in, TIFFTAG_IMAGEWIDTH,  &_columns  );
  ::TIFFGetField ( in, TIFFTAG_IMAGELENGTH, &_rows );
  ::TIFFGetField ( in, TIFFTAG_IMAGEDEPTH,  &_layers  );
  ::TIFFGetFieldDefaulted ( in, TIFFTAG_SAMPLESPERPIXEL, &_channels );

  // Get the photometrics.
  Usul::Types::Uint16 photometrics ( 0 );
  if ( 0 == ::TIFFGetField ( in, TIFFTAG_PHOTOMETRIC, &photometrics ) )
    throw std::runtime_error ( "Error 1840873790: failed to determine photometrics in tiff file: " + filename );

  // Set the number of bytes in a sample.
  Usul::Types::Uint16 bits ( 8 );
  ::TIFFGetField ( in , TIFFTAG_BITSPERSAMPLE, &bits );
  if ( 0 != ( bits % 8 ) )
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 3328034485: unsupported number of bits per value (", bits, ") in tiff file: ", filename );
  _bytes = bits / 8;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file properties.
//
///////////////////////////////////////////////////////////////////////////////

void TiffComponent::readProperties ( const std::string &filename )
{
  // Set/reset error and warning handlers. Default ones pop up a dialog.
  Detail::SetErrorHandlers reset;

  // Initialize data members.
  this->_init();

  // Open the file for reading.
  ::TIFF *in ( ::TIFFOpen ( filename.c_str(), "r" ) );
  if ( 0x0 == in )
    throw std::runtime_error ( "Error 2889662467: failed to open tiff file '" + filename + "' for reading" );

  // Automatically close the file.
  Detail::CloseTiffFile close ( in );

  // Read properties.
  this->_readProperties ( in, filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the image.
//
///////////////////////////////////////////////////////////////////////////////

void TiffComponent::read ( const std::string &filename, Unknown *caller, Unknown *progress )
{
  // Set/reset error and warning handlers. Default ones pop up a dialog.
  Detail::SetErrorHandlers reset;

  // Initialize data members.
  this->_init();

  // Open the file for reading.
  ::TIFF *in ( ::TIFFOpen ( filename.c_str(), "r" ) );
  if ( 0x0 == in )
    throw std::runtime_error ( "Error 3931345665: failed to open tiff file '" + filename + "' for reading" );

  // Automatically close the file.
  Detail::CloseTiffFile close ( in );

  // Read properties.
  this->_readProperties ( in, filename );

  // Only stripped data as of now...
  if ( ::TIFFIsTiled ( in ) )
    throw std::runtime_error ( "Error 2134041573: tiff file '" + filename + "' stores the data in tiles; can only handle data strips" );

  // Get the size data and allocate the buffer.
  tstrip_t numStrips ( ::TIFFNumberOfStrips ( in ) );
  tsize_t stripSize ( ::TIFFStripSize ( in ) );
  Usul::Types::Uint8 *buf ( USUL_UNSAFE_CAST ( Usul::Types::Uint8 *, ::_TIFFmalloc ( stripSize ) ) );

  // Automatically free the buffer.
  Detail::FreeTiffBuffer freeBuf ( buf );

  // Depending on the number of bytes per value...
  switch ( _bytes )
  {
    case 1:

      // Loop through the strips and append to our container.
      _dataUint8.reserve ( numStrips * stripSize );
      for ( tstrip_t i = 0; i < numStrips; ++i )
      {
        // Read the strip.
        ::TIFFReadEncodedStrip ( in, i, buf, (tsize_t) -1 );

        // Append the values to our container.
        std::copy ( buf, buf + stripSize, std::back_inserter ( _dataUint8 ) );
      }
      break;

    case 2:

      // Should be true.
      if ( ( stripSize % 2 ) != 0 )
      {
        Usul::Exceptions::Thrower<std::runtime_error> 
          ( "Error 3856814921: strip size of ", stripSize, " in file '", filename, 
            "' is not divisible by two, and since each value is 2 bytes, it should be" );
      }

      // Loop through the strips.
      _dataUint16.reserve ( numStrips * stripSize );
      for ( tstrip_t i = 0; i < numStrips; ++i )
      {
        // Read the strip.
        ::TIFFReadEncodedStrip ( in, i, buf, (tsize_t) -1 );

        // Every two 8-bit values is a single 16-bit value.
        for ( tsize_t i = 0; i < stripSize; i += 2 )
        {
          // Pack into a single 16-bit unsigned integer.
          Usul::Types::Uint16 value ( ( buf[i+1] & 0xff ) | ( buf[i] << 8 ) );

          // Append to our container.
          _dataUint16.push_back ( value );
        }
      }
      break;

    default:
      Usul::Exceptions::Thrower<std::runtime_error> 
        ( "Error 2372887844: unsupported number of bytes per value (", _bytes, ") in tiff file: ", filename );
  }

#if 0
#ifdef _DEBUG

  if ( !_dataUint16.empty() )
  {
    std::sort ( _dataUint16.begin(), _dataUint16.end() );
    _dataUint16.erase ( std::unique ( _dataUint16.begin(), _dataUint16.end() ), _dataUint16.end() );

    std::cout << "\nFile: " << filename << std::endl;
    std::cout << "\tUnique Values .... " << _dataUint16.size()  << std::endl;
    std::cout << "\tMin Value ........ " << _dataUint16.front() << std::endl;
    std::cout << "\tMax Value ........ " << _dataUint16.back()  << std::endl;
    std::cout << "\tHistogram:\n\t";
    std::copy ( _dataUint16.begin(), _dataUint16.end(), std::ostream_iterator<Usul::Types::Uint16> ( std::cout, ", " ) );
    std::cout << std::endl;
  }

#endif
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image values.
//
///////////////////////////////////////////////////////////////////////////////

void TiffComponent::getImageValues ( DataUint8 &v ) const
{
  v.assign ( _dataUint8.begin(), _dataUint8.end() );
}
void TiffComponent::getImageValues ( DataUint16 &v ) const
{
  v.assign ( _dataUint16.begin(), _dataUint16.end() );
}
void TiffComponent::getImageValues ( DataUint32 &v ) const
{
  v.assign ( _dataUint32.begin(), _dataUint32.end() );
}
void TiffComponent::getImageValues ( DataFloat32 &v ) const
{
  v.assign ( _dataFloat32.begin(), _dataFloat32.end() );
}
void TiffComponent::getImageValues ( DataFloat64 &v ) const
{
  v.assign ( _dataFloat64.begin(), _dataFloat64.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image dimensions.
//
///////////////////////////////////////////////////////////////////////////////

void TiffComponent::getImageDimensions ( unsigned int &rows, unsigned int &columns, unsigned int &layers, unsigned int &channels ) const
{
  rows     = _rows;
  columns  = _columns;
  layers   = _layers;
  channels = _channels;
}
