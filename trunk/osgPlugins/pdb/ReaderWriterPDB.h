
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __READER_WRITER_PDB_H__
#define __READER_WRITER_PDB_H__

#include "osgDB/ReaderWriter"

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

  typedef osg::ref_ptr< Molecule > MoleculePtr;
  typedef std::vector< MoleculePtr > Molecules;
  typedef osgDB::ReaderWriter::ReadResult Result;
  typedef osgDB::ReaderWriter::Options Options;
  typedef Molecule::MaterialFactory MaterialFactory;
  typedef Molecule::ShapeFactory ShapeFactory;

  ReaderWriterPDB();
  ~ReaderWriterPDB();

  virtual bool            acceptsExtension ( const std::string &extension );

  virtual const char*     className();

  virtual Result          readNode ( const std::string &filename, const Options * );

  // Show/hide the item.
  void                    showAtoms ( bool );
  void                    showBonds ( bool );

  // For working with flags that define the state.
  void                    addFlags ( unsigned int flags );
  unsigned int            getFlags() const { return _flags; }
  bool                    hasFlags ( unsigned int flags ) const;
  void                    setFlags ( unsigned int flags, bool state );
  void                    removeFlags ( unsigned int flags );

  void                    parse ( std::ifstream &in ) { _parse ( in ); }

  osg::Group*             build() const { return _build(); }

protected:

  osg::Group *            _build() const;

  void                    _configure ( const Options * );

  void                    _init();

  Molecule*               _getCurrentMolecule();
  void                    _getFileContents ( const std::string &filename, std::string &contents ) const;

  std::string             _getPsfPath ( const std::string &file);

  void                    _parse ( std::ifstream &in /*, std::ifstream &psf */);
  void                    _parsePsf ( std::ifstream &in );

  Result                  _read ( const std::string &, const Options * );

private:
  
  Molecules _molecules; 
  MaterialFactory::Ptr _materialFactory;
  Molecule *_currentMolecule;
  ShapeFactory::Ptr _shapeFactory;
  PeriodicTable _periodicTable;
  unsigned int _flags;
};


#endif // __READER_WRITER_PDB_H__
