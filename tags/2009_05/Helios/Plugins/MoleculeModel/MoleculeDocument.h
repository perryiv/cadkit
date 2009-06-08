
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for Molecule models.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MOLECULE_MODEL_DOCUMENT_H_
#define _MOLECULE_MODEL_DOCUMENT_H_

#include "Usul/Documents/Document.h"

#include "osg/ref_ptr"

#include "osgPlugins/pdb/Molecule.h"

#include "Usul/Interfaces/IBuildScene.h"

#include <string>

class MoleculeDocument : public Usul::Documents::Document,
                         public Usul::Interfaces::IBuildScene
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef osg::ref_ptr< osgPlugins::pdb::Molecule > MoleculePtr;
  typedef std::vector< MoleculePtr > Molecules;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MoleculeDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  MoleculeDocument();

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
  MoleculeDocument ( const MoleculeDocument & );
  MoleculeDocument &operator = ( const MoleculeDocument & );

  /// Use reference counting.
  virtual ~MoleculeDocument();

private:
  Molecules _molecules;

};


#endif // _MOLECULE_MODEL_DOCUMENT_H_
