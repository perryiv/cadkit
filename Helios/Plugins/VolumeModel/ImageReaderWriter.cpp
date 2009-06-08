
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/VolumeModel/ImageReaderWriter.h"
#include "Helios/Plugins/VolumeModel/VolumeDocument.h"

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
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ImageReaderWriter::~ImageReaderWriter()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and add it to existing data.
//
///////////////////////////////////////////////////////////////////////////////

void ImageReaderWriter::read ( const std::string &name, VolumeDocument &doc, Unknown *caller )
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

  osg::ref_ptr < osg::Image > image3D ( OsgTools::Images::image3d ( _imageList, true, 1000 ) );
  doc.image3D ( image3D.get() );

  unsigned int numImages ( _imageList.size() );
  double zSize ( 1.0f / numImages );
  zSize *= 0.5;

  osg::BoundingBox bb ( -1.0, -1.0, -zSize, 1.0, 1.0, zSize );
  doc.boundingBox ( bb );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void ImageReaderWriter::write ( const std::string &filename, const VolumeDocument &doc, Unknown *caller ) const
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
