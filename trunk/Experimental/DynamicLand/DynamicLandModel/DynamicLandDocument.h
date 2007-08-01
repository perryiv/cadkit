
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
//  Document for dld files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DYNAMIC_LAND_MODEL_DOCUMENT_H_
#define _DYNAMIC_LAND_MODEL_DOCUMENT_H_

#include "Usul/Documents/Document.h"

#include "Usul/Interfaces/IBuildScene.h"

#include <string>

namespace osg { class Node; }


class DynamicLandDocument : public Usul::Documents::Document,
                            public Usul::Interfaces::IBuildScene
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DynamicLandDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  DynamicLandDocument();

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
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0  ) const;

protected:

  /// Do not copy.
  DynamicLandDocument ( const DynamicLandDocument & );
  DynamicLandDocument &operator = ( const DynamicLandDocument & );

  /// Use reference counting.
  virtual ~DynamicLandDocument();

private:

};



#endif // _DYNAMIC_LAND_MODEL_DOCUMENT_H_
