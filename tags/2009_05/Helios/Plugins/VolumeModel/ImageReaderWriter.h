
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __HELIOS_VOLUME_MODEL_IMAGE_READER_WRITER_H__
#define __HELIOS_VOLUME_MODEL_IMAGE_READER_WRITER_H__

#include "Helios/Plugins/VolumeModel/IReaderWriter.h"

#include "OsgTools/Configure/OSG.h"

#include "Usul/Base/Object.h"

#include "osg/Image"
#include "osg/ref_ptr"

class ImageReaderWriter : public Usul::Base::Object,
                          public IReaderWriter
{
public:
  typedef Usul::Base::Object BaseClass;

  USUL_DECLARE_IUNKNOWN_MEMBERS;

  ImageReaderWriter();

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  /// Read the file and add it to existing document's data.
  virtual void                read ( const std::string &filename, VolumeDocument &doc, Unknown *caller = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, const VolumeDocument &doc, Unknown *caller = 0x0  ) const;

protected:
  virtual ~ImageReaderWriter();

  ImageReaderWriter ( const ImageReaderWriter& rhs );
  ImageReaderWriter& operator= ( const ImageReaderWriter& rhs );

private:
  typedef osg::ref_ptr<osg::Image> ImagePtr;
  typedef std::vector< ImagePtr >  ImageList;
  typedef std::string              Filename;
  typedef std::vector < Filename > Filenames;

  ImageList _imageList;
  Filenames _filenames;
};


#endif // __HELIOS_VOLUME_MODEL_IMAGE_READER_WRITER_H__
