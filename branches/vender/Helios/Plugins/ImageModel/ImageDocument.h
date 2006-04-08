
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for Image models.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGE_MODEL_DOCUMENT_H_
#define _IMAGE_MODEL_DOCUMENT_H_

#include "Usul/Documents/Document.h"

#include "Usul/Interfaces/IImage.h"
#include "Usul/Interfaces/IFoxImage.h"

#include "osg/Image"
#include "osg/ref_ptr"

#include <string>
#include <vector>

class ImageDocument : public Usul::Documents::Document,
                      public Usul::Interfaces::IImage,
                      public Usul::Interfaces::IFoxImage
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef osg::ref_ptr<osg::Image> ImagePtr;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ImageDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  ImageDocument();

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &ext ) const;
  virtual bool                canInsert ( const std::string &ext ) const;
  virtual bool                canOpen   ( const std::string &ext ) const;
  virtual bool                canSave   ( const std::string &ext ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  /// Get the filters that correspond to what this document can read and write.
  virtual Filters             filtersExport() const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;

  /// Notify this document of the message.
  virtual void                notify ( unsigned short message );

  /// Read the file and add it to existing document's data.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0  ) const;

protected:

  /// Do not copy.
  ImageDocument ( const ImageDocument & );
  ImageDocument &operator = ( const ImageDocument & );

  /// Use reference counting.
  virtual ~ImageDocument();

  /// Usul::Interfaces::IImage
  virtual osg::Image* getImage() { return _image.get(); }
  virtual void        setImage( osg::Image* image) { _image = image; }

  /// Usul::Interfaces::IFoxImage
  virtual FX::FXImage* getFoxImage();

private:
  ImagePtr _image;
  

};


#endif // _IMAGE_MODEL_DOCUMENT_H_
