
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __READER_WRITER_STL_H__
#define __READER_WRITER_STL_H__

#include "osgDB/ReaderWriter"

#include <string>
#include <list>
#include <map>

namespace osg { class Group; class Geode; };


class ReaderWriterSTL : public osgDB::ReaderWriter
{
public:
  typedef osgDB::ReaderWriter::ReadResult Result;
  typedef osgDB::ReaderWriter::Options Options;

  ReaderWriterSTL();
  ~ReaderWriterSTL();

  virtual bool            acceptsExtension ( const std::string &extension );
  virtual const char*     className();
  virtual Result          readNode ( const std::string &filename, const Options *options );


protected:

  osg::Group *            _build() const;
  void                    _init();
  void                    _parse ( std::ifstream &in );
  Result                  _read ( const std::string &, const Options * );

private:

  bool                    _isAscii ( const std::string &filename ) const;
  void                    _parseBinaryFile( std::ifstream &in );
  void                    _parseAsciiFile( std::ifstream &in );

  typedef std::list < osg::Vec3 > Vertices;
  typedef std::list < osg::Vec3 > Normals;
  typedef std::pair < Vertices, Normals > PolygonList;
  typedef std::map < unsigned int, PolygonList > Polygons;

  Polygons _polygons;
};


#endif // __READER_WRITER_STL_H__
