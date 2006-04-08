
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for voxel models.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VOXEL_MODEL_DOCUMENT_H_
#define _VOXEL_MODEL_DOCUMENT_H_

#include "ImageInfo.h"

#include "Usul/Documents/Document.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Interfaces/IBuildScene.h"

#include "osg/Texture3D"

#include <string>
#include <map>
#include <vector>

namespace osg { class Drawable; }


class VoxelDocument : public Usul::Documents::Document,
                      public Usul::Interfaces::IBuildScene
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef std::map < std::string, ImageInfo::RefPtr > ImageMap;
  typedef std::vector < ImageInfo::ValidRefPtr > ImageVector;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( VoxelDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  VoxelDocument();

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
  VoxelDocument ( const VoxelDocument & );
  VoxelDocument &operator = ( const VoxelDocument & );

  /// Use reference counting.
  virtual ~VoxelDocument();

  void                        _makeVector ( ImageVector &images );
  osg::Drawable *             _makePlanes ( const BaseClass::Options &options, Unknown *caller );

  void                        _setState ( osg::Node * );

  void                        _updateTexture3D ( const BaseClass::Options &options, Unknown *caller );

  Usul::Math::Vec3ui          _volumeSize ( ImageVector &images, Usul::Interfaces::IUnknown *caller ) const;

private:

  ImageMap _imageMap;
  osg::ref_ptr<osg::Texture3D> _texture;
};


#endif // _VOXEL_MODEL_DOCUMENT_H_
