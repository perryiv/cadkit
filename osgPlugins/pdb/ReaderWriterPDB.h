
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

#include "MaterialChooser.h"
#include "Molecule.h"
#include "PeriodicTable.h"

namespace osg { class Group; class LOD; class Geode; };


class ReaderWriterPDB : public osgDB::ReaderWriter
{
public:

  typedef osg::ref_ptr< Molecule > MoleculePtr;
  typedef std::vector< MoleculePtr > Molecules;
  typedef osgDB::ReaderWriter::ReadResult Result;
  typedef osgDB::ReaderWriter::Options Options;

  ReaderWriterPDB();
  ~ReaderWriterPDB();

  virtual const char*     className();
  virtual bool            acceptsExtension ( const std::string &extension );
  virtual Result          readNode ( const std::string &filename, const Options * );

protected:
  osg::Group *            _build() const;
  void                    _init();
  Result                  _read ( const std::string &, const Options * );
  void                    _parse ( std::ifstream &in );
  Molecule*               _getCurrentMolecule();

private:
  
  Molecules _molecules; 
  MaterialChooser _materialChooser;
  Molecule *_currentMolecule;
  PeriodicTable _periodicTable;
};


#endif // __READER_WRITER_PDB_H__
