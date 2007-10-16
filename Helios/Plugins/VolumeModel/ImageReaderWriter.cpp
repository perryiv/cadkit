
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/VolumeModel/ImageReaderWriter.h"
#include "Helios/Plugins/VolumeModel/ReaderWriterFactory.h"

#include "OsgTools/Images/Image3d.h"

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"

#include "osgDB/ReadFile"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ImageReaderWriter, ImageReaderWriter::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ImageReaderWriter::ImageReaderWriter() : BaseClass (),
_imageList(),
_filenames()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ImageReaderWriter::ImageReaderWriter ( const ImageReaderWriter& rhs ) : BaseClass ( rhs ),
_imageList( rhs._imageList ),
_filenames( rhs._filenames )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

ImageReaderWriter& ImageReaderWriter::operator= ( const ImageReaderWriter& rhs )
{
  BaseClass::operator= ( rhs );
  _imageList = rhs._imageList;
  _filenames = rhs._filenames;
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ImageReaderWriter::~ImageReaderWriter()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone this reader/writer.
//
///////////////////////////////////////////////////////////////////////////////

IReaderWriter* ImageReaderWriter::clone () const
{
  return new ImageReaderWriter ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this reader/writer can do it.
//
///////////////////////////////////////////////////////////////////////////////

bool ImageReaderWriter::handle ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "jpeg" || ext == "jpg" || ext == "tiff" || ext == "tif" || ext == "gif" || ext == "png" || ext == "bmp" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing data.
//
///////////////////////////////////////////////////////////////////////////////

void ImageReaderWriter::read ( const std::string &name, Unknown *caller )
{
  // Read the image.
  ImagePtr image ( osgDB::readImageFile ( name ) );
  if ( !image.valid() )
    throw std::runtime_error ( "Error 1350090608: Could not load image file: " + name );

  // Get the dimensions.
  const int width  ( image->s() );
  const int height ( image->t() );

  // Set the image name and push it into our list.
  image->setFileName ( name );
  _imageList.push_back ( image.get() );

  _filenames.push_back ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void ImageReaderWriter::write ( const std::string &filename, Unknown *caller ) const
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void ImageReaderWriter::clear ( Usul::Interfaces::IUnknown *caller )
{
  _imageList.clear();
  _filenames.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the 3D image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* ImageReaderWriter::image3D ()
{
  return OsgTools::Images::image3d ( _imageList, true, 1000 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ImageReaderWriter::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case IReaderWriter::IID:
    return static_cast < IReaderWriter * > ( this );
  default:
    return 0x0;
  }
}


/// Register this reader writer in the factory.
//RegisterReaderWriter registerRW ( new ImageReaderWriter() );
