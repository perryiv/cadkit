
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

#include "osg/BoundingBox"
#include "osg/Vec4"

#include <string>
#include <vector>
#include <iosfwd>

#include "Atom.h"
#include "Bond.h"
#include "MaterialChooser.h"

namespace osg { class Group; class LOD; class Geode; };


class ReaderWriterPDB : public osgDB::ReaderWriter
{
public:

  typedef std::vector<Atom> Atoms;
  typedef std::vector<Bond> Bonds;
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
  osg::LOD *              _makeAtom ( const Atom &atom ) const;
  osg::LOD *              _makeBond ( const Bond &bond) const;
  osg::Geode *            _makeSphere ( const osg::Vec3 &center, float radius, float detail ) const;
  osg::Geode *            _makeCube   ( const osg::Vec3 &center, float size ) const;
  Result                  _read ( const std::string &, const Options * );
  void                    _parse ( std::ifstream &in, int size );
  void                    _setCentersAndRanges ( osg::LOD *lod ) const;

private:

  Atoms _atoms;
  Bonds _bonds;
  MaterialChooser _materialChooser;
  float _maxDistanceFactor;
  float _lastRangeMax;
};


#endif // __READER_WRITER_PDB_H__
