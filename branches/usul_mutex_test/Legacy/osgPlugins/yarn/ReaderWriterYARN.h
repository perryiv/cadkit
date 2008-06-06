
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __READER_WRITER_YARN_H__
#define __READER_WRITER_YARN_H__

#include "osgDB/ReaderWriter"

#include "OsgTools/Mesh.h"

#include <string>
#include <iosfwd>

namespace osg { class Group; class Node; };


class ReaderWriterYARN : public osgDB::ReaderWriter
{
public:

  typedef osgDB::ReaderWriter BaseClass;
  typedef osgDB::ReaderWriter::ReadResult ReadResult;
  typedef osgDB::ReaderWriter::Options Options;
  typedef osgDB::ReaderWriter::WriteResult WriteResult;
  typedef osg::Vec3f Vec3;
  typedef OsgTools::Mesh::value_type Vertex;
  typedef OsgTools::Mesh::value_type Normal;
  typedef std::vector<Vec3> CenterCurve;
  typedef std::pair<CenterCurve,OsgTools::Mesh> YarnData;
  typedef std::vector<YarnData> Yarns;

  ReaderWriterYARN();
  ~ReaderWriterYARN();

  virtual bool            acceptsExtension ( const std::string &extension );
  virtual const char*     className();
  virtual ReadResult      readNode ( const std::string &filename, const Options *options );

protected:

  osg::Group *            _build() const;
  void                    _init();
  osg::Node *             _makeFan ( const Vec3 &center, const OsgTools::Mesh &mesh, unsigned int stack ) const;
  void                    _parseYarns     ( std::ifstream &in );
  void                    _parseNumStacks ( std::ifstream& fin, unsigned int numYarns, unsigned int numSlices );
  void                    _parseStack     ( std::ifstream& fin, unsigned int numSlices, const osg::Vec3 &center );
  ReadResult              _read ( const std::string &, const Options * );

private:

  Yarns _yarns;
};


#endif // __READER_WRITER_YARN_H__
