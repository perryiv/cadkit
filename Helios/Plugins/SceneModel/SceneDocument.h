
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for scene files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_MODEL_DOCUMENT_H_
#define _SCENE_MODEL_DOCUMENT_H_

#include "Usul/Documents/Document.h"

#include "osg/Array"

#include "Usul/Interfaces/IFlipNormals.h"
#include "Usul/Interfaces/IBuildScene.h"

#include <string>

namespace osg { class Group; }

class SceneDocument : public Usul::Documents::Document,
                      public Usul::Interfaces::IFlipNormals,
                      public Usul::Interfaces::IBuildScene
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SceneDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  SceneDocument();

  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &file ) const;
  virtual bool                canInsert ( const std::string &file ) const;
  virtual bool                canOpen   ( const std::string &file ) const;
  virtual bool                canSave  ( const std::string &file ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  /// Get the filters that correspond to what this document can read and write.
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersExport() const;

  /// Flip the normals.
  virtual void                flipNormalVectors();

  /// Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

protected:

  osg::Node*                  _loadModel ( const std::string& filename, Unknown* caller, Unknown *progress );

  /// Do not copy.
  SceneDocument ( const SceneDocument & );
  SceneDocument &operator = ( const SceneDocument & );

  /// Use reference counting.
  virtual ~SceneDocument();

  osg::ref_ptr< osg::Group > _scene;
};



#endif // _POLYGON_MODEL_DOCUMENT_H_
