
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
#include <list>
#include <iosfwd>

namespace osg { class Group; class LOD; };


class ReaderWriterPDB : public osgDB::ReaderWriter
{
public:

  typedef std::pair<std::string,osg::Vec4> Atom;
  typedef std::list<Atom> Atoms;
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
  Result                  _read ( const std::string &, const Options * );
  void                    _parse ( std::ifstream &in );

private:

  Atoms _atoms;
  osg::BoundingBox _bbox;
};


#endif // __READER_WRITER_PDB_H__
