
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for GDAL images.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GDAL_READ_IMAGE_DOCUMENT_H_
#define _GDAL_READ_IMAGE_DOCUMENT_H_

#include "Usul/Documents/Document.h"

#include "Usul/Interfaces/IReadImageFile.h"
#include "Usul/Interfaces/IWriteImageFile.h"

#include <string>

class OGRSFDriver;
class OGRLayer;

class GDALReadImageDocument : public Usul::Documents::Document,
                              public Usul::Interfaces::IReadImageFile,
                              public Usul::Interfaces::IWriteImageFile
                                  
{
public:
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( GDALReadImageDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  GDALReadImageDocument();

  /// Return true if this document can do it.
  virtual bool             canExport ( const std::string &file ) const;
  virtual bool             canInsert ( const std::string &file ) const;
  virtual bool             canOpen   ( const std::string &file ) const;
  virtual bool             canSave  ( const std::string &file ) const;

  /// Clear any existing data.
  virtual void             clear ( Unknown *caller = 0x0 );

  /// Get the filters that correspond to what this document can read and write.
  virtual Filters          filtersOpen()   const;
  virtual Filters          filtersSave()   const;
  virtual Filters          filtersInsert() const;
  virtual Filters          filtersExport() const;
  
  /// Read the document.
  virtual void             read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void             write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

protected:

  /// Do not copy.
  GDALReadImageDocument ( const GDALReadImageDocument & );
  GDALReadImageDocument &operator = ( const GDALReadImageDocument & );

  /// Use reference counting.
  virtual ~GDALReadImageDocument();

	/// Read a file and return an image (IReadImageFile).
	virtual ImagePtr         readImageFile ( const std::string& file ) const;

	/// Write the file (IWriteImageFile).
  virtual void             writeImageFile ( const std::string& file, osg::Image & ) const;

private:

};



#endif // _GDAL_READ_IMAGE_DOCUMENT_H_
