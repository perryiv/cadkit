
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for flash files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FLASH_MODEL_DOCUMENT_H_
#define _FLASH_MODEL_DOCUMENT_H_

#include "Usul/Documents/Document.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"

#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ITimestepAnimation.h"
#include "Usul/Interfaces/ITimeVaryingData.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IMenuAdd.h"

#include "Serialize/XML/Macros.h"

#include "OsgTools/Volume/Texture3DVolume.h"
#include "OsgTools/Volume/TransferFunction.h"

#include "osg/BoundingBox"
#include "osg/MatrixTransform"
#include "osg/Image"

#include <string>
#include <vector>
#include <list>

class FlashDocument : public Usul::Documents::Document,
                      public Usul::Interfaces::IBuildScene
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document      BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( FlashDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  FlashDocument();
  
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

  /// Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

  // Deserialize.
  virtual void                deserialize ( const XmlTree::Node &node );
  
protected:

  /// Do not copy.
  FlashDocument ( const FlashDocument & );
  FlashDocument &operator = ( const FlashDocument & );

  /// Use reference counting.
  virtual ~FlashDocument();

private:
  
  typedef std::vector<std::string> Filenames;
  
  Filenames _filenames;
  unsigned int _currentTimestep;
  
  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_CLASS_NAME ( FlashDocument );
  SERIALIZE_XML_SERIALIZE_FUNCTION;
  SERIALIZE_XML_ADD_MEMBER_FUNCTION;
};



#endif // _FLASH_MODEL_DOCUMENT_H_
