
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
#include <list>
#include <map>

#include <iostream>
#include <fstream>

namespace osg { class Group; class Geode; };


class ReaderWriterYARN : public osgDB::ReaderWriter
{
public:
  typedef osgDB::ReaderWriter::ReadResult Result;
  typedef osgDB::ReaderWriter::Options Options;
  typedef osgDB::ReaderWriter::WriteResult WriteResult;

  ReaderWriterYARN();
  ~ReaderWriterYARN();

  virtual bool            acceptsExtension ( const std::string &extension );
  virtual const char*     className();
  virtual Result          readNode ( const std::string &filename, const Options *options );



protected:

  osg::Group *            _build() const;
  void                    _init();
  void                    _parse ( std::ifstream& );
  Result                  _read ( const std::string &, const Options * );


private:
  typedef OsgTools::Mesh::value_type Vertex;
  typedef std::vector< OsgTools::Mesh > Meshes;
  Meshes _meshes;

};


#endif // __READER_WRITER_YARN_H__
