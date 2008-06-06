
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __READER_WRITER_STL_H__
#define __READER_WRITER_STL_H__

#include "Export.h"

#include "Usul/Adaptors/ProgressBarUpdate.h"

#include "osgDB/ReaderWriter"

#include <string>
#include <list>
#include <map>
#include <iosfwd>

namespace osg { class Group; class Geode; };


class OSG_STL_EXPORT ReaderWriterSTL : public osgDB::ReaderWriter
{
public:

  typedef osgDB::ReaderWriter BaseClass;
  typedef osgDB::ReaderWriter::ReadResult Result;
  typedef osgDB::ReaderWriter::Options Options;
  typedef osgDB::ReaderWriter::WriteResult WriteResult;
  typedef Usul::Adaptors::ProgressBarUpdate Update;

  ReaderWriterSTL();

  virtual bool            acceptsExtension ( const std::string &extension );
  virtual const char*     className();
  virtual Result          readNode ( const std::string &filename, const Options *options );

  virtual WriteResult     writeNode(const osg::Node& node, const std::string& fileName, const Options* options);

  void                    init() { this->_init(); }
  void                    parse ( const std::string& filename, const Update& progress  ) { this->_read( filename, progress ); }
  osg::Group*             build () const { return this->_build(); }

  bool                    isAscii ( const std::string &filename ) const { return this->_isAscii ( filename ); }

  // Was last file read binary? Note: default is true.
  bool                    binary() const;
  void                    binary ( bool b );

protected:

  ~ReaderWriterSTL();

  osg::Group *            _build() const;
  void                    _init();

  void                    _read ( const std::string &, const Update& progress );

  WriteResult             _writeAscii  ( const osg::Node& node, const std::string& filename );
  WriteResult             _writeBinary ( const osg::Node& node, const std::string& filename );

private:

  bool                    _isAscii ( const std::string &filename ) const;
  void                    _parseBinaryFile ( std::istream &in, const Update &progress );
  void                    _parseAsciiFile  ( std::istream &in, unsigned int filesize, const Update &progress );

  typedef std::list < osg::Vec3 > Vertices;
  typedef std::list < osg::Vec3 > Normals;
  typedef std::pair < Vertices, Normals > PolygonList;
  typedef std::map < unsigned int, PolygonList > Polygons;

  Polygons _polygons;
  bool _binary;
};


#endif // __READER_WRITER_STL_H__
