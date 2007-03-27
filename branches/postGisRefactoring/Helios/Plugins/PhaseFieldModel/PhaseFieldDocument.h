
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for Phase Field files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __PHASE_FIELD_DOCUMENT_H__
#define __PHASE_FIELD_DOCUMENT_H__

#include "Assembly.h"

#include "Usul/Documents/Document.h"

#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IGetBoundingBox.h"
#include "Usul/Interfaces/ITimeVaryingData.h"
#include "Usul/Interfaces/IAssemblyManager.h"

#include "osg/ref_ptr"

#include <vector>
#include <string>
#include <map>

class PhaseFieldDocument : public Usul::Documents::Document,
                           public Usul::Interfaces::IBuildScene,
                           public Usul::Interfaces::IGetBoundingBox,
                           public Usul::Interfaces::ITimeVaryingData,
                           public Usul::Interfaces::IAssemblyManager
{
public:
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PhaseFieldDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  PhaseFieldDocument();

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
  /// Use reference counting.
  virtual ~PhaseFieldDocument();

  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  // Usul::Interfaces::IGetBoundingBox
  virtual osg::BoundingBox    getBoundingBox() const;

  /// Usul::Interfaces::ITimeVaryingData
  virtual void             setCurrentTimeStep ( unsigned int current );
  virtual unsigned int     getCurrentTimeStep () const;

  virtual unsigned int     getNumberOfTimeSteps () const;

  /// Usul::Interfaces::IAssemblyManager
  virtual void                                getAssemblyNames ( std::vector< std::string >& names );
  virtual Usul::Interfaces::IAssembly*        getAssemblyByName ( const std::string& name );
  virtual void                                createAssembly ( const std::string& name );

private:

  /// Do not copy.
  PhaseFieldDocument ( const PhaseFieldDocument & );
  PhaseFieldDocument &operator = ( const PhaseFieldDocument & );

  osg::ref_ptr< osg::Node > _scene;

  typedef std::vector < std::string > Filenames;
  typedef std::map < std::string, Assembly::RefPtr > Assemblies;

  unsigned int _current;
  Filenames _filenames;

  Assemblies _assemblies;

};


#endif // __PHASE_FIELD_DOCUMENT_H__

