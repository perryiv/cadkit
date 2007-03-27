
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that handles image information.
//
///////////////////////////////////////////////////////////////////////////////

#include "ImageInfo.h"

#include "Usul/File/Temp.h"
#include "Usul/File/Remove.h"
#include "Usul/Exceptions/Thrower.h"

#include <fstream>
#include <algorithm>
#include <stdexcept>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

ImageInfo::ImageInfo ( const std::string &name ) : BaseClass(),
  _name  ( name ),
  _size  ( 0, 0, 0 ),
  _temp  (),
  _dirty ( true )
{
  // Read the file.
  Images::Image::ValidRefPtr image ( new Images::Image );
  image->read ( _name );

  // Set the size.
  _size.set ( image->rows(), image->columns(), image->layers() );

  // Make a temporary file and set our member.
  Usul::File::Temp temp ( Usul::File::Temp::BINARY );
  _temp = temp.name();

  // Write the image to the temporary file.
  image->write ( temp.stream() );

  // Release the temporary file. We will delete it.
  temp.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ImageInfo::~ImageInfo()
{
  // Remove the temporary file.
  Usul::File::remove ( _temp );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the given image.
//
///////////////////////////////////////////////////////////////////////////////

void ImageInfo::set ( Images::Image &image ) const
{
  // Open the binary file.
  std::ifstream in ( _temp.c_str(), std::ifstream::in | std::ifstream::binary );
  if ( !in )
    Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Error 2890459660: failed to open cached image file.",
        "\n\tImage name: ", _name,
        "\n\tCached image: ", _temp );

  // Create a larger file-io buffer.
  const unsigned long size ( 16384 ); // 2^14
  char buffer[size];
  
  // Set the buffer.
  in.rdbuf()->pubsetbuf ( buffer, size );

  // Read the image.
  image.read ( in );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the given image.
//
///////////////////////////////////////////////////////////////////////////////

void ImageInfo::set ( unsigned int size, unsigned char *pixels ) const
{
  // Handle bad input.
  if ( 0x0 == pixels || 0 == size )
    throw Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Error 2284143825: invalid image buffer given.",
        "\n\tImage name: ", _name,
        "\n\tCached image: ", _temp );

  // Make temporary image and set it.
  Images::Image::ValidRefPtr temp ( new Images::Image );
  this->set ( *temp );

  // Make sure the image is in the proper format.
  temp->channels ( 4 );
  temp->alpha ( true );
  temp->redGreenBlue();
  temp->type ( 1, true );

  // Get the data in the format we need.
  Images::Image::DataUint8 values;
  temp->getImageValues ( values );

  // Should be true.
  if ( values.empty() )
    throw Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Error 1668386074: cached image size is zero.",
        "\n\tImage name: ", _name,
        "\n\tCached image: ", _temp );

  // Should be true.
  if ( values.size() != size )
    throw Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Error 1792714223: given buffer size does not match cached image size.",
        "\n\tSize of given buffer: ", size,
        "\n\tSize of cached image: ", values.size(),
        "\n\tImage name: ", _name,
        "\n\tCached image: ", _temp );

  // Copy to the given image.
  std::copy ( pixels, pixels + values.size(), values.begin() );
}
