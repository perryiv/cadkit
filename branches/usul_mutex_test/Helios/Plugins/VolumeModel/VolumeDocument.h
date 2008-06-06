
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV and Adam Kubach
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

#include "Helios/Plugins/VolumeModel/IReaderWriter.h"

#include "OsgTools/Configure/OSG.h"
#include "OsgTools/Volume/TransferFunction.h"

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IUpdateListener.h"

#include "osg/Group"
#include "osg/Projection"
#include "osg/BoundingBox"
#include "osg/Image"
#include "osg/ref_ptr"

#include <string>
#include <vector>

class VolumeDocument : public Usul::Documents::Document,
                       public Usul::Interfaces::IBuildScene,
                       public Usul::Interfaces::IUpdateListener
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef OsgTools::Volume::TransferFunction  TransferFunction;
  typedef TransferFunction::RefPtr            TransferFunctionPtr;
  typedef std::vector < TransferFunctionPtr > TransferFunctions;

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

  /// Read the file and add it to existing document's data.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

  /// Get/Set the dirty flag.
  void                        dirty ( bool b );
  bool                        dirty () const;

  /// Get/Set the 3D image.
  void                        image3D ( osg::Image* );
  osg::Image*                 image3D () const;

  /// Get/Set the bounding box.
  void                        boundingBox ( const osg::BoundingBox& bb );
  osg::BoundingBox            boundingBox () const;

  /// Add a transfer function.
  void                        addTransferFunction ( TransferFunction* );

protected:

  /// Do not copy.
  VolumeDocument ( const VolumeDocument & );
  VolumeDocument &operator = ( const VolumeDocument & );

  /// Use reference counting.
  virtual ~VolumeDocument();

  void                        _buildScene ();

  /// Update (Usul::Interfaces::IUpdateListener).
  virtual void                             updateNotify ( Usul::Interfaces::IUnknown *caller );

private:
  osg::ref_ptr < osg::Group > _root;
  osg::ref_ptr < osg::Node > _node;
  osg::ref_ptr < osg::Projection > _projection;
  osg::ref_ptr < osg::Image > _image3D;
  osg::BoundingBox            _bb;
  IReaderWriter::RefPtr _readerWriter;
  bool _dirty;
  TransferFunctions _transferFunctions;
  unsigned int _activeTransferFunction;
};


#endif // _VOLUME_MODEL_DOCUMENT_H_
