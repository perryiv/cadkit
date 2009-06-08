
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __READER_WRITER_R3D_H__
#define __READER_WRITER_R3D_H__

#include "Export.h"

#include "Usul/Adaptors/ProgressBarUpdate.h"


#include "osgDB/ReaderWriter"

#include <string>
#include <iosfwd>
#include <vector>

namespace osg { class Group; };


class OSG_R3D_EXPORT ReaderWriterR3D : public osgDB::ReaderWriter
{
public:

  typedef osgDB::ReaderWriter BaseClass;
  typedef osgDB::ReaderWriter::ReadResult ReadResult;
  typedef osgDB::ReaderWriter::WriteResult WriteResult;
  typedef osgDB::ReaderWriter::Options Options;

  ReaderWriterR3D();
  ~ReaderWriterR3D();

  virtual bool            acceptsExtension ( const std::string &extension );
  virtual const char*     className();
  virtual ReadResult      readNode ( const std::string &filename, const Options *options );
  virtual WriteResult     writeNode ( const osg::Node &node, const std::string &filename, const Options *options );

  void                    init() { this->_init(); }
  void                    parse ( const std::string& filename, const Usul::Adaptors::ProgressBarUpdate& progress  ) { this->_parse( filename, progress ); }
  osg::Group*             build () const { return this->_build(); }

protected:

  osg::Group *            _build() const;

  void                    _check ( std::istream &in, const std::string &s ) const;

  void                    _init();

  void                    _parse ( const std::string &filename, const Usul::Adaptors::ProgressBarUpdate& progress  );

  ReadResult              _read ( const std::string &, const Options * );

  void                    _skipLine ( std::istream &in ) const;
  void                    _skipWord ( std::istream &in ) const;

  WriteResult             _write ( const osg::Node &node, const std::string &filename, const Options *options );

private:

  typedef std::vector < osg::Vec3f > Vectors;

  Vectors _vertices;
  Vectors _normals;
};


#endif // __READER_WRITER_R3D_H__
