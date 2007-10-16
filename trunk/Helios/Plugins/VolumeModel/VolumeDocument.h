
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

#include "Usul/Documents/Document.h"

#include "Usul/Interfaces/IBuildScene.h"

#include "osg/Image"
#include "osg/ref_ptr"

#include <string>
#include <vector>

class VolumeDocument : public Usul::Documents::Document,
                       public Usul::Interfaces::IBuildScene
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;

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
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0  ) const;

protected:

  /// Do not copy.
  VolumeDocument ( const VolumeDocument & );
  VolumeDocument &operator = ( const VolumeDocument & );

  /// Use reference counting.
  virtual ~VolumeDocument();

private:
  IReaderWriter::RefPtr _readerWriter;
};


#endif // _VOLUME_MODEL_DOCUMENT_H_
