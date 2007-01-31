
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for Volume models.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VOLUME_MODEL_DOCUMENT_H_
#define _VOLUME_MODEL_DOCUMENT_H_

#include "Usul/Documents/Document.h"

#include "Usul/Interfaces/IImageList.h"
#include "Usul/Interfaces/IInitNewDocument.h"
#include "Usul/Interfaces/IBuildScene.h"

#include "osg/Image"
#include "osg/ref_ptr"

namespace osg { class Geometry; class Texture3D; }

#include <string>
#include <vector>

class VolumeDocument : public Usul::Documents::Document,
                       public Usul::Interfaces::IImageList,
                       public Usul::Interfaces::IInitNewDocument,
                       public Usul::Interfaces::IBuildScene
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef osg::ref_ptr<osg::Image> ImagePtr;
  typedef std::vector< ImagePtr > ImageList;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( VolumeDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  VolumeDocument();

  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

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
  VolumeDocument ( const VolumeDocument & );
  VolumeDocument &operator = ( const VolumeDocument & );

  /// Use reference counting.
  virtual ~VolumeDocument();

  void                        _applyTransferFunction ( osg::Image *image );
  double                      _calculateGradient ( unsigned int s, unsigned int t, unsigned int r, osg::Image *image );

  osg::Node*                  _buildScene2DTexture ( const BaseClass::Options&, Unknown* caller );
  osg::Node*                  _buildScene3DTexture ( const BaseClass::Options&, Unknown* caller );
  osg::Node*                  _buildSceneVoxels    ( const BaseClass::Options&, Unknown* caller );

  osg::Node*                  _makeCube( unsigned int numSlices, int width = 1, int height = 1 );

  osg::Texture3D*             _make3DTexture();

  /// Usul::Interfaces::IImageList
  virtual ImageList&          getImageList() { return _imageList; }

  /// Usul::Interfaces::IInitNewDocument
  virtual void                initNewDocument ( Usul::Interfaces::IUnknown *caller = 0x0 );

private:
  typedef std::string Filename;
  typedef std::vector < Filename > Filenames;

  ImageList _imageList;
  Filenames _filenames;

};


#endif // _VOLUME_MODEL_DOCUMENT_H_
