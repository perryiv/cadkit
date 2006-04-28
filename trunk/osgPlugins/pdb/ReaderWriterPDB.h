
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __READER_WRITER_PDB_H__
#define __READER_WRITER_PDB_H__

#include "Usul/Interfaces/GUI/IProgressBar.h"

#include "osgDB/ReaderWriter"

#include "osg/Referenced"
#include "osg/ref_ptr"

#include <string>
#include <vector>
#include <iosfwd>

#include "Export.h"
#include "Molecule.h"
#include "PeriodicTable.h"


namespace osg { class Group; class LOD; class Geode; };


class OSG_PDB_EXPORT ReaderWriterPDB : public osgDB::ReaderWriter
{
public:

  typedef osgDB::ReaderWriter BaseClass;
  typedef osgPlugins::pdb::Molecule Molecule;
  typedef osg::ref_ptr< Molecule > MoleculePtr;
  typedef std::vector< MoleculePtr > Molecules;
  typedef osgDB::ReaderWriter::ReadResult ReadResult;
  typedef osgDB::ReaderWriter::WriteResult WriteResult;
  typedef osgDB::ReaderWriter::Options Options;
  typedef Molecule::MaterialFactory MaterialFactory;
  typedef Molecule::ShapeFactory ShapeFactory;
  typedef Molecule::Atom Atom;
  typedef Molecule::Bond Bond;
  typedef Usul::Interfaces::IProgressBar::UpdateProgressBar Update;

  ReaderWriterPDB();

  virtual bool            acceptsExtension ( const std::string &extension );

  virtual const char*     className();

  virtual ReadResult      readNode ( const std::string &filename, const Options * );

  // Show/hide the item.
  void                    showAtoms ( bool );
  void                    showBonds ( bool );

  // For working with flags that define the state.
  void                    addFlags ( unsigned int flags );
  unsigned int            getFlags() const { return _flags; }
  bool                    hasFlags ( unsigned int flags ) const;
  void                    setFlags ( unsigned int flags, bool state );
  void                    removeFlags ( unsigned int flags );

  void                    parse ( std::ifstream &in, unsigned int filesize, const Update& progress ) { _parse ( in, filesize, progress ); }

  void                    parsePsf ( std::ifstream &in ) { _parsePsf ( in ); }

  std::string             getPsfPath( const std::string &file ) { return _getPsfPath( file ); }
  osg::Group*             build() const { return _build(); }

  Molecules               getMolecules() { return _molecules; }

  void                    init() { this->_init(); }

protected:

  ~ReaderWriterPDB();

  osg::Group *            _build() const;

  void                    _configure ( const Options * );

  void                    _init();

  Molecule*               _getCurrentMolecule( unsigned int numAtoms = 0);
  void                    _getFileContents ( const std::string &filename, std::string &contents ) const;

  std::string             _getPsfPath ( const std::string &file);

  void                    _parse ( std::ifstream &in, unsigned int filesize, const Update& progress );
  void                    _parsePsf ( std::ifstream &in );

  ReadResult              _read ( const std::string &, const Options * );

private:
  
  mutable Molecules _molecules;
  MaterialFactory::Ptr _materialFactory;
  Molecule *_currentMolecule;
  ShapeFactory::Ptr _shapeFactory;
  PeriodicTable _periodicTable;
  unsigned int _flags;
};


#endif // __READER_WRITER_PDB_H__
