
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for Paradis models.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PARADIS_MODEL_DOCUMENT_H_
#define _PARADIS_MODEL_DOCUMENT_H_

#include "FastLoadTriangleSet.h"

#include "Usul/Documents/Document.h"

#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IGetBoundingBox.h"
#include "Usul/Interfaces/IGroupPrimitives.h"

#include "Usul/Types/Types.h"

#include <string>

using namespace Usul::Types;


class ParadisDocument : public Usul::Documents::Document,
                        public Usul::Interfaces::IBuildScene,
                        public Usul::Interfaces::IGetBoundingBox,
                        public Usul::Interfaces::IGroupPrimitives
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef FastLoadTriangleSet TriangleSet;
  typedef Usul::Interfaces::IUnknown IUnknown;

  /// Type information.
  USUL_IMPLEMENT_INLINE_TYPE_ID ( ParadisDocument );

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ParadisDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  ParadisDocument();
  
  /// Add a triangle set.
  void                        addTriangleSet ( TriangleSet * );

  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &ext ) const;
  virtual bool                canInsert ( const std::string &ext ) const;
  virtual bool                canOpen   ( const std::string &ext ) const;
  virtual bool                canSave   ( const std::string &ext ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  /// Create default GUI
  virtual void                createDefaultGUI ( Unknown *caller = 0x0 );

  /// Get the filters that correspond to what this document can read and write.
  virtual Filters             filtersExport() const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;

  /// Get the bounding-box.
  virtual osg::BoundingBox    getBoundingBox() const;

  /// Read the file and add it to document's data.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename,  Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;
  
protected:

  /// Do not copy.
  ParadisDocument ( const ParadisDocument & );
  ParadisDocument &operator = ( const ParadisDocument & );

  /// Use reference counting.
  virtual ~ParadisDocument();

  /// Usul::Interfaces::IGroupPrimitives
  virtual void                groupPrimitives ( Usul::Interfaces::IUnknown *caller );
  virtual unsigned int        groupsNumber () const;
  virtual IUnknown*           getPrimitiveGroup ( unsigned int i );

  /// Usul::Interfaces::ISmoothModel
  virtual void                smoothModel ( );

  /// Usul::Interfaces::IDecimateModel
  virtual void                decimateModel ( );

  /// Usul::Interfaces::ISubdivideModel
  virtual void                subdivideModel ( unsigned int numSubdivisions );
private:

  typedef std::vector < std::string > Filenames;

  Filenames _filenames;
  TriangleSet::ValidRefPtr _triangles;  
};


#endif // _PARADIS_MODEL_DOCUMENT_H_
